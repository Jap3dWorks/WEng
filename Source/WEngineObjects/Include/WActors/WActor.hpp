#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WObject.hpp"

#include <string>

/**
 * Base class for all actors in the engine. 
 * Actors are all entities present in a level.
 * Actors has runtime properties.
*/
WCLASS()
class WCORE_API WActor : public WObject
{
    WOBJECT_BODY(WActor)
    
private:
    std::string name_{};
};

