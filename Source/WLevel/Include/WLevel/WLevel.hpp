#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WObjectManager/WObjectManager.hpp"

#include <unordered_set>

class WLEVEL_API WLevel {
public:

    using InitFn = TFunction<void(WLevel*, const WEngineCycleData&)>;
    using UpdateFn = TFunction<void(WLevel*, const WEngineCycleData&)>;
    using CloseFn = TFunction<void(WLevel*, const WEngineCycleData&)>;

    WLevel(const char* in_name);

    WLevel(const char* in_name,
           const InitFn & in_init_fn,
           const UpdateFn & in_update_fn,
           const CloseFn & in_close_fn);
    
    virtual ~WLevel() = default;

    WLevel(const WLevel& other);

    WLevel(WLevel && other);

    WLevel & operator=(const WLevel& other);

    WLevel & operator=(WLevel && other);

public:    

    // std::unique_ptr<ILevel> Clone() const ;

    void Init(const WEngineCycleData & in_cycle_data) ;

    void Update(const WEngineCycleData & in_cycle_data) ;

    void Close(const WEngineCycleData & in_cycle_data) ;

    WId CreateActor(const WClass * in_class) ;

    TWRef<WActor> GetActor(const WId & in_id) ;

    /**
     * @brief Run in_predicate for each in_class actor (derived from in_class).
     */
    void ForEachActor(const WClass * in_class,
                      TFunction<void(WActor*)> in_predicate) const ;

    WId CreateComponent(const WId & in_actor_id,
                        const WClass * in_class) ;

    TWRef<WComponent> GetComponent(const WClass * in_class,
                                const WId & in_component_id) ;

    // void ForEachComponent(const WId & in_actor_id,
    //                       TFunction<void(WComponent*)> in_component) override;

    void ForEachComponent(const WClass * in_class,
                          TFunction<void(WComponent*)> in_predicate) ;

    std::string Name() const ;

    WId WID() const ;

    void WID(const WId & in_id) ;

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

    void UpdateComponentMetadata(const WClass * in_component_class, const WId& in_Id);

    std::string ActorPath(const WClass* in_class) const;

    std::string ComponentPath(const WId & in_actor,
                              const WClass * in_class) const;

    const char * name_;

    // TODO: Actor(Component)Manager;
    WObjectManager actor_manager_;

    WObjectManager component_manager_;

    WIdPool actor_id_pool_;

    // Track where the asset is stored
    std::unordered_map<WId, const WClass *> id_actorclass_;

    // Track which components has each actor
    std::unordered_map<WId, std::unordered_set<const WClass *>> actor_components_;

    InitFn init_fn_;

    UpdateFn update_fn_;

    CloseFn close_fn_;

    WId wid_;

};
