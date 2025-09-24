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

#define _WCLASS_ WMovementComponent
#define _PWCLASS_ WComponent
#define _WENG_API_ WENGINEOBJECTS_API

#include "WReflections/_DECLARE_WCLASS_.hpp"

