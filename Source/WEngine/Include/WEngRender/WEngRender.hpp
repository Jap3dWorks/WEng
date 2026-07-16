#pragma once

#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WCore/WId.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WComponents/WStaticMeshComponent.hpp"
#include "WComponents/Light/WPointLightComponent.hpp"
#include "WComponents/Light/WDirectionalLightComponent.hpp"
#include "WComponents/Light/WAmbientLightComponent.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WAssets/Level.hpp"
#include "WRender/WRender.hpp"
#include "WRender/WLight.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include <cstdint>
#include <span>

namespace wng::render {

    inline void InitializeLights(
        IRender * in_render,
        was::Level * in_level,
        const WAssetDb & in_asset_db
        ) {

        // TODO It is required that Static Lights goes first than Dynamic lights.
        // id arrays like :
        // [SLight 1 , ..., SLight n, Dlight 1,...,Dlight m]
        // More info at [[org/WEng.org::#StaticDynamicLightsUBOUpdates]]

        // Point Lights

        decltype(wct::render::LightingUBO::point_lights) point_lights;
        std::array<wid::WEntityComponentId, point_lights.size()> pl_ids;
        std::uint32_t pl_count=0;

        in_level->ForEachComponent<wcm::light::WPointLightComponent>(
            [&in_level, &point_lights, &pl_ids, &pl_count]
            (wcm::light::WPointLightComponent * cmp) {
                if (cmp->Get_active()) {

                    auto * transform_component =
                        &in_level->GetComponent<WTransformComponent>
                        (cmp->Get_entity_id());

                    auto plight = wrd::light::ToPointLight(
                        *transform_component,
                        *cmp
                        );
                    
                    point_lights[pl_count] = plight;
                    pl_ids[pl_count] = in_level->
                        GetEntityComponentId<wcm::light::WPointLightComponent>(
                            cmp->Get_entity_id()
                            );
                    pl_count++;
                }
            }
            );

        // Directional Lights

        decltype(wct::render::LightingUBO::directional_lights) directional_lights;
        std::array<wid::WEntityComponentId, directional_lights.size()> dl_ids;
        std::uint32_t dl_count=0;

        in_level->ForEachComponent<wcm::light::WDirectionalLightComponent>(
            [&in_level, &directional_lights, &dl_ids, &dl_count]
            (wcm::light::WDirectionalLightComponent * cmp) {
                if (cmp->Get_active()) {

                    auto * transform_cmp = &in_level
                        ->GetComponent<WTransformComponent>
                        (cmp->Get_entity_id());

                    auto dlight = wrd::light::ToDirectionalLight(
                        *transform_cmp,
                        *cmp
                        );

                    dlight.direction = transform_cmp->Get_transform_matrix()[0];

                    directional_lights[dl_count] = dlight;

                    dl_ids[dl_count] = in_level->
                        GetEntityComponentId<wcm::light::WDirectionalLightComponent>(
                            cmp->Get_entity_id()
                            );
                    
                    dl_count++;
                }
            }
            );

        // Ambient Light

        wct::render::AmbientLight amb_light;
        in_level->ForEachComponent<wcm::light::WAmbientLightComponent>(
            [&amb_light](auto * cmp) {
                amb_light = wrd::light::ToAmbientLight(*cmp);
            }
            );
        
        in_render->InitializeLights(
            {pl_ids.begin(), pl_ids.begin() + pl_count},
            {point_lights.begin(), point_lights.begin() + pl_count},
            {dl_ids.begin(), dl_ids.begin() + dl_count},
            {directional_lights.begin(), directional_lights.begin() + dl_count},
            amb_light
            );

    }

    inline void InitializeResources(
        IRender * in_render,
        was::Level * in_level,
        const WAssetDb & in_asset_db
        ) {

        TSparseSet<wid::WAssetId> static_meshes;
        static_meshes.Reserve(64);
        TSparseSet<wid::WAssetId> texture_assets;
        texture_assets.Reserve(64);
        TSparseSet<wid::WAssetId> render_pipelines;
        render_pipelines.Reserve(64);

        in_level->ForEachComponent<WStaticMeshComponent>(
            [&static_meshes,
             &render_pipelines,
             &texture_assets,
             &in_asset_db](WStaticMeshComponent * in_component) {
                
                wid::WAssetId smid = in_component->Get_static_mesh_asset();
                
                if(smid.IsValid()) {
                    static_meshes.Insert(smid.GetId(), smid);
                }
                else {
                    WFLOG_Warning(
                        "Invalid WStaticMeshAsset in WStaticMeshComponent with id {}.",
                        in_component->Get_entity_id().GetId()
                        );
                }

                auto & smasset = in_asset_db.Get<WStaticMeshAsset>(smid);

                smasset.ForEachMesh(
                    [&in_component,
                     &render_pipelines,
                     &texture_assets,
                     &in_asset_db](WStaticMeshAsset * _sm,
                                   const wid::WSubIdxId & _id,
                                   wct::geometry::WMesh& _m) {

                        wct::render::RPipeAssignment pipassign =
                            in_component->GetPipelineAssignment(_id);
                        
                        if(pipassign.pipeline.IsValid() && pipassign.params.IsValid()) {
                            
                            render_pipelines.Insert(
                                pipassign.pipeline.GetId(), pipassign.pipeline
                                );
                            
                            auto & param_asset = in_asset_db.Get<WRenderPipelineParametersAsset>(pipassign.params);

                            auto texture_list = param_asset.Get_texture_list();

                            for(uint32_t i=0; i < texture_list.size(); i++)
                            {
                                wid::WAssetId tid = texture_list[i].value;
                                texture_assets.Insert(tid.GetId(), tid);
                            }
                            
                        }
                        else {
                            WFLOG_Warning(
                                "Invalid WRenderPipelineAsset in WStaticMeshComponent with id {}.",
                                in_component->Get_entity_id().GetId()
                                );
                            return;
                        }
                    }
                    );
            }
            );

        // Load Meshes
        for (const wid::WAssetId & id : static_meshes) {
            auto & static_mesh = in_asset_db.Get<WStaticMeshAsset>(id);
            
            static_mesh.ForEachMesh(
                [&in_render]
                (WStaticMeshAsset* _sma, const wid::WSubIdxId & _id, wct::geometry::WMesh& _m) {
                    
                    wid::WTypeAssetIndexId asset_index {{}, _sma->Get_asset_id(), _id};

                    in_render->LoadStaticMesh(asset_index, _m);
                }
                );
        }

        // Load Textures
        for (const wid::WAssetId & id : texture_assets) {
            auto & texture_asset = in_asset_db.Get<WTextureAsset>(id);
            in_render->LoadTexture(id, texture_asset);
        }

        // Initialize Render Pipelines
        for (const wid::WAssetId & id : render_pipelines) {
            auto & render_pipeline = in_asset_db.Get<WRenderPipelineAsset>(id);
            in_render->CreateRenderPipeline(&render_pipeline);
        }

        // Pipeline Bindings
        in_level->ForEachComponent<WStaticMeshComponent>(
            [&in_render,
             &in_asset_db,
             &in_level]
            (WStaticMeshComponent* in_component) {
                const wid::WAssetId & sm_id = in_component->Get_static_mesh_asset();
                WStaticMeshAsset & sm_asset = in_asset_db.Get<WStaticMeshAsset>(sm_id);

                sm_asset.ForEachMesh(
                    [&in_asset_db,
                     &in_level,
                     &in_render,
                     &in_component](WStaticMeshAsset* _sma, const wid::WSubIdxId & _id, wct::geometry::WMesh& _m) {
                        
                        auto & param = in_asset_db.Get<WRenderPipelineParametersAsset>(
                            in_component->GetPipelineAssignment(_id).params
                            );

                        wid::WEntityComponentId ecid = in_level->GetEntityComponentId<WStaticMeshComponent>(
                            in_component->Get_entity_id(), _id
                            );

                        wid::WTypeAssetIndexId assidx {
                            wid::null_id, _sma->Get_asset_id(), _id
                        };

                        in_render->CreatePipelineBinding(
                            ecid,
                            in_component->GetPipelineAssignment(_id).pipeline,
                            assidx,
                            param
                            );

                        auto * transform_component = &in_level
                            ->GetComponent<WTransformComponent>
                            (in_component->Get_entity_id());

                        wct::render::ModelUBO grpubo =
                          wrd::render::ToUBOGraphicsStruct(*transform_component);

                        std::uint8_t* ptr = reinterpret_cast<std::uint8_t*>(&grpubo);

                        wct::render::RPipeParamUbo ubodt{
                            .binding=wct::render::CommonBindings::MODEL_UBO,
                            .data=std::span<std::uint8_t>(
                                                          ptr,
                                                          ptr + sizeof(grpubo)),
                            .offset=0
                        };
                        
                        in_render->UpdateParameterStatic(ecid, ubodt);
                    }
                    );
            }
            );

        // Temporal solution, only one camera.
        // other cameras with RenderId() > 1 could render into textures.
        wid::WEntityId camera_entt{};
        in_level->ForEachComponent<WCameraComponent>(
            [&camera_entt](WCameraComponent * _cam){
                if (!camera_entt && _cam->Get_render_id().IsValid()) {
                    camera_entt = _cam->Get_entity_id();
                }
            }
            );

        if(camera_entt.IsValid()) {
            TSparseSet<wid::WAssetId> cam_render_pipelines;
            cam_render_pipelines.Reserve(WENG_MAX_ASSET_IDS);

            in_level->GetComponent<WCameraComponent>(camera_entt).ForEachPostprocessAssignment(
                [&cam_render_pipelines](
                     const WCameraComponent * _cmp,
                     const wid::WSubIdxId & _idx,
                     const auto & _assgn) {
                    cam_render_pipelines.Insert(_assgn.pipeline.GetId(), _assgn.pipeline);
                }
                );

            for (const wid::WAssetId & id : cam_render_pipelines) {
                auto & render_pipeline = in_asset_db.Get<WRenderPipelineAsset>(id);
                in_render->CreateRenderPipeline(&render_pipeline); // TODO Use the data struct
            }

            WCameraComponent & comp = in_level->GetComponent<WCameraComponent>(camera_entt);
            comp.ForEachPostprocessAssignment(
                [&in_level,
                 &in_render,
                 &in_asset_db](
                    const WCameraComponent * _cmp,
                    const wid::WSubIdxId & _idx,
                    const auto & _assgn
                    ) {
                    wid::WEntityComponentId ecid = in_level->GetEntityComponentId<WStaticMeshComponent>(
                        _cmp->Get_entity_id(), _idx
                        );

                    in_render->CreatePipelineBinding(
                        ecid,
                        _assgn.pipeline, {},
                        in_asset_db.Get<WRenderPipelineParametersAsset>(_assgn.params)
                        );
                }
                );
        }

        InitializeLights(in_render, in_level, in_asset_db);

        in_render->RefreshPipelines();
    }

    inline void ReleaseRenderResources(
        IRender * in_render,
        was::Level * in_level,
        const WAssetDb & in_asset_db
        ) {

        TSparseSet<wid::WAssetId> static_meshes;
        static_meshes.Reserve(64);
        TSparseSet<wid::WAssetId> texture_assets;
        texture_assets.Reserve(64);
        TSparseSet<wid::WEntityComponentId> pipeline_bindings;
        pipeline_bindings.Reserve(64);
        
        in_level->ForEachComponent<WStaticMeshComponent>(
            [&in_render,
             &in_asset_db,
             &static_meshes,
             &texture_assets,
             &pipeline_bindings,
             &in_level
                ](WStaticMeshComponent * _component) {

                static_meshes.Insert(
                    _component->Get_static_mesh_asset().GetId(),
                    _component->Get_static_mesh_asset()
                    );

                WStaticMeshAsset & sm_asset = in_asset_db.Get<WStaticMeshAsset>(
                    _component->Get_static_mesh_asset()
                    );

                sm_asset.ForEachMesh(
                    [&in_asset_db,
                     &_component,
                     &texture_assets,
                     &in_level,
                     &pipeline_bindings]
                    (WStaticMeshAsset * _sm, const wid::WSubIdxId & _id, wct::geometry::WMesh& _m) {

                        auto & pipeline_parameters =
                            in_asset_db.Get<WRenderPipelineParametersAsset>(
                                _component->GetPipelineAssignment(_id).params
                                );

                        // auto & parameters_struct = pipeline_parameters
                        //     ->RenderPipelineParameters();

                        auto texture_list = pipeline_parameters.Get_texture_list();

                        for(uint8_t i=0; i < texture_list.size(); i++) {
                            wid::WAssetId t_id = texture_list[i].value;
                    
                            texture_assets.Insert(
                                t_id.GetId(),
                                t_id
                                );
                        }

                        wid::WEntityComponentId ecid = in_level->GetEntityComponentId<WStaticMeshComponent>(
                            _component->Get_entity_id(), _id
                            );

                        pipeline_bindings.Insert(ecid.GetId(), ecid);
                    }
                    );
            }
            );
        
        for(auto & id : static_meshes) {
            in_asset_db.Get<WStaticMeshAsset>(id).ForEachMesh(
                [&in_render](WStaticMeshAsset * _sm, const wid::WSubIdxId & _id, wct::geometry::WMesh & _m) {
                    in_render->UnloadStaticMesh(
                        {wid::null_id, _sm->Get_asset_id(), _id}
                        );
                }
                );            
        }

        for(auto & id : texture_assets) {
            in_render->UnloadTexture(id);
        }

        for(auto & id : pipeline_bindings) {
            in_render->DeletePipelineBinding(id);
            
            // TODO: if a render pipeline has no bindings can be marked to unload.
        }
    }
}
