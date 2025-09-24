#pragma once

#ifdef _WCLASS_
#undef _WCLASS_
#endif

#ifdef _PWCLASS_
#undef _PWCLASS_
#endif

#ifdef _WENG_API_
#undef _WENG_API_
#endif

#define _WCLASS_ WRenderPipelineAsset
#define _PWCLASS_ WAsset
#define _WENG_API_ WENGINEOBJECTS_API

#include "WReflections/_DECLARE_WCLASS_.hpp"

