#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"
#include "ILevel.hpp"
#include "WActors/WActor.hpp"
#include "WEngineObjects/TWRef.hpp"


class WLEVEL_API WLevel : public ILevel {
public:
    
    WLevel() {}

    ~WLevel() override  = default;

    WLevel(const WLevel& other) = delete;

    WLevel(WLevel && other) {}

    WLevel & operator=(const WLevel& other) = delete;

    WLevel & operator=(WLevel && other) {
        return *this;
    }

    WId CreateActor(const WClass & in_class) override {
        return {};
    }

    TWRef<WActor> Actor(const WId & in_id) override { return {}; }

    void ForEachActor(const WClass & in_class, TFunction<void(TWRef<WActor>)> in_predicate) override {}

    WId CreateComponent(const WId & in_actor, const WClass & in_class) override {return {};}

    TWRef<WComponent> Component(const WId & in_component_id) override {return nullptr; }

private:

    

};
