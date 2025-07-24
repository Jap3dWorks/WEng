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

TWRef<WActor> WActorComponentDb::GetActor(const WId & in_id) const {
    assert(id_actorclass_.contains(in_id));

    return static_cast<WActor *>(actor_manager_.Get(
                                     id_actorclass_.at(in_id),
                                     in_id).Ptr());
}

/**
 * @brief Run in_predicate for each in_class actor (derived from in_class).
 */
void WActorComponentDb::ForEachActor(const WClass * in_class,
                                     TFunction<void(WActor*)> in_predicate) const {
    assert(in_class == WActor::StaticClass() || WActor::StaticClass()->IsBaseOf(in_class));
    
    for(const WClass * c : actor_manager_.Classes()) {
        if(in_class == c || in_class->IsBaseOf(c)) {
            actor_manager_.ForEach(c,
                                   [&in_predicate](WObject* _obj) {
                                       in_predicate(static_cast<WActor*>(_obj));
                                   }
                );
        }
    }
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

TWRef<WComponent> WActorComponentDb::GetComponent(const WClass * in_class,
                                                  const WId & in_component_id) const {
    assert(component_manager_.Contains(in_class, in_component_id));

    return static_cast<WComponent*>(component_manager_.Get(
                                        in_class,
                                        in_component_id
                                        ).Ptr());

}

void WActorComponentDb::ForEachComponent(const WClass * in_class,
                                         TFunction<void(WComponent*)> in_predicate) const {
    for(const WClass * c : component_manager_.Classes()) {
        if (c == in_class || in_class->IsBaseOf(c)) {
            component_manager_.ForEach(c,
                                       [&in_predicate](WObject * _obj) {
                                           in_predicate(static_cast<WComponent*>(_obj));
                                       });
        }
    }
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
