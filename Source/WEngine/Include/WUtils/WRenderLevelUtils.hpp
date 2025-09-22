#pragma once

#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WObjectDb/WEntityComponentDb.hpp"
#include "WComponents/WStaticMeshComponent.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WLevel/WLevel.hpp"
#include "WRenderUtils.hpp"
#include "WStructs/WComponentStructs.hpp"
#include "WUtils/WMathUtils.hpp"

namespace WRenderLevelUtils {

    inline void InitializeResources(
        IRender * in_render,
        WLevel * in_level,
        const WAssetDb & in_asset_db
        ) {

        TSparseSet<WAssetId> static_meshes;
        static_meshes.Reserve(64);
        TSparseSet<WAssetId> texture_assets;
        texture_assets.Reserve(64);
        TSparseSet<WAssetId> render_pipelines;
        render_pipelines.Reserve(64);

        in_level->ForEachComponent<WStaticMeshComponent>(
            [&static_meshes,
             &render_pipelines,
             &texture_assets,
             &in_asset_db](WStaticMeshComponent * in_component) {
                
                WAssetId smid = in_component->GetStaticMeshAsset();
                
                if(smid.IsValid()) {
                    static_meshes.Insert(smid.GetId(), smid);
                }
                else {
                    WFLOG_Warning(
                        "Invalid WStaticMeshAsset in WStaticMeshComponent with id {}.",
                        in_component->EntityId().GetId()
                        );
                }

                auto smasset = in_asset_db.Get<WStaticMeshAsset>(smid);

                smasset->ForEachMesh(
                    [&in_component,
                     &render_pipelines,
                     &texture_assets,
                     &in_asset_db](WStaticMeshAsset * _sm, const WSubIdxId & _id, WMeshStruct& _m) {
                        
                        WRenderPipelineAssignmentStruct pipassign =
                            in_component->GetRenderPipelineAssignment(0);
                        
                        if(pipassign.pipeline.IsValid() && pipassign.params.IsValid()) {
                            
                            render_pipelines.Insert(
                                pipassign.pipeline.GetId(), pipassign.pipeline
                                );
                            
                            auto param = in_asset_db.Get<WRenderPipelineParametersAsset>(pipassign.params);

                            for(uint32_t i=0; i < param->RenderPipelineParameters().texture_params.size(); i++)
                            {
                                WAssetId tid = param->RenderPipelineParameters().texture_params[i].value;
                                texture_assets.Insert(tid.GetId(), tid);
                            }
                            
                        }
                        else {
                            WFLOG_Warning(
                                "Invalid WRenderPipelineAsset in WStaticMeshComponent with id {}.",
                                in_component->EntityId().GetId()
                                );
                            return;
                        }
                    }
                    );
            }
            );

        // Load Meshes
        for (const WAssetId & id : static_meshes) {
            auto static_mesh = in_asset_db.Get<WStaticMeshAsset>(id);
            
            static_mesh->ForEachMesh(
                [&in_render](WStaticMeshAsset* _sma, const WSubIdxId & _id, WMeshStruct& _m) {
                    WAssetIndexId asset_index = WIdUtils::ToAssetIndexId(
                        _sma->WID(), _id
                        );
                    in_render->LoadStaticMesh(asset_index, _m);
                }
                );
        }

        // Load Textures
        for (const WAssetId & id : texture_assets) {
            auto texture_asset = in_asset_db.Get<WTextureAsset>(id);
            in_render->LoadTexture(id, texture_asset->GetTexture());
        }

        // Initialize Render Pipelines
        for (const WAssetId & id : render_pipelines) {
            auto render_pipeline = in_asset_db.Get<WRenderPipelineAsset>(id);
            in_render->CreateRenderPipeline(render_pipeline); // TODO Use the data struct
        }

        // Pipeline Bindings
        in_level->ForEachComponent<WStaticMeshComponent>(
            [&in_render,
             &in_asset_db,
             &in_level]
            (WStaticMeshComponent* in_component) {
                const WAssetId & sm_id = in_component->GetStaticMeshAsset();
                WStaticMeshAsset * sm_asset = in_asset_db.Get<WStaticMeshAsset>(sm_id);

                sm_asset->ForEachMesh(
                    [&in_asset_db,
                     &in_level,
                     &in_render,
                     &in_component](WStaticMeshAsset* _sma, const WSubIdxId & _id, WMeshStruct& _m) {
                        
                        auto param = in_asset_db.Get<WRenderPipelineParametersAsset>(
                            in_component->GetRenderPipelineAssignment(_id).params
                            );

                        WEntityComponentId ecid = in_level->GetEntityComponentId<WStaticMeshComponent>(
                            in_component->EntityId(), _id
                            );

                        WAssetIndexId assidx = WIdUtils::ToAssetIndexId(_sma->WID(), _id);
                        in_render->CreatePipelineBinding(
                            ecid,
                            in_component->GetRenderPipelineAssignment(_id).pipeline,
                            assidx,
                            param->RenderPipelineParameters()
                            );

                        WTransformStruct & tstruct = in_level->GetComponent<WTransformComponent>(
                            in_component->EntityId()
                            )->TransformStruct();

                        WUBOGraphicsStruct grpubo = WRenderUtils::ToUBOGraphicsStruct(tstruct);
                        WRPParamUboStruct ubodt{.binding=0, .offset=0};
                        ubodt.databuffer.resize(sizeof(decltype(grpubo)));

                        std::memcpy(ubodt.databuffer.data(), &grpubo, sizeof(decltype(grpubo)));

                        in_render->UpdateParameterStatic(ecid, ubodt);
                    }
                    );
            }
            );

        // TODO: Camera postprocess pipelines
        // Temporal solution, only one camera.
        WEntityId camera_entt{};
        in_level->ForEachComponent<WCameraComponent>(
            [&camera_entt](WCameraComponent * _cam){
                if (!camera_entt && _cam->RenderId().IsValid()) {
                    camera_entt = _cam->EntityId();
                }
            }
            );

        if(camera_entt.IsValid()) {
            TSparseSet<WAssetId> cam_render_pipelines;
            cam_render_pipelines.Reserve(WENG_MAX_ASSET_IDS);

            in_level->GetComponent<WCameraComponent>(camera_entt)->ForEachAssignment(
                [&cam_render_pipelines](
                     const WCameraComponent * _cmp,
                     const WSubIdxId & _idx,
                     const WRenderPipelineAssignmentStruct & _assgn) {
                    
                    cam_render_pipelines.Insert(_assgn.pipeline.GetId(), _assgn.pipeline);
                }
                );

            for (const WAssetId & id : cam_render_pipelines) {
                auto render_pipeline = in_asset_db.Get<WRenderPipelineAsset>(id);
                in_render->CreateRenderPipeline(render_pipeline); // TODO Use the data struct
            }
            
            WCameraComponent* comp = in_level->GetComponent<WCameraComponent>(camera_entt);
            comp->ForEachAssignment(
                [&in_level,
                 &in_render,
                 &in_asset_db](
                    const WCameraComponent * _cmp,
                    const WSubIdxId & _idx,
                    const WRenderPipelineAssignmentStruct & _assgn
                    ) {
                    WEntityComponentId ecid = in_level->GetEntityComponentId<WStaticMeshComponent>(
                        _cmp->EntityId(), _idx
                        );

                    in_render->CreatePipelineBinding(
                        ecid,
                        _assgn.pipeline, {},
                        in_asset_db.Get<WRenderPipelineParametersAsset>(_assgn.params)
                            ->RenderPipelineParameters()
                        );
                }
                );
        }

        in_render->RefreshPipelines();
    }

    inline void ReleaseRenderResources(
        IRender * in_render,
        WLevel * in_level,
        const WAssetDb & in_asset_db
        ) {

        TSparseSet<WAssetId> static_meshes;
        static_meshes.Reserve(64);
        TSparseSet<WAssetId> texture_assets;
        texture_assets.Reserve(64);
        TSparseSet<WEntityComponentId> pipeline_bindings;
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
                    _component->GetStaticMeshAsset().GetId(),
                    _component->GetStaticMeshAsset()
                    );

                WStaticMeshAsset * sm_asset = in_asset_db.Get<WStaticMeshAsset>(
                    _component->GetStaticMeshAsset()
                    );

                sm_asset->ForEachMesh(
                    [&in_asset_db,
                     &_component,
                     &texture_assets,
                     &in_level,
                     &pipeline_bindings]
                    (WStaticMeshAsset * _sm, const WSubIdxId & _id, WMeshStruct& _m) {

                        auto pipeline_parameters =
                            static_cast<WRenderPipelineParametersAsset*>(
                                in_asset_db.Get(
                                    _component->GetRenderPipelineAssignment(_id).params
                                    ));

                        auto & parameters_struct = pipeline_parameters
                            ->RenderPipelineParameters();

                        for(uint8_t i=0; i < parameters_struct.texture_params.size(); i++) {
                            WAssetId t_id = parameters_struct.texture_params[i].value;
                    
                            texture_assets.Insert(
                                t_id.GetId(),
                                t_id
                                );
                        }

                        WEntityComponentId ecid = in_level->GetEntityComponentId<WStaticMeshComponent>(
                            _component->EntityId(), _id
                            );

                        pipeline_bindings.Insert(ecid.GetId(), ecid);
                    }
                    );
            }
            );
        
        for(auto & id : static_meshes) {
            in_asset_db.Get<WStaticMeshAsset>(id)->ForEachMesh(
                [&in_render](WStaticMeshAsset * _sm, const WSubIdxId & _id, WMeshStruct & _m) {
                    in_render->UnloadStaticMesh(
                        WIdUtils::ToAssetIndexId(_sm->WID(), _id)
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
