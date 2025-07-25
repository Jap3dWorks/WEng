#pragma once

#include "WCore/WCore.hpp"

#include "WObjectDb/WObjectDb.hpp"
#include "WCore/WIdPool.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WEngineObjects/WComponent.hpp"

#include <unordered_map>
#include <unordered_set>
#include <concepts>

class WENGINEOBJECTS_API WActorComponentDb {
public:

    WId CreateActor(const WClass * in_class, const std::string& in_name) ;

    TWRef<WActor> GetActorRef(const WId & in_id) const {
        return GetActor(in_id);
    }

    WActor * GetActor(const WId & in_id) const {
        assert(id_actorclass_.contains(in_id));

        return static_cast<WActor *>(actor_manager_.Get(
                                         id_actorclass_.at(in_id),
                                         in_id));
    }

    /**
     * @brief Run in_predicate for each in_class actor (derived from in_class).
     */
    void ForEachActor(const WClass * in_class,
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

    size_t ActorCount(const WClass * in_class) const {
        return actor_manager_.Count(in_class);
    }

    WId CreateComponent(const WId & in_actor_id,
                        const WClass * in_class,
                        const std::string & in_name) ;

    TWRef<WComponent> GetComponentRef(const WClass * in_class,
                                   const WId & in_component_id) const {
        return GetComponent(in_class, in_component_id);
    }

    WComponent * GetComponent(const WClass * in_class, const WId & in_component_id) const {
        assert(component_manager_.Contains(in_class, in_component_id));

        return static_cast<WComponent*>(component_manager_.Get(
                                            in_class,
                                            in_component_id
                                            ));
    }

    void ForEachComponent(const WClass * in_class,
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

    template<std::derived_from<WComponent> T>
    void ForEachComponent(TFunction<void(T*)> in_predicate) const {
        ForEachComponent(T::StaticClass(),
                         [&in_predicate](WComponent* in_component) {
                             in_predicate(static_cast<T*>(in_component));
                         });
    }

private:

    WId CreateActorId(const WClass * in_class);

    void UpdateComponentMetadata(const WClass * in_component_class, const WId& in_Id);

    WObjectDb actor_manager_;

    WObjectDb component_manager_;

    WIdPool actor_id_pool_;

    // Track where the asset is stored
    std::unordered_map<WId, const WClass *> id_actorclass_;

    // Track which components has each actor
    std::unordered_map<WId, std::unordered_set<const WClass *>> actor_components_;
  
};
