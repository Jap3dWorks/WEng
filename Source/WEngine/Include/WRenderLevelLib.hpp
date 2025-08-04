#pragma once

#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TSparseSet.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WObjectDb/WEntityComponentDb.hpp"
#include "WComponents/WStaticMeshComponent.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"

namespace WRenderLevelLib {

    inline void InitializeResources(
        IRender * in_render,
        const WEntityComponentDb & in_entity_component_db,
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
        
        in_entity_component_db.ForEachComponent<WStaticMeshComponent>(
            [&static_meshes,
             &render_pipelines,
             &pipeline_parameters](WStaticMeshComponent * in_component) {
                WAssetId aux = in_component->StaticMeshAsset();
                if(aux.IsValid()) {
                    static_meshes.Insert(aux.GetId(), aux);
                }
                else {
                    WFLOG_Warning("Invalid WStaticMeshAsset in WStaticMeshComponent with id {}.",
                                  in_component->EntityId().GetId());
                }

                aux = in_component->RenderPipelineAsset();
                if(aux.IsValid()) {
                    render_pipelines.Insert(aux.GetId(), aux);
                }
                else {
                    WFLOG_Warning("Invalid WRenderPipelineAsset in WStaticMeshComponent with id {}.",
                                  in_component->EntityId().GetId());
                }

                aux = in_component->RenderPipelineParametersAsset();
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

        // Initialize Render
        for (const WAssetId & id : render_pipelines) {
            auto render_pipeline = in_asset_db.Get<WRenderPipelineAsset>(id);
            in_render->CreateRenderPipeline(render_pipeline);
        }

        // Pipeline Bindings
        in_entity_component_db.ForEachComponent<WStaticMeshComponent>(
            [&in_render,
             &in_asset_db,
             &in_entity_component_db]
            (WStaticMeshComponent* in_component) {
                auto param =static_cast<WRenderPipelineParametersAsset*> (
                    in_asset_db.Get(in_component->RenderPipelineParametersAsset())
                    );

                in_render->CreatePipelineBinding(
                    in_entity_component_db.EntityComponentId(
                        in_component->EntityId(),
                        in_component->Class()
                        ),
                    in_component->RenderPipelineAsset(),
                    in_component->StaticMeshAsset(),
                    param->RenderPipelineParameters()
                    );
                
            });
    }

    inline void ReleaseRenderResources(
        IRender * in_render,
        const WEntityComponentDb & in_emtity_component_db,
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
        
        in_emtity_component_db.ForEachComponent<WStaticMeshComponent>(
            [&in_render,
             &in_asset_db,
             &static_meshes,
             &texture_assets,
             &render_pipelines /* ,
             &pipeline_bindings
             */ ](WStaticMeshComponent * _component) {
                static_meshes.Insert(
                    _component->StaticMeshAsset().GetId(),
                    _component->StaticMeshAsset()
                    );

                render_pipelines.Insert(
                    _component->RenderPipelineAsset().GetId(),
                    _component->RenderPipelineAsset()
                    );

                auto pipeline_parameters =
                    static_cast<WRenderPipelineParametersAsset*>(
                        in_asset_db.Get(
                            _component->RenderPipelineParametersAsset()
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
