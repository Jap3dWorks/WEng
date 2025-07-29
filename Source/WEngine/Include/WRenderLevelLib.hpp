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
        const WEntityComponentDb & in_actor_component_db,
        const WAssetDb & in_asset_db
        ) {

        TSparseSet<WId> static_meshes;
        static_meshes.Reserve(64);
        TSparseSet<WId> texture_assets;
        texture_assets.Reserve(64);
        TSparseSet<WId> render_pipelines;
        render_pipelines.Reserve(64);
        TSparseSet<WId> pipeline_parameters;
        pipeline_parameters.Reserve(64);
        
        in_actor_component_db.ForEachComponent<WStaticMeshComponent>(
            [&static_meshes,
             &render_pipelines,
             &pipeline_parameters](WStaticMeshComponent * in_component) {

                static_meshes.Insert(in_component->StaticMeshAsset().GetId(),
                                     in_component->StaticMeshAsset());

                render_pipelines.Insert(in_component->RenderPipelineAsset().GetId(),
                                        in_component->RenderPipelineAsset());

                pipeline_parameters.Insert(in_component->RenderPipelineParametersAsset().GetId(),
                                           in_component->RenderPipelineParametersAsset());

                

            }
            );

        for (const WId & id : pipeline_parameters) {

            auto * pparam = static_cast<WRenderPipelineParametersAsset*>(
                in_asset_db.Get(id)
                );
            
            for(auto & tparam : pparam->RenderPipelineParameters().texture_assets) {
                texture_assets.Insert(tparam.value.GetId(),
                                      tparam.value);
            }
        }

        // Register Meshes
        for (const WId & id : static_meshes) {
            auto * static_mesh = static_cast<WStaticMeshAsset*>(in_asset_db.Get(id));
            in_render->RegisterStaticMesh(*static_mesh);
            in_render->LoadStaticMesh(static_mesh->WID());
        }

        // Register Textures
        for (const WId & id : texture_assets) {
            auto * texture_asset = static_cast<WTextureAsset*>(in_asset_db.Get(id));
            in_render->RegisterTexture(*texture_asset);
            in_render->LoadTexture(texture_asset->WID());
        }

        // Initialize Render
        for (const WId & id : render_pipelines) {
            auto * render_pipeline = static_cast<WRenderPipelineAsset*>(in_asset_db.Get(id));
            in_render->CreateRenderPipeline(render_pipeline);
        }

        // Pipeline Bindings
        in_actor_component_db.ForEachComponent<WStaticMeshComponent>(
            [&in_render,
             &in_asset_db]
            (WStaticMeshComponent* in_component) {
                auto param =static_cast<WRenderPipelineParametersAsset*> (
                    in_asset_db.Get(in_component->RenderPipelineParametersAsset())
                    );

                in_render->CreatePipelineBinding(
                    in_component->WID(),
                    in_component->RenderPipelineAsset(),
                    in_component->StaticMeshAsset(),
                    param->RenderPipelineParameters()
                    );
            });
    }

    inline void ReleaseRenderResources(
        IRender * in_render,
        const WEntityComponentDb & in_actor_component_db,
        const WAssetDb & in_asset_db
        ) {

        // TODO Release Render Resources from components.

        TSparseSet<WId> static_meshes;
        static_meshes.Reserve(64);
        TSparseSet<WId> texture_assets;
        texture_assets.Reserve(64);
        TSparseSet<WId> render_pipelines;
        render_pipelines.Reserve(64);
        TSparseSet<WId> pipeline_bindings;
        pipeline_bindings.Reserve(64);
        
        in_actor_component_db.ForEachComponent<WStaticMeshComponent>(
            [&in_render,
             &in_asset_db,
             &static_meshes,
             &texture_assets,
             &render_pipelines](WStaticMeshComponent * _component) {
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

                in_render->DeletePipelineBinding(_component->WID());
            }
            );
        
        for(auto & id : static_meshes) {
            in_render->UnloadStaticMesh(id);
        }

        for(auto & id : texture_assets) {
            in_render->UnloadTexture(id);
        }

        for(auto & id : render_pipelines) {
            in_render->DeletePipeline(id);
        }

        // auto pipeline_parameters = static_cast<WRenderPipelineParametersAsset*>(
        //     in_asset_db.Get(
        //         in_component->RenderPipelineParametersAsset()
        //         ));

        // uint8_t textures_count = pipeline_parameters
        //     ->RenderPipelineParameters()
        //     .texture_assets_count;

        // for(uint8_t i=0; i < textures_count; i++) {
        //     WId t_id = pipeline_parameters
        //         ->RenderPipelineParameters()
        //         .texture_assets[i].value;
                    
        //     texture_assets.Insert(
        //         t_id.GetId(),
        //         t_id
        //         );

        // render_pipelines.Reserve(64);

        // in_level.


    }

}
