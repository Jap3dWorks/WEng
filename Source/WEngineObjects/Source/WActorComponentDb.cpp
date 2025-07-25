#include "WObjectDb/WActorComponentDb.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WEngineObjects/WComponent.hpp"

WId WActorComponentDb::CreateActor(const WClass * in_class, const std::string & in_name) {
    assert(in_class == WActor::StaticClass() ||
           WActor::StaticClass()->IsBaseOf(in_class));

    WId id = CreateActorId(in_class);

    actor_manager_.Create(
        in_class,
        id,
        in_name.c_str()
        );

    return id;

}

WId WActorComponentDb::CreateComponent(const WId & in_actor_id,
                                       const WClass * in_class,
                                       const std::string & in_name) {
    assert(WComponent::StaticClass()->IsBaseOf(in_class));
    assert(actor_manager_.Contains(id_actorclass_[in_actor_id], in_actor_id));

    UpdateComponentMetadata(in_class, in_actor_id);
    
    component_manager_.Create(in_class,
                           in_actor_id,
                           in_name.c_str());

    return in_actor_id;
}

WId WActorComponentDb::CreateActorId(const WClass * in_class) {
    assert(WActor::StaticClass() == in_class ||
           WActor::StaticClass()->IsBaseOf(in_class));

    WId id = actor_id_pool_.Generate();
    id_actorclass_[id] = in_class;

    return id;

}

void WActorComponentDb::UpdateComponentMetadata(const WClass * in_component_class, const WId& in_id) {
    if (!actor_components_.contains(in_id)) {
        actor_components_[in_id] = {};
    }
    
    actor_components_[in_id].insert(in_component_class);
    
}
