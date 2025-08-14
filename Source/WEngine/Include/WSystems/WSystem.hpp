#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"
#include "WSystems/WSystemMacros.hpp"

class WLevel;
class WEngine;
class WSystemRegistry;

WNSPCE()
namespace WSystem {

    using WLevelSystemFn = TFnPtr<bool(WLevel*, WEngine*)>;
    using WSystemFn = TFnPtr<bool(WEngine*)>;

    DECLARE_LEVEL_WSYSTEM(UpdateRenderTransformSystem)

    

    // bool UpdateRenderTransformSystem(WLevel* in_level, WEngine* in_engine);

}

