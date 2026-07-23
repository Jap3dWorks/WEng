#pragma once

#include "WCore/WCore.hpp"
#include "WClass/WClass.hpp"

#include <string>
#include <cstring>
#include <cassert>

#include "WObject.WEng.hpp"

WCLASS()
class WENGINEOBJECTS_API WObject
{
    WOBJECT_BODY

public:

    constexpr void MarkToRemove() {
        remove_=true;
    }

    constexpr bool IsMarkedToRemove() const {
        return remove_;
    }
    
protected:

private:

    bool remove_;                  // TODO Move to WAsset WComponent WEntity

};

