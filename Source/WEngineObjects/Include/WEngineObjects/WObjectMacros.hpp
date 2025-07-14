#pragma once

#ifndef WOBJECT_NAME_SIZE
#define WOBJECT_NAME_SIZE 256
#endif

#define WCLASS(...)

#define _WOBJECT_ORIGINAL_BODY_(_WCLASS_)                \
public :                                                 \
    constexpr _WCLASS_() noexcept = default;             \
    virtual ~_WCLASS_() = default;                       \
    static const _WCLASS_& GetDefaultObject() noexcept { \
        static _WCLASS_ do_{};                           \
        return do_;                                      \
    }                                                    \
    static WClass * GetStaticClass() noexcept {          \
        static WClass__ ## _WCLASS_ sc_{};               \
        return &sc_;                                     \
    }                                                    \
    virtual WClass * GetClass() const {                  \
        return _WCLASS_::GetStaticClass();               \
    }


#define _WOBJECT_BODY_(_WCLASS_)                         \
public:                                                  \
    constexpr _WCLASS_() noexcept = default;             \
    ~_WCLASS_() override = default;                      \
    static const _WCLASS_& GetDefaultObject() noexcept { \
        static _WCLASS_ default_object{};                \
        return default_object;                           \
    }                                                    \
    static WClass * GetStaticClass() noexcept {          \
        static WClass__ ## _WCLASS_ static_class;        \
        return &static_class;                            \
    }                                                    \
    WClass * GetClass() const override {                 \
        return _WCLASS_::GetStaticClass();               \
    }


#define WOBJECT_BODY(_WCLASS_)                           \
    _WOBJECT_BODY_(_WCLASS_)

#define _INCLUDES_                              \
    #include "WCore/WCore.hpp"                  \
    #include "WEngineObjects/WClass.hpp"        \
    #include "WCore/TObjectDataBase.hpp"

#define _STR(VAL) #VAL

#define DECLARE_WCLASS(_WCLASS_)                                      \
    _INCLUDES_                                                        \
class _WCLASS_ ;                                                      \
class WENGINEOBJECTS_API WClass__ ## _WCLASS_ : public WClass{        \
public :                                                              \
    constexpr WClass__ ## _WCLASS_ () :                               \
        WClass( _STR(WClass ## _WCLASS_ )) {}                         \
    ~WClass__ ## _WCLASS_ () override = default;                      \
public:                                                               \
    std::unique_ptr<IObjectDataBase> CreateObjectDatabase() override; \
    WObject * DefaultObject() const override;                         \
};

