#pragma once
#include <memory>

#define WCLASS(...)

#define _WOBJECT_BODY_(_WCLASS_)                                                  \
private:                                                                          \
    static inline std::unique_ptr<_WCLASS_> default_object_{};                    \
public:                                                                           \
    _WCLASS_() = default;                                                         \
    ~_WCLASS_() = default;                                                        \
    static const _WCLASS_& GetDefaultObject() {                                   \
        static _WCLASS_ default_object{};                                         \
        return default_object;                                                    \
    }                                                                             \
        static const WClass& GetStaticClass() {                                   \
        static WClass static_class( #_WCLASS_ );                                  \
        return static_class;                                                      \
    }                                                                             \
    virtual const WClass& GetClass() const { return _WCLASS_::GetStaticClass(); }     


#define WOBJECT_BODY(_WCLASS_)  \
_WOBJECT_BODY_(_WCLASS_)

#define WNODISCARD [[nodiscard]]

#define WINLINE inline

#ifndef NDEBUG
#define WDEBUGBUILD 1
#else
#define WDEBUGBUILD 0
#endif
