#pragma once

#include "WCore/WCore.hpp"
#include "ILevel.hpp"

class WENGINE_API WLevel : public ILevel {

    WId CreateActor(WClass in_class) override {return {};}
    
    TRef<WActor> Actor(WId in_id) override { return {}; }

    void ForEachActor(WClass in_class, TFunction<void(TRef<WActor>)> in_predicate) override {}

    WId CreateComponent(WId in_actor, WClass in_class) override {return {};}

    TRef<WComponent> Component(WId in_component_id) override {return nullptr; }

};
