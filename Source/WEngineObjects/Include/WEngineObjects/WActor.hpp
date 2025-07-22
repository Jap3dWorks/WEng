#pragma once

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

private:

};

