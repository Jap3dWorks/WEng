#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"
#include "ILevel.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WObjectManager/WObjectManager.hpp"

class WLEVEL_API WLevel : public ILevel {
public:

    using InitFn = TFunction<void(ILevel*)>;
    using UpdateFn = TFunction<void(ILevel*, const WEngineCycleData&)>;
    using CloseFn = TFunction<void(ILevel*)>;

    WLevel(const char* in_name);

    WLevel(const char* in_name,
           const InitFn & in_init_fn,
           const UpdateFn & in_update_fn,
           const CloseFn & in_close_fn);
    
    virtual ~WLevel() = default;

    WLevel(const WLevel& other) = delete;

    WLevel(WLevel && other);

    WLevel & operator=(const WLevel& other) = delete;

    WLevel & operator=(WLevel && other);

public:    

    void Init() override;

    WId CreateActor(const WClass * in_class) override;

    TWRef<WActor> GetActor(const WId & in_id) override;

    /**
     * @brief Run in_predicate for each in_class actor (derived from in_class).
     */
    void ForEachActor(const WClass * in_class,
                      TFunction<void(WActor*)> in_predicate) const override;

    WId CreateComponent(const WId & in_actor_id,
                        const WClass * in_class) override;

    TWRef<WComponent> GetComponent(const WClass * in_class,
                                const WId & in_component_id) override;

    void Update(const WEngineCycleData & in_cycle_data) override;

    void Close() override;

    std::string Name() const override;

    void SetInitFn(const InitFn & in_fn) {
        init_fn_ = in_fn;
    }
    
    void SetInitFn(InitFn && in_fn) {
        init_fn_ = std::move(in_fn);
    }

    void SetUpdateFn(const UpdateFn & in_fn) {
        update_fn_ = in_fn;
    }

    void SetUpdateFn(UpdateFn && in_fn) {
        update_fn_ = std::move(in_fn);
    }

    void SetCloseFn(const CloseFn & in_fn) {
        close_fn_ = in_fn;
    }

    void SetCloseFn(CloseFn && in_fn) {
        close_fn_ = std::move(in_fn);
    }

private:

    WId CreateActorId(const WClass * in_class);

    std::string ActorPath(const WClass* in_class) const;

    std::string ComponentPath(const WId & in_actor,
                              const WClass * in_class) const;

    const char * name_;

    WObjectManager object_manager_;

    WIdPool actor_id_pool_;

    std::unordered_map<WId, const WClass *> id_actorclass_;

    InitFn init_fn_;

    UpdateFn update_fn_;

    CloseFn close_fn_;

};
