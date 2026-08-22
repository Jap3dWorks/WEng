// [[file:../WObjects.org::wcm::light::ambient-wengine-hpp][wcm::light::ambient-wengine-hpp]]
#pragma once
#include "WClass/WClassFor.hpp"

#ifndef IN_WSTR1
#define IN_WSTR1(A) #A
#endif

#ifndef IN_WSTR2
#define IN_WSTR2(A) IN_WSTR1(A)
#endif

#ifndef WSTR
#define WSTR(A) IN_WSTR2(A)
#endif
#ifndef IN_WJOIN1
#define IN_WJOIN1(A,B) A ## B
#endif

#ifndef IN_WJOIN2
#define IN_WJOIN2(A,B) IN_WJOIN1(A,B)
#endif

#ifndef WJOIN
#define WJOIN(A,B) IN_WJOIN2(A, B)
#endif
#ifndef WEXPND
#define WEXPND(x) x
#endif
#ifndef WCLASSNAME
#define WCLASSNAME(_CLS) WJOIN(WClass__, _CLS)
#endif

#ifndef WCLASSNAME_CLSS_VAR
#define WCLASSNAME_CLSS_VAR(_CLS) WJOIN(_VAR_, _CLS)
#endif

#ifndef WCLASSNAME_STR
#define WCLASSNAME_STR(_CLS) WSTR(_CLS)
#endif

#ifndef WCLASSNAME_STR_VAR
#define WCLASSNAME_STR_VAR(_CLS) WJOIN(_NAME_, _CLS)
#endif


#ifdef WCLASS
#undef WCLASS
#endif
#define WCLASS(...)

#ifdef WPROPERTY
#undef WPROPERTY
#endif

#define WPROPERTY(_type, _name, _value)                                      \
public:                                                                      \
    inline _type const & Get_ ## _name () const { return _name ; };          \
    inline void Set_ ## _name (_type const & in_value) { _name = in_value; } \
    inline void Set_ ## _name (_type && in_value) { _name = std::move(in_value); } \
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

#ifdef _PWCLASS_NAMESPACE_
#undef _PWCLASS_NAMESPACE_
#endif

#define _WCLASS_ Ambient
#define _PWCLASS_ WComponent
#define _WENG_API_ WOBJECTS_API
#define _WCLASS_NAMESPACE_ wcm::light
#define _PWCLASS_NAMESPACE_ 

#ifdef _STR_WCLASS_NAMESPACE_
#undef _STR_WCLASS_NAMESPACE_
#endif

#define _STR_WCLASS_NAMESPACE_ wcm__light

#ifdef _WCLASS_DECLARATION_
#undef _WCLASS_DECLARATION_
#endif

#ifdef _PWCLASS_DECLARATION_
#undef _PWCLASS_DECLARATION_
#endif

#define _WCLASS_DECLARATION_ namespace _WCLASS_NAMESPACE_ { class _WCLASS_; }
#define _PWCLASS_DECLARATION_ class _PWCLASS_;

_WCLASS_DECLARATION_
_PWCLASS_DECLARATION_

#ifdef _WCLASSNAME_V_
#undef _WCLASSNAME_V_
#endif

#define _WCLASSNAME_V_ WCLASSNAME( \
         WJOIN(WJOIN(_STR_WCLASS_NAMESPACE_, __), _WCLASS_))

#ifdef _WCLASSNAME_STR_V_
#undef _WCLASSNAME_STR_V_
#endif

#define _WCLASSNAME_STR_V_ WCLASSNAME_STR( \
        WJOIN( WJOIN(_STR_WCLASS_NAMESPACE_, __), _WCLASS_))

#ifdef _WCLASSNAME_STR_VAR_V_
#undef _WCLASSNAME_STR_VAR_V_
#endif

#define _WCLASSNAME_STR_VAR_V_ WCLASSNAME_STR_VAR(     \
        WJOIN( WJOIN(_STR_WCLASS_NAMESPACE_, __),      \
        _WCLASS_ ))

#ifdef _WCLASSNAME_CLASS_VAR_V_
#undef _WCLASSNAME_CLASS_VAR_V_
#endif

#define _WCLASSNAME_CLASS_VAR_V_ WCLASSNAME_CLSS_VAR( \
         WJOIN(WJOIN(_STR_WCLASS_NAMESPACE_, __), _WCLASS_))

namespace wrf::wclass {

    inline constexpr std::string_view _WCLASSNAME_STR_VAR_V_ = _WCLASSNAME_STR_V_;

    using _WCLASSNAME_V_ = WClassFor<_WCLASS_NAMESPACE_ :: _WCLASS_, _PWCLASS_NAMESPACE_ :: _PWCLASS_>;

    inline constexpr _WCLASSNAME_V_ _WCLASSNAME_CLASS_VAR_V_ = _WCLASSNAME_STR_VAR_V_;
}

#ifdef WOBJECT_BODY
#undef WOBJECT_BODY
#endif

class WClass;

#define WOBJECT_BODY                               \
    public:                                        \
    constexpr Ambient() noexcept = default;           \
    constexpr Ambient(const Ambient &) = default;        \
    constexpr Ambient(Ambient &&) noexcept=default;      \
    Ambient & operator=(const Ambient &) = default;      \
    Ambient & operator=(Ambient &&) = default;           \
    ~Ambient() = default;                             \
    static const WClass * StaticClass() noexcept ; \
    const WClass * Class() const override {        \
        return Ambient::StaticClass();                \
    }
// wcm::light::ambient-wengine-hpp ends here
