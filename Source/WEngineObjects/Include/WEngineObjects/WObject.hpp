#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"

#include <string>
#include <cstring>
#include <cassert>

#ifndef WOBJECT_NAME_SIZE
#define WOBJECT_NAME_SIZE 256
#endif

#define WCLASS(...)

#define _WOBJECT_BODY_(_WCLASS_)                                                  \
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
    virtual const WClass& GetClass() const { return _WCLASS_::GetStaticClass(); }


#define WOBJECT_BODY(_WCLASS_)  \
    _WOBJECT_BODY_(_WCLASS_)


WCLASS()
class WCORE_API WObject
{
    WOBJECT_BODY(WObject)

public:

    WNODISCARD constexpr WId WID() const noexcept
    { return wid_; }

    constexpr void WID(WId in_wid) noexcept {
        assert(wid_.GetId() == 0);
        wid_ = in_wid;
    }

    void Name(const char * in_name) noexcept {
        std::strcpy(name_, in_name);
    }

    constexpr std::string Name() {
        return name_;
    }

    constexpr void MarkToRemove() {
        remove_=true;
    }

    constexpr bool IsMarkedToRemove() {
        return remove_;
    }
    
protected:

private:

    WId wid_;
    char name_[WOBJECT_NAME_SIZE];
    bool remove_;

};
