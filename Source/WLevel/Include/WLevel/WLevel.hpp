#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WEngineObjects/WObjectMacros.hpp"
#include "WObjectDb/WEntityComponentDb.hpp"

class WLEVEL_API WLevel {
public:

    using InitFn = TFunction<void(WLevel*, const WEngineCycleData&)>;
    using UpdateFn = TFunction<void(WLevel*, const WEngineCycleData&)>;
    using CloseFn = TFunction<void(WLevel*, const WEngineCycleData&)>;

    WLevel();

    WLevel(const char* in_name, const WLevelId & in_id);

    WLevel(const char* in_name,
           const WLevelId & in_id,
           const InitFn & in_init_fn,
           const UpdateFn & in_update_fn,
           const CloseFn & in_close_fn);
    
    virtual ~WLevel() = default;

    WLevel(const WLevel& other);

    WLevel(WLevel && other);

    WLevel & operator=(const WLevel& other);

    WLevel & operator=(WLevel && other);

public:    

    void Init(const WEngineCycleData & in_cycle_data) ;

    void Update(const WEngineCycleData & in_cycle_data) ;

    void Close(const WEngineCycleData & in_cycle_data) ;

    WEntityId CreateEntity(const WClass * in_class) ;

    WEntity * GetEntity(const WEntityId & in_id) const ;

    /**
     * @brief Run in_predicate for each in_class actor (derived from in_class).
     */
    void ForEachEntity(const WClass * in_class,
                       TFunction<void(WEntity*)> in_predicate) const ;

    WEntityComponentId CreateComponent(const WEntityId & in_entity_id,
                                       const WClass * in_class) ;

    WComponent * GetComponent(const WClass * in_class,
                              const WEntityId & in_component_id) const ;

    void ForEachComponent(const WClass * in_class,
                          TFunction<void(WComponent*)> in_predicate) const ;

    template<std::derived_from<WComponent> T>
    void ForEachComponent(TFunction<void(T*)> in_predicate) const {
        entity_component_db_.ForEachComponent<T>(in_predicate);
    }

    const char * Name() const ;

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

    const WEntityComponentDb & ActorComponentDb() const {
        return entity_component_db_;
    }

    WLevelId WID() const ;

    void WID(const WLevelId & in_id) ;

private:

    std::string ActorPath(const WClass* in_class) const;

    std::string ComponentPath(const WEntityId & in_entity_id,
                              const WClass * in_class) const;

    char name_[WOBJECT_NAME_SIZE];

    WEntityComponentDb entity_component_db_;

    InitFn init_fn_;

    UpdateFn update_fn_;

    CloseFn close_fn_;

    WLevelId wid_;

};
