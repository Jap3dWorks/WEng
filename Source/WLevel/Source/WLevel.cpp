#include "WLevel/WLevel.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WEngineObjects/WComponent.hpp"

#include <format>

WLevel::WLevel() :
    name_("InvalidLevel"),
    wid_(0),
    entity_component_db_(),
    init_fn_([](WLevel*, const WEngineCycleData&){}),
    update_fn_([](WLevel*, const WEngineCycleData&){}),
    close_fn_([](WLevel*, const WEngineCycleData&){})
{}

WLevel::WLevel(const char * in_name,
               const WLevelId & in_id) :
    wid_(in_id),
    entity_component_db_(),
    init_fn_([](WLevel*, const WEngineCycleData &){}),
    update_fn_([](WLevel*, const WEngineCycleData &){}),
    close_fn_([](WLevel*, const WEngineCycleData &){})
{
    std::strcpy(name_, in_name);
}

WLevel::WLevel(const char * in_name,
               const WLevelId & in_id,
               const InitFn & in_init_fn,
               const UpdateFn & in_update_fn,
               const CloseFn & in_close_fn) :
    wid_(in_id),
    entity_component_db_(),
    init_fn_(in_init_fn),
    update_fn_(in_update_fn),
    close_fn_(in_close_fn)
{
    std::strcpy(name_, in_name);
}

WLevel::WLevel(const WLevel& other) :
    wid_(other.wid_),
    entity_component_db_(other.entity_component_db_),
    init_fn_(other.init_fn_),
    update_fn_(other.update_fn_),
    close_fn_(other.close_fn_)
{
    std::strcpy(name_, other.name_);
}

WLevel::WLevel(WLevel && other) :
    wid_(std::move(other.wid_)),
    entity_component_db_(std::move(other.entity_component_db_)),
    init_fn_(std::move(other.init_fn_)),
    update_fn_(std::move(other.update_fn_)),
    close_fn_(std::move(other.close_fn_))
{
    std::strcpy(name_, other.name_);
}

WLevel & WLevel::operator=(const WLevel& other) {
    if (this != &other) {
        std::strcpy(name_, other.name_);
        wid_ = other.wid_;
        entity_component_db_ = other.entity_component_db_;
        init_fn_ = other.init_fn_;
        update_fn_ = other.update_fn_;
        close_fn_ = other.close_fn_;
    }
    return *this;
}

WLevel & WLevel::operator=(WLevel && other) {
    if (this != &other) {
        std::strcpy(name_, other.name_);
        wid_ = std::move(other.wid_);
        entity_component_db_ = std::move(other.entity_component_db_);
        init_fn_ = std::move(other.init_fn_);
        update_fn_ = std::move(other.update_fn_);
        close_fn_ = std::move(other.close_fn_);

        // other.name_=nullptr;
    }

    return *this;
}

void WLevel::Init(const WEngineCycleData & in_cycle_data) {
    init_fn_(this, in_cycle_data);
}

void WLevel::Update(const WEngineCycleData & in_cycle_data) {
    update_fn_(this, in_cycle_data);
}

void WLevel::Close(const WEngineCycleData & in_cycle_data) {
    close_fn_(this, in_cycle_data);
}

WEntityId WLevel::CreateEntity(const WClass * in_class) {
    std::string actor_path = ActorPath(in_class);

    WEntityId id = entity_component_db_.CreateEntity(in_class, actor_path.c_str());

    return id;
}

WEntity * WLevel::GetEntity(const WEntityId & in_id) const {
    return entity_component_db_.GetEntity(in_id);
}

void WLevel::ForEachEntity(const WClass * in_class,
                          TFunction<void(WEntity*)> in_predicate) const {
    entity_component_db_.ForEachEntity(in_class, in_predicate);
}

WEntityComponentId WLevel::CreateComponent(const WEntityId & in_actor_id,
                                           const WClass * in_class) {

    std::string component_path=ComponentPath(in_actor_id, in_class);

    entity_component_db_.CreateComponent(in_class, in_actor_id);
    
    return entity_component_db_.EntityComponentId(in_actor_id, in_class);
}

WComponent * WLevel::GetComponent(const WClass * in_class,
                                  const WEntityId & in_component_id) const {
    return entity_component_db_.GetComponent(in_class,
                                             in_component_id);
}

void WLevel::ForEachComponent(const WClass * in_class,
                              TFunction<void(WComponent*)> in_predicate) const {
    entity_component_db_.ForEachComponent(in_class, in_predicate);
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

std::string WLevel::ActorPath(const WClass * in_class) const {
    assert(in_class == WEntity::StaticClass() || WEntity::StaticClass()->IsBaseOf(in_class));

    return std::string(Name()) + ":" + in_class->Name() + "_" +
        std::format("{}", entity_component_db_.EntityCount(in_class));
}

std::string WLevel::ComponentPath(const WEntityId & in_entity_id,
                                  const WClass * in_class) const {

    TWRef<WEntity> actor = entity_component_db_.GetEntity(in_entity_id);

    return std::string(actor->Name()) + ":" + in_class->Name();
}

