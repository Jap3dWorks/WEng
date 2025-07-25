#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TSparseSet.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WObjectDb/WActorComponentDb.hpp"
#include "WComponents/WStaticMeshComponent.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"

#include <unordered_set>

namespace WRenderLevelLib {

    inline void InitilizeResources(
        TRef<IRender> in_render,
        WActorComponentDb & in_actor_component_db,
        WAssetDb & in_asset_db
        ) {

        TSparseSet<WId> static_meshes;
        static_meshes.Reserve(64);
        TSparseSet<WId> render_pipelines;
        render_pipelines.Reserve(64);
        TSparseSet<WId> pipeline_parameters;
        pipeline_parameters.Reserve(64);
        TSparseSet<WId> texture_assets;
        texture_assets.Reserve(64);

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
                in_asset_db.GetRaw(id)
                );
            
            for(auto & tparam : pparam->RenderPipelineParameters().texture_assets) {
                texture_assets.Insert(tparam.value.GetId(),
                                      tparam.value);
            }
        }

        // Register Meshes
        for (const WId & id : static_meshes) {
            auto * static_mesh = static_cast<WStaticMeshAsset*>(in_asset_db.GetRaw(id));
            in_render->RenderResources()->RegisterStaticMesh(static_mesh);
            in_render->RenderResources()->LoadStaticMesh(static_mesh->WID());
        }

        // Register Textures
        for (const WId & id : texture_assets) {
            auto * texture_asset = static_cast<WTextureAsset*>(in_asset_db.GetRaw(id));
            in_render->RenderResources()->RegisterTexture(texture_asset);
            in_render->RenderResources()->LoadTexture(texture_asset->WID());
        }

        // Initialize Render
        for (const WId & id : render_pipelines) {
            auto * render_pipeline = static_cast<WRenderPipelineAsset*>(in_asset_db.GetRaw(id));
            // in_render->CreateRenderPipeline(render_pipeline);  // TODO pipeline from asset
        }

    }

}
