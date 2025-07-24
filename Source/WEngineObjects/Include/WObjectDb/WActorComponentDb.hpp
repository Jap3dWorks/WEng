#pragma once

#include "WCore/WCore.hpp"

#include "WObjectDb/WObjectDb.hpp"
#include "WCore/WIdPool.hpp"

#include <unordered_map>
#include <unordered_set>
#include <concepts>

class WActor;
class WComponent;

class WENGINEOBJECTS_API WActorComponentDb {
public:

    WId CreateActor(const WClass * in_class, const std::string& in_name) ;

    TWRef<WActor> GetActor(const WId & in_id) const;

    /**
     * @brief Run in_predicate for each in_class actor (derived from in_class).
     */
    void ForEachActor(const WClass * in_class,
                      TFunction<void(WActor*)> in_predicate) const ;

    size_t ActorCount(const WClass * in_class) const {
        return actor_manager_.Count(in_class);
    }

    WId CreateComponent(const WId & in_actor_id,
                        const WClass * in_class,
                        const std::string & in_name) ;

    TWRef<WComponent> GetComponent(const WClass * in_class,
                                const WId & in_component_id) const ;

    void ForEachComponent(const WClass * in_class,
                          TFunction<void(WComponent*)> in_predicate) const ;

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
