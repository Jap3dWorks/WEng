#include "WObjectDb/WEntityComponentDb.hpp"
#include "WCore/WCore.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WEngineObjects/WComponent.hpp"

WEntityId WEntityComponentDb::CreateEntity(const WClass * in_class, const char * in_name) {
    assert(
        in_class == WEntity::StaticClass() || WEntity::StaticClass()->IsBaseOf(in_class)
        );

    auto id = CreateEntityId(in_class);

    entity_db_.CreateAt(in_class, id);
    UpdateEntityData(in_class, id, in_name);

    return id;
}

void WEntityComponentDb::InsertEntity(const WClass * in_class,
                                      const WEntityId & in_id,
                                      const char * in_name) {
    entity_db_.CreateAt(in_class, in_id);
    UpdateEntityData(in_class, in_id, in_name);
    entity_id_pool_.Reserve(in_id);
}

void WEntityComponentDb::CreateComponent(const WClass * in_component_class,
                                         const WEntityId & in_entity_id) {
    assert(WComponent::StaticClass()->IsBaseOf(in_component_class));
    assert(entity_db_.Contains(id_entityclass_[in_entity_id], in_entity_id));

    UpdateComponentMetadata(in_component_class, in_entity_id);

    component_db_.CreateAt(in_component_class, in_entity_id);

    WComponent * component = component_db_.Get(in_component_class, in_entity_id);
    component->EntityId(in_entity_id);
}

WEntityId WEntityComponentDb::CreateEntityId(const WClass * in_class) {
    assert(WEntity::StaticClass() == in_class ||
           WEntity::StaticClass()->IsBaseOf(in_class));

    auto id = entity_id_pool_.Generate();
    id_entityclass_[id] = in_class;

    return id;

}

void WEntityComponentDb::UpdateComponentMetadata(const WClass * in_component_class, const WEntityId & in_id) {
    if (!entity_components_.contains(in_id)) {
        entity_components_[in_id] = {};
    }
    
    entity_components_[in_id].insert(in_component_class);

    // Update component class id
    if (!componentclass_id_.contains(in_component_class)) {
        WComponentTypeId id = component_class_id_pool_.Generate();
        componentclass_id_[in_component_class] = id;
        id_componentclass_[id] = in_component_class;
    }
}

void WEntityComponentDb::UpdateEntityData(const WClass * in_entity_class,
                                          const WEntityId & in_id,
                                          const char * in_name) {

    WEntity* entity = entity_db_.Get(in_entity_class, in_id);

    entity->WID(in_id);
    entity->Name(in_name);
    
}
