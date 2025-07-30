#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WLevelObject.hpp"

#include "WEngineObjects/WObjectMacros.hpp"
#include "WEntity.WEngine.hpp"
#include <cstring>

class ILevel;

/**
 * @brief Base class for all actors in the engine. 
 * Entities are composed by components in the level.
*/
WCLASS()
class WCORE_API WEntity : public WLevelObject
{

    WOBJECT_BODY(WEntity)

public:

    constexpr WEntityId WID() const noexcept {
        return wid_;
    }

    constexpr void WID(const WEntityId & in_id) noexcept {
        assert(wid_.GetId() == 0);
        wid_ = in_id;
    }

    const char * Name() const {
        return name_;
    }

    void Name(const char * in_name) {
        std::strcpy(name_, in_name);
    }

private:

    char name_[WOBJECT_NAME_SIZE];

    WEntityId wid_;


};

