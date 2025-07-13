#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"

#include <string>
#include <cstring>

#ifndef WOBJECT_NAME_SIZE
#define WOBJECT_NAME_SIZE 256
#endif

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

protected:

    WId wid_;

    char name_[WOBJECT_NAME_SIZE];

private:

};
