#pragma once

#include "WCore/WCore.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WObjectManager/WObjectManager.hpp"
#include "WObjectManager/WAssetManager.hpp"

namespace WRenderLevelLib {

    inline void Initilize(
        TRef<IRender> in_renderer,
        WObjectManager & actor_manager,
        WObjectManager & component_manager,
        WAssetManager & asset_manager
        ) {}

    

}
