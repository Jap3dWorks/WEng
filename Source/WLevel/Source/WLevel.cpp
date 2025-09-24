#include "WLevel/WLevel.hpp"
#include "WCore/WCore.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WEngineObjects/WComponent.hpp"

#include <format>

WLevel::WLevel() :
    name_("InvalidLevel"),
    wid_(0),
    entity_component_db_()
{
    entity_component_db_.InsertEntity<WEntity>(
        WEntityId{0},
        "LevelEntity"
        );
}

WLevel::WLevel(const char * in_name,
               const WLevelId & in_id) :
    wid_(in_id),
    entity_component_db_()
{
    std::strcpy(name_, in_name);

    entity_component_db_.InsertEntity<WEntity>(
        WEntityId{0},
        "LevelEntity"
        );
}

WLevel::WLevel(const WLevel& other) :
    wid_(other.wid_),
    entity_component_db_(other.entity_component_db_)
{
    std::strcpy(name_, other.name_);
}

WLevel::WLevel(WLevel && other) :
    wid_(std::move(other.wid_)),
    entity_component_db_(std::move(other.entity_component_db_))
{
    std::strcpy(name_, other.name_);
}

WLevel & WLevel::operator=(const WLevel& other) {
    if (this != &other) {
        std::strcpy(name_, other.name_);
        wid_ = other.wid_;
        entity_component_db_ = other.entity_component_db_;
    }
    return *this;
}

WLevel & WLevel::operator=(WLevel && other) {
    if (this != &other) {
        std::strcpy(name_, other.name_);
        wid_ = std::move(other.wid_);
        entity_component_db_ = std::move(other.entity_component_db_);

        // other.name_=nullptr;
    }

    return *this;
}

WEntityId WLevel::CreateEntity(const WClass * in_class) {
    std::string actor_path = WEntityPath(in_class);

    WEntityId id = entity_component_db_
        .CreateEntity(in_class,
                      actor_path.c_str());

    return id;
}

WEntity * WLevel::GetEntity(const WEntityId & in_id) const {
    return entity_component_db_.GetEntity(in_id);
}

WEntityComponentId WLevel::CreateComponent(const WEntityId & in_entity_id,
                                           const WClass * in_class) {

    std::string component_path=ComponentPath(in_entity_id, in_class);

    entity_component_db_.CreateComponent(in_class, in_entity_id);

    // WEntityComponentId
    return GetEntityComponentId(in_class, in_entity_id, 0);
}

const char * WLevel::Name() const {
    return name_;
}

WLevelId WLevel::WID() const {
    return wid_;
}

void WLevel::WID(const WLevelId & in_id) {
    wid_ = in_id;
}

std::string WLevel::WEntityPath(const WClass * in_class) const {
    assert(in_class == WEntity::StaticClass() || WEntity::StaticClass()->IsBaseOf(in_class));

    return std::string(Name()) + ":" + in_class->Name() + "_" +
        std::format("{}", entity_component_db_.EntityCount(in_class));
}

std::string WLevel::ComponentPath(const WEntityId & in_entity_id,
                                  const WClass * in_class) const {

    TWRef<WEntity> actor = entity_component_db_.GetEntity(in_entity_id);

    return std::string(actor->Name()) + ":" + in_class->Name();
}

