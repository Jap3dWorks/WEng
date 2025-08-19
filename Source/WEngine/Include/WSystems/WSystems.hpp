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

    DECLARE_WSYSTEM(WENGINE_API, System_PostUpdateRenderTransform)

    DECLARE_WSYSTEM(WENGINE_API, System_PostUpdateRenderCamera)

    DECLARE_WSYSTEM(WENGINE_API, System_InitRenderLevelResources)

    DECLARE_WSYSTEM(WENGINE_API, System_EndRenderLevelResources)

    DECLARE_WSYSTEM(WENGINE_API, System_InitCameraInput)

END_WSYSTEMS_REG()

