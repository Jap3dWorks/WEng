#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WLevelObject.hpp"

#include "WComponent.WEngine.hpp"

WCLASS()
class WCORE_API WComponent : public WLevelObject
{

    WOBJECT_BODY(WComponent)

public:

    constexpr WComponentTypeId WID() const noexcept {
        return wid_;
    }

    constexpr void WID(const WComponentTypeId & in_id) noexcept {
        assert(wid_.GetId() == 0);
        wid_ = in_id;
    }

    WComponentTypeId wid_;        
        

};

