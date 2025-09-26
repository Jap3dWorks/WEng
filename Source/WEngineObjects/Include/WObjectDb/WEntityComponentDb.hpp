#pragma once

#include "WCore/WCore.hpp"

#include "WObjectDb/WObjectDb.hpp"
#include "WCore/WIdPool.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WEngineObjects/WComponent.hpp"

#include <unordered_map>
#include <unordered_set>
#include <concepts>

class WENGINEOBJECTS_API WEntityComponentDb {
public:

    using WEntityDbType = WObjectDb<WEntity, WEntityId>;

    using WComponentDbType = WObjectDb<WComponent, WEntityId>;

public:

    constexpr WEntityComponentDb() noexcept = default;

    virtual ~WEntityComponentDb() = default;

    WEntityComponentDb(const WEntityComponentDb & other) = default;
    
    WEntityComponentDb(WEntityComponentDb && other) noexcept = default;

    WEntityComponentDb & operator=(const WEntityComponentDb & other) = default;

    WEntityComponentDb & operator=(WEntityComponentDb && other) noexcept = default;

public:

    template<std::derived_from<WEntity> T>
    WEntityId CreateEntity(const char * in_name) {
        return CreateEntity(T::StaticClass(), in_name);
    }

    WEntityId CreateEntity(const WClass * in_class, const char * in_name);

    template<std::derived_from<WEntity> T>
    void InsertEntity(const WEntityId & in_id, const char * in_name) {
        InsertEntity(T::StaticClass(), in_id, in_name);
    }

    void InsertEntity(const WClass * in_class,
                      const WEntityId & in_id,
                      const char * in_name);

    WEntity * GetEntity(const WEntityId & in_id) const {
        assert(id_entityclass_.contains(in_id));

        return entity_db_.Get(id_entityclass_.at(in_id), in_id);
    }

    WEntity * GetFirstEntity(const WClass * in_class, WEntityId & in_id) const {
        return entity_db_.GetFirst(in_class, in_id);
    }

    template<std::derived_from<WEntity> T>
    T * GetFirstEntity(WEntityId & in_id) const {
        return entity_db_.GetFirst<T>(in_id);
    }

    /**
     * @brief Run in_predicate for each in_class entity (derived from in_class).
     */
    template<CCallable<void, WEntity*> TFn>
    void ForEachEntity(const WClass * in_class, TFn && in_fn) const {
        assert(in_class == WEntity::StaticClass() || WEntity::StaticClass()->IsBaseOf(in_class));
    
        for(const WClass * c : entity_db_.IterWClasses()) {
            if(in_class == c || in_class->IsBaseOf(c)) {
                entity_db_.ForEach(c, std::forward<TFn>(in_fn));
            }
        }
    }

    size_t EntityCount(const WClass * in_class) const {
        return entity_db_.Count(in_class);
    }

    template<std::derived_from<WComponent> T>
    void CreateComponent(const WEntityId & in_entity_id) {
        return CreateComponent(T::StaticClass(), in_entity_id);
    }

    /**
     * @brief Create component for entity id.
     */
    void CreateComponent(const WClass * in_class,
                         const WEntityId & in_entity_id);

    TWRef<WComponent> GetComponentRef(const WClass * in_class,
                                      const WEntityId & in_entity_id) const {
        return GetComponent(in_class, in_entity_id);
    }

    template<std::derived_from<WComponent> T>
    T & GetComponent(const WEntityId & in_entity_id) const {
        return component_db_.Get<T>(in_entity_id);
    }

    WComponent * GetComponent(const WClass * in_class,
                              const WEntityId & in_entity_id) const {
        assert(component_db_.Contains(in_class, in_entity_id));
        return component_db_.Get(in_class, in_entity_id);
    }

    WComponent * GetComponent(const WEntityComponentId & in_entity_component_id) const {
        WLevelId lid;
        WEntityId eid;
        WComponentTypeId cid;
        WSubIdxId idxid;
    
        WIdUtils::FromEntityComponentId(in_entity_component_id, lid, eid, cid, idxid);

        return GetComponent(id_componentclass_.at(cid), eid);
    }

    WComponent * GetFirstComponent(const WClass * in_class, WEntityId & out_id) const {
        return component_db_.GetFirst(in_class, out_id);
    }

    template<std::derived_from<WComponent> T>
    T & GetFirstComponent(WEntityId & out_id) const {
        return component_db_.GetFirst<T>(out_id);
    }

    template<CCallable<void, WComponent*> TFn>
    void ForEachComponent(const WClass * in_class, TFn && in_fn) const {
        for(const WClass * c : component_db_.IterWClasses()) {
            if (c == in_class || in_class->IsBaseOf(c)) {
                component_db_.ForEach(c, std::forward<TFn>(in_fn));
            }
        }
    }

    template<std::derived_from<WComponent> T, CCallable<void, T*> TFn>
    void ForEachComponent(TFn && in_fn) const {
        ForEachComponent(T::StaticClass(),
                         [&in_fn](WComponent* in_component) {
                             in_fn(static_cast<T*>(in_component));
                         });
    }

    template<std::derived_from<WComponent> T>
    WComponentTypeId GetComponentTypeId() const {
        return GetComponentTypeId(T::StaticClass());
    }

    WComponentTypeId GetComponentTypeId(const WClass * in_class) const {
        assert(WComponent::StaticClass()->IsBaseOf(in_class));
        return componentclass_id_.at(in_class);
    }

private:

    WEntityId CreateEntityId(const WClass * in_class);

    void UpdateComponentMetadata(const WClass * in_component_class, const WEntityId & in_Id);

    void UpdateEntityData(const WClass * in_entity_class, const WEntityId & in_id, const char * in_name);

    WEntityDbType entity_db_;

    WComponentDbType component_db_;

    WIdPool<WEntityId> entity_id_pool_;

    // Track where the Entity is stored
    std::unordered_map<WEntityId, const WClass *> id_entityclass_;

    // Track which components has each entity
    std::unordered_map<WEntityId, std::unordered_set<const WClass *>> entity_components_;

    // Each component class has a unique 8 bit id
    std::unordered_map<const WClass *, WComponentTypeId> componentclass_id_;
    std::unordered_map<WComponentTypeId, const WClass*> id_componentclass_;
    WIdPool<WComponentTypeId> component_class_id_pool_;

};
