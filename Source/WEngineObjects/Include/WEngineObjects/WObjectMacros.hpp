#pragma once

#ifndef WOBJECT_NAME_SIZE
#define WOBJECT_NAME_SIZE 256
#endif

#define _STR(VAL) #VAL

#define WCLASS(...)

#define _WOBJECT_ORIGINAL_BODY_(_WCLASS_)               \
    public :                                            \
    constexpr _WCLASS_() noexcept = default;            \
    virtual ~_WCLASS_() = default;                      \
    static const _WCLASS_& DefaultObject() noexcept {   \
        static _WCLASS_ do_{};                          \
        return do_;                                     \
    }                                                   \
    static WClass * StaticClass() noexcept {            \
        static WClass__ ## _WCLASS_ sc_{};              \
        return &sc_;                                    \
    }                                                   \
    virtual WClass * Class() const {                    \
        return _WCLASS_::StaticClass();                 \
    }


#define _WOBJECT_BODY_(_WCLASS_)                        \
    public:                                             \
    constexpr _WCLASS_() noexcept = default;            \
    ~_WCLASS_() override = default;                     \
    static const _WCLASS_& DefaultObject() noexcept {   \
        static _WCLASS_ default_object{};               \
        return default_object;                          \
    }                                                   \
    static WClass * StaticClass() noexcept {            \
        static WClass__ ## _WCLASS_ static_class;       \
        return &static_class;                           \
    }                                                   \
    WClass * Class() const override {                   \
        return _WCLASS_::StaticClass();                 \
    }


#define WOBJECT_BODY(_WCLASS_)                  \
    _WOBJECT_BODY_(_WCLASS_)


