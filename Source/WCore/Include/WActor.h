#pragma once
#include <string>
#include "WCore.h"

/*
 * Base class for all actors in the engine. 
 * Actors are all entities present in a level.
*/
WCLASS()
class WCORE_API WActor : public WObject
{
    WOBJECT_BODY(WActor)
    
private:
    std::string name_{};
};
