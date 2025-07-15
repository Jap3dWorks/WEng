#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"
#include "WEngineObjects/WObjectMacros.hpp"

#include <string>
#include <cstring>
#include <cassert>

#include "WObject.WEngine.hpp"

WCLASS()
class WCORE_API WObject
{
    _WOBJECT_ORIGINAL_BODY_(WObject)

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

