#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WCore/TFunction.hpp"

class WActor;
class WComponent;

// TODO Serializable

class  ILevel {

    virtual WId CreateActor(WClass in_class)=0;

    virtual TRef<WActor> Actor(WId in_id)=0;

    virtual void ForEachActor(WClass in_class, TFunction<void(TRef<WActor>)> in_predicate)=0;

    virtual WId CreateComponent(WId in_actor, WClass in_class)=0;

    virtual TRef<WComponent> Component(WId in_component_id)=0;

};
