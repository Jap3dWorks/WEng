#pragma once

#include "WCore/TSingleton.hpp"

#ifndef WOBJECT_NAME_SIZE
#define WOBJECT_NAME_SIZE 256
#endif

// #define _STR(VAL) #VAL

#define WCLASS(...)

class WClass;

#define _WOBJECT_ORIGINAL_BODY_(_WCLASS)                            \
    public :                                                        \
    constexpr _WCLASS() noexcept = default;                         \
    constexpr _WCLASS(const _WCLASS &) = default;                   \
    constexpr _WCLASS(_WCLASS &&) noexcept=default;                 \
    _WCLASS & operator=(const _WCLASS &) = default;                 \
    _WCLASS & operator=(_WCLASS &&) = default;                      \
    virtual ~_WCLASS() = default;                                   \
    static inline constexpr const WClass * StaticClass() noexcept { \
        return &TSingleton<WClass, WClass__ ## _WCLASS>::value;     \
    }                                                               \
    virtual const WClass * Class() const {                          \
        return _WCLASS::StaticClass();                              \
    }


#define _WOBJECT_BODY_(_WCLASS)                                     \
    public:                                                         \
    constexpr _WCLASS() noexcept = default;                         \
    constexpr _WCLASS(const _WCLASS &) = default;                   \
    constexpr _WCLASS(_WCLASS &&) noexcept=default;                 \
    _WCLASS & operator=(const _WCLASS &) = default;                 \
    _WCLASS & operator=(_WCLASS &&) = default;                      \
    ~_WCLASS() override = default;                                  \
    static inline constexpr const WClass * StaticClass() noexcept { \
        return &TSingleton<WClass, WClass__ ## _WCLASS>::value;     \
    }                                                               \
    const WClass * Class() const override {                         \
        return _WCLASS::StaticClass();                              \
    }


#define WOBJECT_BODY(_WCLASS)                   \
    _WOBJECT_BODY_(_WCLASS)


