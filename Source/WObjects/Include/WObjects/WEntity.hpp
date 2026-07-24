#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include "WObjects/WObject.hpp"

#include "WEntity.WEng.hpp"

/**
 * @brief Base class for all actors in the engine. 
 * Entities are composed by components in the level.
*/
class WCORE_API WEntity : public WObject
{

    WOBJECT_BODY

public:

    WPROPERTY(wcr::wid::WEntityId, entity_id,);
    WPROPERTY(WObjectName, name,);

public:

    virtual void OnUpdate(const WEngineCycleStruct & in_cycle_data) {}

    virtual void OnInit(const WEngineCycleStruct & in_cycle_data) {}

    virtual void OnClose(const WEngineCycleStruct & in_cycle_data) {}

};

