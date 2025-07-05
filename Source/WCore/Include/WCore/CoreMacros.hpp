#pragma once
#include <memory>

#define WCLASS(...)

#define _WOBJECT_BODY_(_WCLASS_)                                                  \
private:                                                                          \
    static inline std::unique_ptr<_WCLASS_> default_object_{};                    \
public:                                                                           \
    constexpr _WCLASS_() noexcept = default;                                      \
    ~_WCLASS_() = default;                                                        \
    static const _WCLASS_& GetDefaultObject() noexcept {                          \
        static _WCLASS_ default_object{};                                         \
        return default_object;                                                    \
    }                                                                             \
    static const WClass& GetStaticClass() noexcept {                              \
        static WClass static_class( #_WCLASS_ );                                  \
        return static_class;                                                      \
    }                                                                             \
    virtual const WClass& GetClass() const { return _WCLASS_::GetStaticClass(); } \



#define WOBJECT_BODY(_WCLASS_)  \
_WOBJECT_BODY_(_WCLASS_)

#define WNODISCARD [[nodiscard]]

#define WINLINE inline

#define WCNOEXCEPT const noexcept

#define WNOEXCEPT noexcept

#ifndef NDEBUG
#define WDEBUGBUILD 1
#else
#define WDEBUGBUILD 0
#endif

