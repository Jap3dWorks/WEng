#pragma once

#include "WCore/WCore.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WObjectDb/WActorComponentDb.hpp"
#include "WComponents/WStaticMeshComponent.hpp"
#include "WAssets/WStaticMeshAsset.hpp"

namespace WRenderLevelLib {

    inline void InitilizeResources(
        TRef<IRender> in_render,
        WActorComponentDb & in_actor_component_db,
        WAssetDb & in_asset_db
        ) {

        // Register static meshes
        
        in_actor_component_db.ForEachComponent<WStaticMeshComponent>(
            [&in_render,
             &in_asset_db](WStaticMeshComponent * in_component) {

                TWRef<WAsset> asset = in_asset_db.Get(
                    in_component->StaticMeshAsset()
                    );
                
                in_render->RenderResources()->RegisterStaticMesh(
                    static_cast<WStaticMeshAsset*>(asset.Ptr())
                    );
            }
            );

        // TODO Register Textures

                
            
    }

    

}
