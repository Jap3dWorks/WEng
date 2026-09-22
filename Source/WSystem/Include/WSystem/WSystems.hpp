#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"
#include "WSystem/WSystemMacros.hpp"
#include "WAssets/Level.hpp"

struct WSystemParameters {
    WEngine * engine;
    was::Level * level;
};

using WSystemFn = TFnPtr<bool(const WSystemParameters &)>;


START_WSYSTEMS_REG(WSYSTEM, WSYSTEM)

DECLARE_WSYSTEM(WENGINE, SystemInit_InitializeTransformsMatrix)
DECLARE_WSYSTEM(WENGINE, SystemInit_RenderLevelResources)
DECLARE_WSYSTEM(WENGINE, SystemInit_CameraInput)
DECLARE_WSYSTEM(WENGINE, SystemPre_UpdateMovement)
DECLARE_WSYSTEM(WENGINE, SystemPre_CameraInputMovement)
DECLARE_WSYSTEM(WENGINE, SystemPost_UpdateRenderCamera)
DECLARE_WSYSTEM(WENGINE, SystemPost_UpdateShadowMap)
DECLARE_WSYSTEM(WENGINE, SystemEnd_RenderLevelResources)

END_WSYSTEMS_REG()

