#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"
#include "WSystems/WSystemMacros.hpp"

struct WSystemParameters {
    WEngine * engine;
    WLevel * level;
};

using WSystemFn = TFnPtr<bool(const WSystemParameters &)>;

START_WSYSTEMS_REG(WENGINE, WSYSTEMS)

    DECLARE_WSYSTEM(WENGINE_API, SystemInit_InitializeTransformsMatrix)

    DECLARE_WSYSTEM(WENGINE_API, SystemInit_RenderLevelResources)

    DECLARE_WSYSTEM(WENGINE_API, SystemInit_CameraInput)

    DECLARE_WSYSTEM(WENGINE_API, SystemPre_UpdateMovement)

    DECLARE_WSYSTEM(WENGINE_API, SystemPre_CameraInputMovement)

    DECLARE_WSYSTEM(WENGINE_API, SystemPost_UpdateRenderCamera)

    DECLARE_WSYSTEM(WENGINE_API, SystemEnd_RenderLevelResources)

END_WSYSTEMS_REG()

