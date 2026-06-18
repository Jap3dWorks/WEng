// [[file:../WEngineObjects.org::WTextureAsset-wengine-hpp][WTextureAsset-wengine-hpp]]
#pragma once
#include "WClass/WClassFor.hpp"

#ifdef WCLASS
#undef WCLASS
#endif
#define WCLASS(...)

#ifdef WPROPERTY
#undef WPROPERTY
#endif

#define WPROPERTY(_type, _name, _value)                                      \
public:                                                                      \
    inline _type Get_ ## _name () const { return _name ; };                   \
    inline void Set_ ## _name (const _type & in_value) { _name = in_value; } \
private:                                                                     \
    _type _name { _value };

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

#define _WCLASS_ WTextureAsset
#define _PWCLASS_ WObject
#define _WENG_API_ WENGINEOBJECTS_API
#define _WCLASS_NAMESPACE_ 

#ifdef _WCLASS_DECLARATION_
#undef _WCLASS_DECLARATION_
#endif

#ifdef _PWCLASS_DECLARATION_
#undef _PWCLASS_DECLARATION_
#endif

#define _WCLASS_DECLARATION_ class _WCLASS_;
#define _PWCLASS_DECLARATION_ class _PWCLASS_;

#ifndef WENG_DCONCAT
#define WENG_DCONCAT(A,B) A##B
#endif
#ifndef WENG_MP
#define WENG_MP(x) x
#endif
#ifndef WENG_CLASSNAME
#define WENG_CLASSNAME(_CLS) WENG_DCONCAT(WClass__, _CLS)
#endif

#ifndef WENG_CLASSNAME_CLSS_VAR
#define WENG_CLASSNAME_CLSS_VAR(_CLS) WENG_DCONCAT(_VAR_, _CLS)
#endif

#ifndef WENG_CLASSNAME_STR
#define WENG_CLASSNAME_STR(_CLS) _STR(_CLS)
#endif

#ifndef WENG_CLASSNAME_STR_VAR
#define WENG_CLASSNAME_STR_VAR(_CLS) WENG_DCONCAT(_NAME_, _CLS)
#endif

_WCLASS_DECLARATION_
_PWCLASS_DECLARATION_

namespace wrf::wclass {
    inline constexpr std::string_view WENG_CLASSNAME_STR_VAR(WENG_MP( _WCLASS_ )) =
        WENG_CLASSNAME_STR(_WCLASS_);

    using WENG_CLASSNAME(_WCLASS_) = WClassFor<_WCLASS_, _PWCLASS_>;

    inline constexpr WENG_CLASSNAME(_WCLASS_) WENG_CLASSNAME_CLSS_VAR(_WCLASS_) =
        WENG_CLASSNAME_STR_VAR(WENG_MP( _WCLASS_ )) ;
}

#ifdef WOBJECT_BODY
#undef WOBJECT_BODY
#endif

class WClass;

#define WOBJECT_BODY                               \
    public:                                        \
    constexpr WTextureAsset() noexcept = default;           \
    constexpr WTextureAsset(const WTextureAsset &) = default;        \
    constexpr WTextureAsset(WTextureAsset &&) noexcept=default;      \
    WTextureAsset & operator=(const WTextureAsset &) = default;      \
    WTextureAsset & operator=(WTextureAsset &&) = default;           \
    ~WTextureAsset() override = default;                    \
    static const WClass * StaticClass() noexcept ; \
    const WClass * Class() const override {        \
        return WTextureAsset::StaticClass();                \
    }
// WTextureAsset-wengine-hpp ends here
