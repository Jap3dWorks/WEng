#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include "WEngineObjects/WObject.hpp"

#include "WEntity.WEngine.hpp"

/**
 * @brief Base class for all actors in the engine. 
 * Entities are composed by components in the level.
*/
class WCORE_API WEntity : public WObject
{

    WOBJECT_BODY

public:

    WPROPERTY(WEntityId, entity_id,);
    WPROPERTY(WObjectName, name,);

public:

    virtual void OnUpdate(const WEngineCycleStruct & in_cycle_data) {}

    virtual void OnInit(const WEngineCycleStruct & in_cycle_data) {}

    virtual void OnClose(const WEngineCycleStruct & in_cycle_data) {}

};

