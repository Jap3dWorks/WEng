#include "WObjectDb/WEntityComponentDb.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WEngineObjects/WComponent.hpp"

WEntityId WEntityComponentDb::CreateEntity(const WClass * in_class, const std::string & in_name) {
    assert(in_class == WEntity::StaticClass() ||
           WEntity::StaticClass()->IsBaseOf(in_class));

    auto id = CreateEntityId(in_class);

    entity_db_.Create(
        in_class,
        id,
        in_name.c_str()
        );

    return id;

}

WComponentTypeId WEntityComponentDb::CreateComponent(const WEntityId & in_entity_id,
                                                 const WClass * in_class,
                                                 const std::string & in_name) {
    assert(WComponent::StaticClass()->IsBaseOf(in_class));
    assert(actor_manager_.Contains(id_actorclass_[in_actor_id], in_actor_id));

    UpdateComponentMetadata(in_class, in_entity_id);

    // TODO resolve component id

    

    WComponentTypeId r;
    
    component_db_.Create(in_class,
                         r,
                         in_name.c_str());

    return r;
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
}
