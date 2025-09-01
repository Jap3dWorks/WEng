#pragma once

#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TSparseSet.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WObjectDb/WEntityComponentDb.hpp"
#include "WComponents/WStaticMeshComponent.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WLevel/WLevel.hpp"
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
        TSparseSet<WAssetId> pipeline_parameters;
        pipeline_parameters.Reserve(64);

        in_level->ForEachComponent<WStaticMeshComponent>(
            [&static_meshes,
             &render_pipelines,
             &pipeline_parameters,
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

                for(std::uint8_t i=0; i<smasset->MeshCount(); i++) {

                    WAssetId piid = in_component->GetRenderPipelineAsset(i);
                    if(piid.IsValid()) {
                        render_pipelines.Insert(piid.GetId(), piid);
                    }
                    else {
                        WFLOG_Warning("Invalid WRenderPipelineAsset in WStaticMeshComponent with id {}.",
                                      in_component->EntityId().GetId());
                    }

                    WAssetId paid = in_component->GetRenderPipelineParametersAsset();
                    if(paid.IsValid()) {
                        pipeline_parameters.Insert(paid.GetId(), paid);
                    }
                    else {
                        WFLOG_Warning(
                            "Invalid WRenderPipelineParametersAsset in WStaticMeshComponent with id {}.",
                            in_component->EntityId().GetId());
                    }
                }
            }
            );

        for (const WAssetId & id : pipeline_parameters) {

            auto pparam = in_asset_db.Get<WRenderPipelineParametersAsset>(id);

            for(uint32_t i=0; i<pparam->RenderPipelineParameters().texture_assets_count; i++) {
                WAssetId tid = pparam->RenderPipelineParameters().texture_assets[i].value;
                texture_assets.Insert(tid.GetId(), tid);
            }
        }

        // Load Meshes
        for (const WAssetId & id : static_meshes) {
            auto static_mesh = in_asset_db.Get<WStaticMeshAsset>(id);
            
            static_mesh->ForEachMesh(
                [&in_render](WStaticMeshAsset* _sma, const WAssIdxId & _id, WMeshStruct& _m) {
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
                     &in_component](WStaticMeshAsset* _sma, const WAssIdxId & _id, WMeshStruct& _m) {
                        auto param = in_asset_db.Get<WRenderPipelineParametersAsset>(
                            in_component->GetRenderPipelineParametersAsset(_id)
                            );

                        WEntityComponentId ecid = in_level->GetEntityComponentId<WStaticMeshComponent>(
                            in_component->EntityId(), _id
                            );

                        WAssetIndexId assidx = WIdUtils::ToAssetIndexId(_sma->WID(), _id);
                        in_render->CreatePipelineBinding(
                            ecid,
                            in_component->GetRenderPipelineAsset(_id),
                            assidx,
                            param->RenderPipelineParameters()
                            );

                        WTransformStruct & tstruct = in_level->GetComponent<WTransformComponent>(
                            in_component->EntityId()
                            )->TransformStruct();

                        in_render->UpdateUboModelStatic(
                            ecid,
                            tstruct
                            );
                    }
                    );

            }
            );
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
                    (WStaticMeshAsset * _sm, const WAssIdxId & _id, WMeshStruct& _m) {

                        auto pipeline_parameters =
                            static_cast<WRenderPipelineParametersAsset*>(
                                in_asset_db.Get(
                                    _component->GetRenderPipelineParametersAsset(_id)
                                    ));

                        auto & parameters_struct = pipeline_parameters
                            ->RenderPipelineParameters();

                        for(uint8_t i=0; i < parameters_struct.texture_assets_count; i++) {
                            WAssetId t_id = parameters_struct.texture_assets[i].value;
                    
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
                [&in_render](WStaticMeshAsset * _sm, const WAssIdxId & _id, WMeshStruct & _m) {
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
            
            // TODO if a render pipeline has no bindings can be marked to unload.
        }
    }
}
