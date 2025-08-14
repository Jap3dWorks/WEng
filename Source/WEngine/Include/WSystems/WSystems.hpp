#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"
#include "WSystems/WSystemMacros.hpp"

using WLevelSystemFn = TFnPtr<bool(WLevel*, WEngine*)>;
using WSystemFn = TFnPtr<bool(WEngine*)>;

START_WSYSTEMS_REG(WENGINE, WSYSTEMS)

    WENGINE_API DECLARE_LEVEL_WSYSTEM(UpdateRenderTransformSystem)

    // bool UpdateRenderTransformSystem(WLevel* in_level, WEngine* in_engine);

END_WSYSTEMS_REG()

