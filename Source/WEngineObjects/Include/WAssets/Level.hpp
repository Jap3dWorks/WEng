#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WAsset.hpp"
#include "WObjectDb/WEntityComponentDb.hpp"

#include "was_Level.WEngine.hpp"

namespace was {

    class WENGINEOBJECTS_API Level : public WAsset {

        WOBJECT_BODY;

    public:

        WPROPERTY(WEntityComponentDb, entity_component_db,);

    public:

        void Init(WEngine *) ;

        void Update(WEngine * in_engine) ;

        void Close(WEngine * in_engine) ;

        template<std::derived_from<WEntity> T>
        WEntityId CreateEntity() {
            return CreateEntity(T::StaticClass());
        }

        WEntityId CreateEntity(WClass const * in_class);

        WEntity * GetEntity(const WEntityId & in_id) const ;

        /**
         * @brief Run in_predicate for each in_class actor (derived from in_class).
         */
        template<CCallable<void, WEntity*> TFn>
        void ForEachEntity(const WClass * in_class, TFn && in_fn)  const {
            entity_component_db.ForEachEntity(in_class, std::forward<TFn>(in_fn));
        }

        template<std::derived_from<WComponent> T>
        WEntityComponentId CreateComponent(const WEntityId & in_entity_id) {
            return CreateComponent(in_entity_id, T::StaticClass());
        }

        WEntityComponentId CreateComponent(const WEntityId & in_entity_id,
                                           const WClass * in_class) ;

        template<std::derived_from<WComponent> T>
        T & GetComponent(const WEntityId & in_entity_id) const {
            return entity_component_db.GetComponent<T>(in_entity_id);
        }

        WComponent * GetComponent(const WClass * in_class,
                                  const WEntityId & in_component_id) const {
            return entity_component_db.GetComponent(in_class,
                                                     in_component_id);
        }

        WComponent * GetComponent(const WEntityComponentId & in_component_id) const {
            return entity_component_db.GetComponent(in_component_id);
        }

        // IF GetFirstComponent returns a pointer (not a reference)
        // Can be used to know if there is at least one component of a type in the level
        // It will be more flexible and useful.

        template<std::derived_from<WComponent> T>
        T & GetFirstComponent(WEntityId & out_id) const {
            return entity_component_db.GetFirstComponent<T>(out_id);
        }

        WComponent * GetFirstComponent(const WClass * in_class, WEntityId & out_id) const {
            return entity_component_db.GetFirstComponent(in_class, out_id);
        }

        template<CCallable<void, WComponent*> TFn>
        void ForEachComponent(const WClass * in_class, TFn && in_fn) const {
            entity_component_db.ForEachComponent(in_class, std::forward<TFn>(in_fn));        
        }

        // --

        template<std::derived_from<WComponent> T, CCallable<void, T*> TFn>
        void ForEachComponent(TFn && in_fn) const {
            entity_component_db.ForEachComponent<T>(std::forward<TFn>(in_fn));
        }

        template<std::derived_from<WComponent> T>
        WEntityComponentId GetEntityComponentId(const WEntityId & in_entity_id,
                                                const WSubIdxId & in_index_id=0) const noexcept {
            return GetEntityComponentId(T::StaticClass(), in_entity_id, in_index_id);
        }

        WEntityComponentId GetEntityComponentId(const WClass * in_component_class,
                                                const WEntityId & in_entity_id,
                                                const WSubIdxId & in_index_id=0) const noexcept {
            assert(WComponent::StaticClass()->IsBaseOf(in_component_class));

            WComponentTypeId cid = entity_component_db.GetComponentTypeId(in_component_class);
            return {Get_asset_id(), in_entity_id, cid, in_index_id};
        }

    private:

        std::string WEntityPath(const WClass* in_class) const;

        std::string ComponentPath(const WEntityId & in_entity_id,
                                  const WClass * in_class) const;

    private:

    };

}
