#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"
#include "WSystems/WSystemMacros.hpp"

struct WSystemParameters {
    WEngine * engine;
    WLevel * level;
};

// using WLevelSystemFn = TFnPtr<bool(WLevel*, WEngine*)>;
using WSystemFn = TFnPtr<bool(const WSystemParameters &)>;

START_WSYSTEMS_REG(WENGINE, WSYSTEMS)

    DECLARE_WSYSTEM(WENGINE_API, System_UpdateRenderTransform)

    DECLARE_WSYSTEM(WENGINE_API, System_UpdateRenderCamera)

    // bool UpdateRenderTransformSystem(WLevel* in_level, WEngine* in_engine);

END_WSYSTEMS_REG()

