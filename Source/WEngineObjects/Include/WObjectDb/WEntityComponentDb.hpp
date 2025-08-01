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

    template<std::derived_from<WEntity> T>
    WEntityId CreateEntity(const char * in_name) {
        return CreateEntity(T::StaticClass(), in_name);
    }

    WEntityId CreateEntity(const WClass * in_class, const char * in_name);

    TWRef<WEntity> GetEntityRef(const WEntityId & in_id) const {
        return GetEntity(in_id);
    }

    WEntity * GetEntity(const WEntityId & in_id) const {
        assert(id_entityclass_.contains(in_id));

        return entity_db_.Get(id_entityclass_.at(in_id), in_id);
    }

    /**
     * @brief Run in_predicate for each in_class entity (derived from in_class).
     */
    void ForEachEntity(const WClass * in_class,
                       TFunction<void(WEntity*)> in_predicate) const {
        assert(in_class == WEntity::StaticClass() || WEntity::StaticClass()->IsBaseOf(in_class));
    
        for(const WClass * c : entity_db_.Classes()) {
            if(in_class == c || in_class->IsBaseOf(c)) {
                entity_db_.ForEach(c, in_predicate);
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
                                      const WEntityId & in_component_id) const {
        return GetComponent(in_class, in_component_id);
    }

    WComponent * GetComponent(const WClass * in_class,
                              const WEntityId & in_component_id) const {
        assert(component_db_.Contains(in_class, in_component_id));
        return component_db_.Get(in_class, in_component_id);
    }

    WComponent * GetComponent(const WEntityComponentId & in_entity_component_id) const {
        WEntityId eid;
        WComponentTypeId cid;
    
        WIdUtils::FromEntityComponentId(in_entity_component_id, eid, cid);

        return GetComponent(id_componentclass_.at(cid), eid);
    }

    WEntityComponentId EntityComponentId(const WEntityId & in_id, const WClass * in_class) const {
        assert(componentclass_id_.contains(in_class));
        return WIdUtils::ToEntityComponentId(in_id, componentclass_id_.at(in_class));
    }

    void ForEachComponent(const WClass * in_class,
                          TFunction<void(WComponent*)> in_predicate) const {
        for(const WClass * c : component_db_.Classes()) {
            if (c == in_class || in_class->IsBaseOf(c)) {
                component_db_.ForEach(c, in_predicate);
            }
        }
    }

    template<std::derived_from<WComponent> T>
    void ForEachComponent(TFunction<void(T*)> in_predicate) const {
        ForEachComponent(T::StaticClass(),
                         [&in_predicate](WComponent* in_component) {
                             in_predicate(static_cast<T*>(in_component));
                         });
    }

private:

    WEntityId CreateEntityId(const WClass * in_class);

    void UpdateComponentMetadata(const WClass * in_component_class, const WEntityId& in_Id);

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
