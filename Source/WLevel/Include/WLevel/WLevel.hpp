#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WEngineObjects/WObjectMacros.hpp"
#include "WObjectDb/WEntityComponentDb.hpp"

class WEngine;

/**
 * @brief WLevel stores all current entities and components.
 * WEntityId 1 is reserved to store Level components.
 */
class WLEVEL_API WLevel {
public:

    WLevel();

    WLevel(const char* in_name, const WLevelId & in_id);
    
    virtual ~WLevel() = default;

    WLevel(const WLevel& other);

    WLevel(WLevel && other);

    WLevel & operator=(const WLevel& other);

    WLevel & operator=(WLevel && other);

public:    

    void Init(WEngine *) ;

    void Update(WEngine * in_engine) ;

    void Close(WEngine * in_engine) ;

    template<std::derived_from<WEntity> T>
    WEntityId CreateEntity() {
        return CreateEntity(T::StaticClass());
    }

    WEntityId CreateEntity(const WClass * in_class) ;

    WEntity * GetEntity(const WEntityId & in_id) const ;

    /**
     * @brief Run in_predicate for each in_class actor (derived from in_class).
     */
    template<CCallable<void, WEntity*> TFn>
    void ForEachEntity(const WClass * in_class, TFn && in_fn)  const {
        entity_component_db_.ForEachEntity(in_class, std::forward<TFn>(in_fn));
    }

    template<std::derived_from<WComponent> T>
    WEntityComponentId CreateComponent(const WEntityId & in_entity_id) {
        return CreateComponent(in_entity_id, T::StaticClass());
    }

    WEntityComponentId CreateComponent(const WEntityId & in_entity_id,
                                       const WClass * in_class) ;

    template<std::derived_from<WComponent> T>
    T & GetComponent(const WEntityId & in_entity_id) const {
        return entity_component_db_.GetComponent<T>(in_entity_id);
    }

    WComponent * GetComponent(const WClass * in_class,
                              const WEntityId & in_component_id) const {
        return entity_component_db_.GetComponent(in_class,
                                                 in_component_id);
    }

    WComponent * GetComponent(const WEntityComponentId & in_component_id) const {
        return entity_component_db_.GetComponent(in_component_id);
    }

    template<std::derived_from<WComponent> T>
    T & GetFirstComponent(WEntityId & out_id) const {
        return entity_component_db_.GetFirstComponent<T>(out_id);
    }

    WComponent * GetFirstComponent(const WClass * in_class, WEntityId & out_id) const {
        return entity_component_db_.GetFirstComponent(in_class, out_id);
    }

    template<CCallable<void, WComponent*> TFn>
    void ForEachComponent(const WClass * in_class, TFn && in_fn) const {
        entity_component_db_.ForEachComponent(in_class, std::forward<TFn>(in_fn));        
    }

    // --

    template<std::derived_from<WComponent> T, CCallable<void, T*> TFn>
    void ForEachComponent(TFn && in_fn) const {
        entity_component_db_.ForEachComponent<T>(std::forward<TFn>(in_fn));
    }

    const char * Name() const;

    template<std::derived_from<WComponent> T>
    WEntityComponentId GetEntityComponentId(const WEntityId & in_entity_id,
                                            const WSubIdxId & in_index_id=0) const noexcept {
        
        return GetEntityComponentId(T::StaticClass(), in_entity_id, in_index_id);
    }

    WEntityComponentId GetEntityComponentId(const WClass * in_component_class,
                                            const WEntityId & in_entity_id,
                                            const WSubIdxId & in_index_id=0) const noexcept {
        assert(WComponent::StaticClass()->IsBaseOf(in_component_class));
        
        WComponentTypeId cid = entity_component_db_.GetComponentTypeId(in_component_class);
        return WIdUtils::ToEntityComponentId(WID(), in_entity_id, cid, in_index_id);
    }

    WLevelId WID() const;

    void WID(const WLevelId & in_id);

private:

    std::string WEntityPath(const WClass* in_class) const;

    std::string ComponentPath(const WEntityId & in_entity_id,
                              const WClass * in_class) const;

    char name_[WOBJECT_NAME_SIZE];

    // WEntityId 0 is reserved for Level Components.
    WEntityComponentDb entity_component_db_;

    WLevelId wid_;

};
