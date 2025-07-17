#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"
#include "ILevel.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WObjectManager/WObjectManager.hpp"

#include <format>

class WLEVEL_API WLevel : public ILevel {
public:
    
    WLevel() :
        object_manager_()
    {}

    ~WLevel() override  = default;

    WLevel(const WLevel& other) = delete;

    WLevel(WLevel && other) :
        object_manager_(std::move(other.object_manager_))
    {}

    WLevel & operator=(const WLevel& other) = delete;

    WLevel & operator=(WLevel && other) {
        if (this != &other) {
            object_manager_ = std::move(other.object_manager_);
        }

        return *this;
    }

public:    

    WId CreateActor(const WClass * in_class) override {
        std::string actor_path =
            Name() + ":" + in_class->Name() + "_" +
            std::format("{}", object_manager_.Size(in_class));

        return object_manager_.CreateObject(
            in_class,
            actor_path.c_str()
            )->WID();
    }

    TWRef<WActor> Actor(const WClass * in_class, const WId & in_id) override {
        return static_cast<WActor*>(object_manager_.GetObject(in_class, in_id).Ptr());
    }

    void ForEachActor(const WClass * in_class, TFunction<void(WActor*)> in_predicate) override;

    WId CreateComponent(const WId & in_actor, const WClass & in_class) override {
        return {};
    }

    TWRef<WComponent> Component(const WId & in_component_id) override {
        return nullptr;
    }

    void Update(const WEngineCycleData & in_cycle_data) override {}

    std::string Name() const override {
        return "/path/to/level.level";
    }

private:

    WObjectManager object_manager_;

};
