#pragma once
#include "WCore/TFunction.hpp"

class WLevel;
class WEngine;

#ifndef LOCAL_WSYSTEM
#define LOCAL_WSYSTEM
#endif

#ifndef GLOBAL_WSYSTEM
#define GLOBAL_WSYSTEM
#endif

namespace WSystem {

    using WLocalSystemFn = TFnPtr<bool(WLevel*, WEngine*)>;
    using WGlobalSystemFn = TFnPtr<bool(WEngine*)>;

    bool UpdateRenderTransformSystem(WLevel* in_level, WEngine* in_engine);

}

