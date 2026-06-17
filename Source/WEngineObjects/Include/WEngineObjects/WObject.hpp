#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"

#include <string>
#include <cstring>
#include <cassert>

#include "WObject.WEngine.hpp"

WCLASS()
class WENGINEOBJECTS_API WObject
{
    WOBJECT_BODY

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

