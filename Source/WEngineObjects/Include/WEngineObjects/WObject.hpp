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

    // void Name(const char * in_name) noexcept {
    //     std::strcpy(name_, in_name);
    // }

    // constexpr std::string Name() const {
    //     return name_;
    // }

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

