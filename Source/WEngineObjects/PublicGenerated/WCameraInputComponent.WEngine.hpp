// [[file:../WEngineObjects.org::WCameraInputComponent-wengine-hpp][WCameraInputComponent-wengine-hpp]]
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

#define _WCLASS_ WCameraInputComponent
#define _PWCLASS_ WObject
#define _WENG_API_ WENGINEOBJECTS_API
#define _WCLASS_NAMESPACE_ 

#include "WReflection/_DECLARE_WCLASS_.inc"

#ifdef WOBJECT_BODY
#undef WOBJECT_BODY
#endif

class WClass;

#define WOBJECT_BODY                               \
    public:                                        \
    constexpr WCameraInputComponent() noexcept = default;           \
    constexpr WCameraInputComponent(const WCameraInputComponent &) = default;        \
    constexpr WCameraInputComponent(WCameraInputComponent &&) noexcept=default;      \
    WCameraInputComponent & operator=(const WCameraInputComponent &) = default;      \
    WCameraInputComponent & operator=(WCameraInputComponent &&) = default;           \
    ~WCameraInputComponent() override = default;                    \
    static const WClass * StaticClass() noexcept ; \
    const WClass * Class() const override {        \
        return WCameraInputComponent::StaticClass();                \
    }
// WCameraInputComponent-wengine-hpp ends here
