#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"

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

protected:

    WId wid_;

private:

};
