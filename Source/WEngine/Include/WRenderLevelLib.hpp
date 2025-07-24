#pragma once

#include "WCore/WCore.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WObjectDb/WActorComponentDb.hpp"
#include "WComponents/WStaticMeshComponent.hpp"

namespace WRenderLevelLib {

    inline void InitilizeResources(
        TRef<IRender> in_renderer,
        WActorComponentDb & in_actor_component_db,
        WAssetDb & in_asset_db
        ) {

        in_actor_component_db.ForEachComponent<WStaticMeshComponent>(
            [&in_asset_db](WStaticMeshComponent * in_component) {
                // TODO submit meshes (using asset wid);
                
            }
            );

        // in_asset_db.ForEach();
        
            
    }

    

}
