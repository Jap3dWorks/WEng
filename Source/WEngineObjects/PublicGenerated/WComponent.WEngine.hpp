#pragma once

#ifdef _WCLASS_
#undef _WCLASS_
#endif

#ifdef _PWCLASS_
#undef _PWCLASS_
#endif

#define _WCLASS_ WComponent
#define _PWCLASS_ WObject

#include "WReflections/_DECLARE_WCLASS_.hpp"


