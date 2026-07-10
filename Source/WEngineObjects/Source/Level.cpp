#include "WAssets/Level.hpp"

#include "WCore/WCore.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WEngineObjects/WComponent.hpp"

#include <format>

// WEntityId was::Level::CreateEntity(WClass const * in_class) {
//     std::string actor_path = WEntityPath(in_class);

//     WEntityId id = entity_component_db
//         .CreateEntity(in_class,
//                       actor_path.c_str());

//     return id;
// }

WEntity * was::Level::GetEntity(const WEntityId & in_id) const {
    return entity_component_db.GetEntity(in_id);
}

// WEntityComponentId was::Level::CreateComponent(const WEntityId & in_entity_id,
//                                            const WClass * in_class) {

//     std::string component_path=ComponentPath(in_entity_id, in_class);

//     entity_component_db.CreateComponent(in_class, in_entity_id);

//     // WEntityComponentId
//     return GetEntityComponentId(in_class, in_entity_id, 0);
// }

std::string was::Level::WEntityPath(const WClass * in_class) const {
    assert(in_class == WEntity::StaticClass() || WEntity::StaticClass()->IsBaseOf(in_class));

    return std::string(Get_name().View()) + ":" + in_class->Name() + "_" +
        std::format("{}", entity_component_db.EntityCount(in_class));
}

std::string was::Level::ComponentPath(const WEntityId & in_entity_id,
                                  const WClass * in_class) const {

    TWRef<WEntity> actor = entity_component_db.GetEntity(in_entity_id);

    return std::string(actor->Get_name().View()) + ":" + in_class->Name();
}

