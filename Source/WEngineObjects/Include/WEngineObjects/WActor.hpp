#pragma once

#include "WEngineObjects/WObject.hpp"

#include "WActor.WEngine.hpp"

class ILevel;

/**
 * @brief Base class for all actors in the engine. 
 * Actors are all entities present in a level.
 * Actors has runtime properties.
*/
WCLASS()
class WCORE_API WActor : public WObject
{

    WOBJECT_BODY(WActor)

public:

    ILevel * Level() const;

    void Level(ILevel * in_level);

private:

    ILevel * level;

};

