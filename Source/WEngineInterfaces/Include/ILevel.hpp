#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TFunction.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WActors/WActor.hpp"
#include "WComponents/WComponent.hpp"
#include "WEngineObjects/WEngineCycleData.hpp"

// TODO Serializable

class  ILevel {
public:
    
    virtual ~ILevel()=default;

    virtual WId CreateActor(const WClass & in_class)=0;

    virtual TWRef<WActor> Actor(const WId & in_id)=0;

    virtual void ForEachActor(const WClass & in_class, TFunction<void(TWRef<WActor>)> in_predicate)=0;

    virtual WId CreateComponent(const WId & in_actor, const WClass & in_class)=0;

    virtual TWRef<WComponent> Component(const WId & in_component_id)=0;

    /**
     * Call each cycle
     */
    virtual void Update(const WEngineCycleData & in_cycle_data)=0;

};
