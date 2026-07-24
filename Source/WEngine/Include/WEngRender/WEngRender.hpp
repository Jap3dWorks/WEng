#pragma once

#include "WAssets/RenderPipelineParams.hpp"
#include "WAssets/RenderPipeline.hpp"
#include "WComponents/Camera.hpp"
#include "WComponents/Transform.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WCore/WId.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WComponents/StaticMesh.hpp"
#include "WComponents/Light/Point.hpp"
#include "WComponents/Light/Directional.hpp"
#include "WComponents/Light/Ambient.hpp"
#include "WAssets/StaticMesh.hpp"
#include "WAssets/Texture.hpp"
#include "WAssets/Level.hpp"
#include "WRender/WRender.hpp"
#include "WRender/WLight.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WCore/WDebug.hpp"

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
        std::array<wcr::wid::WEntityComponentId, point_lights.size()> pl_ids;
        std::uint32_t pl_count=0;

        in_level->ForEachComponent<wcm::light::Point>(
            [&in_level, &point_lights, &pl_ids, &pl_count]
            (wcm::light::Point * cmp) {
                if (cmp->Get_active()) {

                    auto * transform_component =
                        &in_level->GetComponent<wcm::Transform>
                        (cmp->Get_entity_id());

                    auto plight = wrd::light::ToPointLight(
                        *transform_component,
                        *cmp
                        );
                    
                    point_lights[pl_count] = plight;

                    pl_ids[pl_count] = {
                        in_level->Get_asset_id(),
                        cmp->Get_entity_id(),
                        in_level->GetComponentTypeId<wcm::light::Point>(),
                        wcr::wid::null_id
                    };

                    pl_count++;
                }
            }
            );

        // Directional Lights

        decltype(wct::render::LightingUBO::directional_lights) directional_lights;
        std::array<wcr::wid::WEntityComponentId, directional_lights.size()> dl_ids;
        std::uint32_t dl_count=0;

        in_level->ForEachComponent<wcm::light::Directional>(
            [&in_level, &directional_lights, &dl_ids, &dl_count]
            (wcm::light::Directional * cmp) {
                if (cmp->Get_active()) {

                    auto * transform_cmp = &in_level
                        ->GetComponent<wcm::Transform>
                        (cmp->Get_entity_id());

                    auto dlight = wrd::light::ToDirectionalLight(
                        *transform_cmp,
                        *cmp
                        );

                    dlight.direction = transform_cmp->Get_transform_matrix()[0];

                    directional_lights[dl_count] = dlight;

                    dl_ids[dl_count] = {
                        in_level->Get_asset_id(),
                        cmp->Get_entity_id(),
                        in_level->GetComponentTypeId<wcm::light::Directional>(),
                        wcr::wid::null_id
                    };
                    
                    dl_count++;
                }
            }
            );

        // Ambient Light

        wct::render::AmbientLight amb_light;
        in_level->ForEachComponent<wcm::light::Ambient>(
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

        TSparseSet<wcr::wid::WAssetId> static_meshes;
        static_meshes.Reserve(64);
        TSparseSet<wcr::wid::WAssetId> texture_assets;
        texture_assets.Reserve(64);
        TSparseSet<wcr::wid::WAssetId> render_pipelines;
        render_pipelines.Reserve(64);

        in_level->ForEachComponent<wcm::StaticMesh>(
            [&static_meshes,
             &render_pipelines,
             &texture_assets,
             &in_asset_db](wcm::StaticMesh * in_component) {
                
                wcr::wid::WAssetId smid = in_component->Get_static_mesh_asset();
                
                if(smid.IsValid()) {
                    static_meshes.Insert(smid.GetId(), smid);
                }
                else {
                    WFLOG_Warning(
                        "Invalid was::StaticMesh in wcm::StaticMesh with id {}.",
                        in_component->Get_entity_id().GetId()
                        );
                }

                auto & smasset = in_asset_db.Get<was::StaticMesh>(smid);

                smasset.ForEachMesh(
                    [&in_component,
                     &render_pipelines,
                     &texture_assets,
                     &in_asset_db](was::StaticMesh * _sm,
                                   const wcr::wid::WSubIdxId & _id,
                                   wct::geometry::WMesh& _m) {

                        wct::render::RPipeAssignment pipassign =
                            in_component->GetPipelineAssignment(_id);
                        
                        if(pipassign.pipeline.IsValid() && pipassign.params.IsValid()) {
                            
                            render_pipelines.Insert(
                                pipassign.pipeline.GetId(), pipassign.pipeline
                                );
                            
                            auto & param_asset = in_asset_db.Get<was::RenderPipelineParams>(pipassign.params);

                            auto texture_list = param_asset.Get_texture_list();

                            for(uint32_t i=0; i < texture_list.size(); i++)
                            {
                                wcr::wid::WAssetId tid = texture_list[i].value;
                                texture_assets.Insert(tid.GetId(), tid);
                            }
                            
                        }
                        else {
                            WFLOG_Warning(
                                "Invalid was::RenderPipeline in wcm::StaticMesh with id {}.",
                                in_component->Get_entity_id().GetId()
                                );
                            return;
                        }
                    }
                    );
            }
            );

        // Load Meshes
        for (const wcr::wid::WAssetId & id : static_meshes) {
            auto & static_mesh = in_asset_db.Get<was::StaticMesh>(id);
            
            static_mesh.ForEachMesh(
                [&in_render, &in_asset_db]
                (was::StaticMesh * _sma,
                 wcr::wid::WSubIdxId _id,
                 wct::geometry::WMesh & _m) {
                    
                    wcr::wid::WTypeAssetIndexId asset_index {
                        wcr::wid::null_id,
                        _sma->Get_asset_id(),
                        _id};

                    in_render->LoadStaticMesh(asset_index, _m);
                }
                );
        }

        // Load Textures
        for (const wcr::wid::WAssetId & id : texture_assets) {
            auto & texture_asset = in_asset_db.Get<was::Texture>(id);
            in_render->LoadTexture(id, texture_asset);
        }

        // Initialize Render Pipelines
        for (const wcr::wid::WAssetId & id : render_pipelines) {
            auto & render_pipeline = in_asset_db.Get<was::RenderPipeline>(id);
            in_render->CreateRenderPipeline(&render_pipeline);
        }

        // Pipeline Bindings
        in_level->ForEachComponent<wcm::StaticMesh>(
            [&in_render,
             &in_asset_db,
             &in_level]
            (wcm::StaticMesh* in_component) {

                was::StaticMesh & sm_asset = in_asset_db.Get<was::StaticMesh>(
                    in_component->Get_static_mesh_asset()
                    );

                sm_asset.ForEachMesh(
                    [&in_asset_db,
                     &in_level,
                     &in_render,
                     &in_component](was::StaticMesh * _sma,
                                    wcr::wid::WSubIdxId _id,
                                    wct::geometry::WMesh & _m) {
                        
                        auto & param = in_asset_db.Get<was::RenderPipelineParams>(
                            in_component->GetPipelineAssignment(_id).params
                            );

                        auto & pipeline = in_asset_db.Get<was::RenderPipeline>(
                            in_component->GetPipelineAssignment(_id).pipeline
                            );

                        wcr::wid::WEntityComponentId ecid =
                            {
                                in_level->Get_asset_id(),
                                in_component->Get_entity_id(),
                                in_level->GetComponentTypeId<wcm::StaticMesh>(),
                                _id
                            };

                        WCORE_DEBUG_ONLY(
                            WFLOG("ecid bitset: {}", std::bitset<64>(ecid.GetId()).to_string())
                            );
                        
                        wcr::wid::WTypeAssetIndexId assid {
                            wcr::wid::null_id,
                            _sma->Get_asset_id(),
                            _id
                        };

                        in_render->CreatePipelineBinding(
                            ecid,
                            assid,
                            pipeline,
                            param
                            );

                    });

                    auto * transform_component = &in_level
                        ->GetComponent<wcm::Transform>
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

                    // Update only the first static mesh model binding
                    wcr::wid::WEntityComponentId ecid {
                        in_level->Get_asset_id(),
                        in_component->Get_entity_id(),
                        in_level->GetComponentTypeId<wcm::StaticMesh>(),
                        {0}
                    };

                    WCORE_DEBUG_ONLY(
                        WFLOG("ecid bitset: {}", std::bitset<64>(ecid.GetId()).to_string())
                        );

                    in_render->UpdateParameterStatic(ecid, ubodt);
            }
            );

        // Temporal solution, only one camera.
        // other cameras with RenderId() > 1 could render into textures.
        wcr::wid::WEntityId camera_entt{};
        in_level->ForEachComponent<wcm::Camera>(
            [&camera_entt](wcm::Camera * _cam){
                if (!camera_entt && _cam->Get_render_id().IsValid()) {
                    camera_entt = _cam->Get_entity_id();
                }
            }
            );

        if(camera_entt.IsValid()) {
            TSparseSet<wcr::wid::WAssetId> cam_render_pipelines;
            cam_render_pipelines.Reserve(WENG_MAX_ASSET_IDS);

            in_level->GetComponent<wcm::Camera>(camera_entt).ForEachPostprocessAssignment(
                [&cam_render_pipelines](
                     const wcm::Camera * _cmp,
                     const wcr::wid::WSubIdxId & _idx,
                     const auto & _assgn) {
                    cam_render_pipelines.Insert(_assgn.pipeline.GetId(), _assgn.pipeline);
                }
                );

            for (const wcr::wid::WAssetId & id : cam_render_pipelines) {
                auto & render_pipeline = in_asset_db.Get<was::RenderPipeline>(id);
                in_render->CreateRenderPipeline(&render_pipeline); // TODO Use the data struct
            }

            wcm::Camera & comp = in_level
                ->GetComponent<wcm::Camera>(camera_entt);
            
            comp.ForEachPostprocessAssignment(
                [&in_level,
                 &in_render,
                 &in_asset_db](
                    const wcm::Camera * _cmp,
                    const wcr::wid::WSubIdxId & _idx,
                    const auto & _assgn
                    ) {

                    wcr::wid::WEntityComponentId ecid = {
                        in_level->Get_asset_id(),
                        _cmp->Get_entity_id(),
                        // TODO MeshComponent? should be camera component?
                        in_level->GetComponentTypeId<wcm::StaticMesh>(),
                        _idx
                    };

                    in_render->CreatePipelineBinding(
                        ecid,
                        wcr::wid::null_id,
                        in_asset_db.Get<was::RenderPipeline>(_assgn.pipeline),
                        in_asset_db.Get<was::RenderPipelineParams>(_assgn.params)
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

        TSparseSet<wcr::wid::WAssetId> static_meshes;
        static_meshes.Reserve(64);
        TSparseSet<wcr::wid::WAssetId> texture_assets;
        texture_assets.Reserve(64);
        TSparseSet<wcr::wid::WEntityComponentId> pipeline_bindings;
        pipeline_bindings.Reserve(64);
        
        in_level->ForEachComponent<wcm::StaticMesh>(
            [&in_render,
             &in_asset_db,
             &static_meshes,
             &texture_assets,
             &pipeline_bindings,
             &in_level
                ](wcm::StaticMesh * _component) {

                static_meshes.Insert(
                    _component->Get_static_mesh_asset().GetId(),
                    _component->Get_static_mesh_asset()
                    );

                was::StaticMesh & sm_asset = in_asset_db.Get<was::StaticMesh>(
                    _component->Get_static_mesh_asset()
                    );

                sm_asset.ForEachMesh(
                    [&in_asset_db,
                     &_component,
                     &texture_assets,
                     &in_level,
                     &pipeline_bindings]
                    (was::StaticMesh * _sm, const wcr::wid::WSubIdxId & _id, wct::geometry::WMesh& _m) {

                        auto & pipeline_parameters =
                            in_asset_db.Get<was::RenderPipelineParams>(
                                _component->GetPipelineAssignment(_id).params
                                );

                        // auto & parameters_struct = pipeline_parameters
                        //     ->RenderPipelineParameters();

                        auto texture_list = pipeline_parameters.Get_texture_list();

                        for(uint8_t i=0; i < texture_list.size(); i++) {
                            wcr::wid::WAssetId t_id = texture_list[i].value;
                    
                            texture_assets.Insert(
                                t_id.GetId(),
                                t_id
                                );
                        }

                        wcr::wid::WEntityComponentId ecid = {
                            in_level->Get_asset_id(),
                            _component->Get_entity_id(),
                            in_level->GetComponentTypeId<wcm::StaticMesh>(),
                            _id
                        };

                        pipeline_bindings.Insert(ecid.GetId(), ecid);
                    }
                    );
            }
            );
        
        for(auto & id : static_meshes) {
            in_asset_db.Get<was::StaticMesh>(id).ForEachMesh(
                [&in_render](was::StaticMesh * _sm, const wcr::wid::WSubIdxId & _id, wct::geometry::WMesh & _m) {
                    in_render->UnloadStaticMesh(
                        {wcr::wid::null_id, _sm->Get_asset_id(), _id}
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
