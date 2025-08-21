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

        // TODO WLevel interface to access entity and components
        in_level->EntityComponentDb().ForEachComponent<WStaticMeshComponent>(
            [&static_meshes,
             &render_pipelines,
             &pipeline_parameters](WStaticMeshComponent * in_component) {
                WAssetId aux = in_component->StaticMeshStruct().static_mesh_asset;
                if(aux.IsValid()) {
                    static_meshes.Insert(aux.GetId(), aux);
                }
                else {
                    WFLOG_Warning("Invalid WStaticMeshAsset in WStaticMeshComponent with id {}.",
                                  in_component->EntityId().GetId());
                }

                aux = in_component->StaticMeshStruct().render_pipeline_asset;
                if(aux.IsValid()) {
                    render_pipelines.Insert(aux.GetId(), aux);
                }
                else {
                    WFLOG_Warning("Invalid WRenderPipelineAsset in WStaticMeshComponent with id {}.",
                                  in_component->EntityId().GetId());
                }

                aux = in_component->StaticMeshStruct().render_pipeline_parameters_asset;
                if(aux.IsValid()) {
                    pipeline_parameters.Insert(aux.GetId(), aux);
                }
                else {
                    WFLOG_Warning("Invalid WRenderPipelineParametersAsset in WStaticMeshComponent with id {}.",
                                  in_component->EntityId().GetId());
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

        // Register Meshes
        for (const WAssetId & id : static_meshes) {
            auto static_mesh = in_asset_db.Get<WStaticMeshAsset>(id);
            in_render->RegisterStaticMesh(*static_mesh);
            in_render->LoadStaticMesh(id);
        }

        // Register Textures
        for (const WAssetId & id : texture_assets) {
            auto texture_asset = in_asset_db.Get<WTextureAsset>(id);
            in_render->RegisterTexture(*texture_asset);
            in_render->LoadTexture(id);
        }

        // Initialize Render Pipelines
        for (const WAssetId & id : render_pipelines) {
            auto render_pipeline = in_asset_db.Get<WRenderPipelineAsset>(id);
            in_render->CreateRenderPipeline(render_pipeline);
        }

        // Pipeline Bindings
        in_level->EntityComponentDb().ForEachComponent<WStaticMeshComponent>(
            [&in_render,
             &in_asset_db,
             &in_level]
            (WStaticMeshComponent* in_component) {
                auto param = static_cast<WRenderPipelineParametersAsset*> (
                    in_asset_db.Get(
                        in_component->StaticMeshStruct().render_pipeline_parameters_asset
                        )
                    );

                WEntityComponentId ecid = in_level->EntityComponentDb().EntityComponentId(
                    in_level->WID(),
                    in_component->EntityId(),
                    in_component->Class()
                    );

                in_render->CreatePipelineBinding(
                    ecid,
                    in_component->StaticMeshStruct().render_pipeline_asset,
                    in_component->StaticMeshStruct().static_mesh_asset,
                    param->RenderPipelineParameters()
                    );

                WTransformStruct & tstruct = in_level->GetComponent<WTransformComponent>(
                    in_component->EntityId()
                    )->TransformStruct();

                in_render->UpdateUboModelStatic(
                    ecid,
                    tstruct
                    );
            });
    }

    inline void ReleaseRenderResources(
        IRender * in_render,
        WLevel * in_level,
        const WAssetDb & in_asset_db
        ) {

        TSparseSet<WId> static_meshes;
        static_meshes.Reserve(64);
        TSparseSet<WId> texture_assets;
        texture_assets.Reserve(64);
        TSparseSet<WId> render_pipelines;
        render_pipelines.Reserve(64);
        // TSparseSet<WId> pipeline_bindings;
        // pipeline_bindings.Reserve(64);
        
        in_level->EntityComponentDb().ForEachComponent<WStaticMeshComponent>(
            [&in_render,
             &in_asset_db,
             &static_meshes,
             &texture_assets,
             &render_pipelines /* ,
             &pipeline_bindings
             */ ](WStaticMeshComponent * _component) {
                static_meshes.Insert(
                    _component->StaticMeshStruct().static_mesh_asset.GetId(),
                    _component->StaticMeshStruct().static_mesh_asset
                    );

                render_pipelines.Insert(
                    _component->StaticMeshStruct().render_pipeline_asset.GetId(),
                    _component->StaticMeshStruct().render_pipeline_asset
                    );

                auto pipeline_parameters =
                    static_cast<WRenderPipelineParametersAsset*>(
                        in_asset_db.Get(
                            _component->StaticMeshStruct().render_pipeline_parameters_asset
                            ));

                auto & parameters_struct = pipeline_parameters
                    ->RenderPipelineParameters();

                for(uint8_t i=0; i < parameters_struct.texture_assets_count; i++) {
                    WId t_id = parameters_struct.texture_assets[i].value;
                    
                    texture_assets.Insert(
                        t_id.GetId(),
                        t_id
                        );
                }
            }
            );
        
        for(auto & id : static_meshes) {
            in_render->UnloadStaticMesh(id);
        }

        for(auto & id : texture_assets) {
            in_render->UnloadTexture(id);
        }

        for(auto & id : render_pipelines) {
            // Removes Render Pipelines and Pipeline Bindings
            in_render->DeleteRenderPipeline(id);
        }
    }
}
