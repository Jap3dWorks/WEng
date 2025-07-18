#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"
#include "ILevel.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WObjectManager/WObjectManager.hpp"

class WLEVEL_API WLevel : public ILevel {
public:
    
    WLevel(const char* in_name);
    
    ~WLevel() override  = default;

    WLevel(const WLevel& other) = delete;

    WLevel(WLevel && other);

    WLevel & operator=(const WLevel& other) = delete;

    WLevel & operator=(WLevel && other);

public:    

    void Init() override;

    WId CreateActor(const WClass * in_class) override;

    TWRef<WActor> GetActor(const WId & in_id) const override;

    /**
     * @brief Run in_predicate for each in_class actor (derived from in_class).
     */
    void ForEachActor(const WClass * in_class,
                      TFunction<void(WActor*)> in_predicate) const override;

    WId CreateComponent(const WId & in_actor_id,
                        const WClass * in_class) override;

    TWRef<WComponent> GetComponent(const WClass * in_class,
                                const WId & in_component_id) const override;

    void Update(const WEngineCycleData & in_cycle_data) override;

    void Close() override;

    std::string Name() const override;

private:

    WId CreateActorId(const WClass * in_class);

    std::string ActorPath(const WClass* in_class) const;

    std::string ComponentPath(const WId & in_actor,
                              const WClass * in_class) const;

    WIdPool actor_id_pool_;

    std::unordered_map<WId, const WClass *> id_actorclass_;

    WObjectManager object_manager_;

    const char * name_;

};
