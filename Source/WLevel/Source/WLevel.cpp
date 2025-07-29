#include "WLevel/WLevel.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WEngineObjects/WComponent.hpp"

#include <format>

WLevel::WLevel() :
    name_("InvalidLevel"),
    wid_(0),
    actor_component_db_(),
    init_fn_([](WLevel*, const WEngineCycleData&){}),
    update_fn_([](WLevel*, const WEngineCycleData&){}),
    close_fn_([](WLevel*, const WEngineCycleData&){})
{}

WLevel::WLevel(const char * in_name,
               const WId & in_id) :
    name_(in_name),
    wid_(in_id),
    actor_component_db_(),
    init_fn_([](WLevel*, const WEngineCycleData &){}),
    update_fn_([](WLevel*, const WEngineCycleData &){}),
    close_fn_([](WLevel*, const WEngineCycleData &){})
{}

WLevel::WLevel(const char * in_name,
               const WId & in_id,
               const InitFn & in_init_fn,
               const UpdateFn & in_update_fn,
               const CloseFn & in_close_fn) :
    name_(in_name),
    wid_(in_id),
    actor_component_db_(),
    init_fn_(in_init_fn),
    update_fn_(in_update_fn),
    close_fn_(in_close_fn)
{}

WLevel::WLevel(const WLevel& other) :
    name_(other.name_),
    actor_component_db_(other.actor_component_db_),
    init_fn_(other.init_fn_),
    update_fn_(other.update_fn_),
    close_fn_(other.close_fn_)
{}

WLevel::WLevel(WLevel && other) :
    name_(std::move(other.name_)),
    actor_component_db_(other.actor_component_db_),
    init_fn_(std::move(other.init_fn_)),
    update_fn_(std::move(other.update_fn_)),
    close_fn_(std::move(other.close_fn_))
{
    other.name_=nullptr;
}

WLevel & WLevel::operator=(const WLevel& other) {
    if (this != &other) {
        name_ = other.name_;
        actor_component_db_ = other.actor_component_db_;
        init_fn_ = other.init_fn_;
        update_fn_ = other.update_fn_;
        close_fn_ = other.close_fn_;
    }
    return *this;
}

WLevel & WLevel::operator=(WLevel && other) {
    if (this != &other) {
        name_ = std::move(other.name_);
        actor_component_db_ = std::move(other.actor_component_db_);
        init_fn_ = std::move(other.init_fn_);
        update_fn_ = std::move(other.update_fn_);
        close_fn_ = std::move(other.close_fn_);

        other.name_=nullptr;
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

WId WLevel::CreateActor(const WClass * in_class) {
    std::string actor_path = ActorPath(in_class);

    WId id = actor_component_db_.CreateEntity(in_class, actor_path);
    
    actor_component_db_.GetEntity(id)->Level(this);

    return id;
}

TWRef<WEntity> WLevel::GetActor(const WId & in_id) const {
    return actor_component_db_.GetEntity(in_id);
}

void WLevel::ForEachActor(const WClass * in_class,
                          TFunction<void(WEntity*)> in_predicate) const {
    actor_component_db_.ForEachEntity(in_class, in_predicate);
}

WId WLevel::CreateComponent(const WId & in_actor_id,
                            const WClass * in_class) {

    std::string component_path=ComponentPath(in_actor_id, in_class);

    actor_component_db_.CreateComponent(in_actor_id, in_class, component_path);

    actor_component_db_.GetComponent(in_class, in_actor_id)->Level(this);

    return in_actor_id;
}

TWRef<WComponent> WLevel::GetComponent(const WClass * in_class,
                                       const WId & in_component_id) const {
    return actor_component_db_.GetComponent(in_class, in_component_id);
}

void WLevel::ForEachComponent(const WClass * in_class,
                              TFunction<void(WComponent*)> in_predicate) const {
    actor_component_db_.ForEachComponent(in_class, in_predicate);
}

std::string WLevel::Name() const {
    return name_;
}

WId WLevel::WID() const {
    return wid_;
}

void WLevel::WID(const WId & in_id) {
    wid_ = in_id;
}

std::string WLevel::ActorPath(const WClass * in_class) const {
    assert(in_class == WEntity::StaticClass() || WEntity::StaticClass()->IsBaseOf(in_class));

    return Name() + ":" + in_class->Name() + "_" +
        std::format("{}", actor_component_db_.EntityCount(in_class));
}

std::string WLevel::ComponentPath(const WId & in_actor_id,
                                  const WClass * in_class) const {
    assert(object_manager_.Contains(id_actorclass_.at(in_actor_id), in_actor_id));
    assert((in_class == WEntity::StaticClass() || WEntity::StaticClass()->IsBaseOf(in_class)));

    TWRef<WEntity> actor = actor_component_db_.GetEntity(in_actor_id);

    return actor->Name() + ":" + in_class->Name();
}

