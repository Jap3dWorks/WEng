// [[file:../WEngineObjects.org::WAmbienLightComponent-WEngine-hpp][WAmbienLightComponent-WEngine-hpp]]
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

#ifdef _WCLASS_NAMESPACE_
#undef _WCLASS_NAMESPACE_
#endif

#ifdef _WCLASS_DECLARATION_
#undef _WCLASS_DECLARATION_
#endif

#ifdef _PWCLASS_DECLARATION_
#undef _PWCLASS_DECLARATION_
#endif

#define _WCLASS_ WAmbientLightComponent
#define _PWCLASS_ WComponent
#define _WENG_API_ WENGINEOBJECTS_API
#define _WCLASS_NAMESPACE_ wcm::light

#include "WReflection/_DECLARE_WCLASS_.inc"

// #ifdef WOBJECT_BODY
// #undef WOBJECT_BODY
// #endif
// 
// #ifndef WCLASS
// #define WCLASS(...)
// #endif
// 
// class WClass;
// 
// #define WOBJECT_BODY                               \
//     public:                                        \
//     constexpr WAmbientLightComponent() noexcept = default;           \
//     constexpr WAmbientLightComponent(const WAmbientLightComponent &) = default;        \
//     constexpr WAmbientLightComponent(WAmbientLightComponent &&) noexcept=default;      \
//     WAmbientLightComponent & operator=(const WAmbientLightComponent &) = default;      \
//     WAmbientLightComponent & operator=(WAmbientLightComponent &&) = default;           \
//     ~WAmbientLightComponent() override = default;                    \
//     static const WClass * StaticClass() noexcept ; \
//     const WClass * Class() const override {        \
//         return WAmbientLightComponent::StaticClass();                \
//     }
// WAmbienLightComponent-WEngine-hpp ends here
