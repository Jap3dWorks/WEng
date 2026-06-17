// [[file:../WEngineObjects.org::wpointlightcomponent-wengine-hpp][wpointlightcomponent-wengine-hpp]]
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

#ifdef WCLASS
#undef WCLASS
#endif

#define WCLASS(...)

#define _WCLASS_ WPointLightComponent
#define _PWCLASS_ WComponent
#define _WENG_API_ WENGINEOBJECTS_API
#define _WCLASS_NAMESPACE_ wcm::light

#include "WReflection/_DECLARE_WCLASS_.inc"

#ifdef WOBJECT_BODY
#undef WOBJECT_BODY
#endif

class WClass;

#define WOBJECT_BODY                               \
    public:                                        \
    constexpr WPointLightComponent() noexcept = default;           \
    constexpr WPointLightComponent(const WPointLightComponent &) = default;        \
    constexpr WPointLightComponent(WPointLightComponent &&) noexcept=default;      \
    WPointLightComponent & operator=(const WPointLightComponent &) = default;      \
    WPointLightComponent & operator=(WPointLightComponent &&) = default;           \
    ~WPointLightComponent() override = default;                    \
    static const WClass * StaticClass() noexcept ; \
    const WClass * Class() const override {        \
        return WPointLightComponent::StaticClass();                \
    }
// wpointlightcomponent-wengine-hpp ends here
