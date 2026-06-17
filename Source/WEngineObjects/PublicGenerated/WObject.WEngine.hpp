// [[file:../WEngineObjects.org::WObject-wengine-hpp][WObject-wengine-hpp]]
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

#define _WCLASS_ WObject
#define _PWCLASS_ WObject
#define _WENG_API_ WENGINEOBJECTS_API
#define _WCLASS_NAMESPACE_ 

#include "WReflection/_DECLARE_WCLASS_.inc"

#ifdef WOBJECT_BODY
#undef WOBJECT_BODY
#endif

class WClass;

#define WOBJECT_BODY                              \
    public :                                      \
    constexpr WObject() noexcept = default;          \
    constexpr WObject(const WObject &) = default;       \
    constexpr WObject(WObject &&) noexcept=default;     \
    WObject & operator=(const WObject &) = default;     \
    WObject & operator=(WObject &&) = default;          \
    virtual ~WObject() = default;                    \
    static const WClass * StaticClass() noexcept; \
    virtual const WClass * Class() const {        \
        return WObject::StaticClass();               \
    }
// WObject-wengine-hpp ends here
