#pragma once

#ifdef _WCLASS_
#undef _WCLASS_
#endif

#ifdef _PWCLASS_
#undef _PWCLASS_
#endif

#ifdef _WENG_API
#undef _WENG_API
#endif

#ifdef _WCLASS_DECLARATION_
#undef _WCLASS_DECLARATION_
#endif

#ifdef _PWCLASS_DECLARATION_
#undef _PWCLASS_DECLARATION_
#endif

// #define _WCLASS_DECLARATION_ namespace wcm::light {class WAmbientLightComponent;}
// #define _WCLASS_ wcm::light::WAmbientLightComponent
#define _WCLASS_ WAmbientLightComponent
#define _PWCLASS_ WComponent
#define _WENG_API_ WENGINEOBJECTS_API

#include "WReflections/_DECLARE_WCLASS_.inc"

