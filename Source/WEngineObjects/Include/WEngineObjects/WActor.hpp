#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WLevelObject.hpp"

#include "WActor.WEngine.hpp"

class ILevel;

/**
 * @brief Base class for all actors in the engine. 
 * Actors are all entities present in a level.
 * Actors has runtime properties.
*/
WCLASS()
class WCORE_API WActor : public WLevelObject
{

    WOBJECT_BODY(WActor)

public:

    constexpr WEntityId WID() const noexcept {
        return wid_;
    }

    constexpr void WID(const WEntityId & in_id) noexcept {
        assert(wid_.GetId() == 0);
        wid_ = in_id;
    }

    WEntityId wid_;

private:

        

};

