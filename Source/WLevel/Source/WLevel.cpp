#include "WLevel/WLevel.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WEngineObjects/WComponent.hpp"

#include <format>

WLevel::WLevel(const char * in_name) :
    name_(in_name),
    object_manager_(),
    actor_id_pool_(),
    id_actorclass_(),
    init_fn_([](ILevel*){}),
    update_fn_([](ILevel*, const WEngineCycleData &){}),
    close_fn_([](ILevel*){})
{}

WLevel::WLevel(const char * in_name,
               const InitFn & in_init_fn,
               const UpdateFn & in_update_fn,
               const CloseFn & in_close_fn) :
    name_(in_name),
    object_manager_(),
    actor_id_pool_(),
    id_actorclass_(),
    init_fn_(in_init_fn),
    update_fn_(in_update_fn),
    close_fn_(in_close_fn)
{}

WLevel::WLevel(WLevel && other) :
    name_(std::move(other.name_)),
    object_manager_(std::move(other.object_manager_)),
    actor_id_pool_(std::move(other.actor_id_pool_)),
    id_actorclass_(std::move(other.id_actorclass_)),
    init_fn_(std::move(other.init_fn_)),
    update_fn_(std::move(other.update_fn_)),
    close_fn_(std::move(other.close_fn_))
{
    other.name_=nullptr;
}

WLevel & WLevel::operator=(WLevel && other) {
    if (this != &other) {
        name_ = std::move(other.name_);
        object_manager_ = std::move(other.object_manager_);
        actor_id_pool_ = std::move(other.actor_id_pool_);
        id_actorclass_ = std::move(other.id_actorclass_);
        init_fn_ = std::move(other.init_fn_);
        update_fn_ = std::move(other.update_fn_);
        close_fn_ = std::move(other.close_fn_);

        other.name_=nullptr;
    }

    return *this;
}

void WLevel::Init() {
    init_fn_(this);
}

WId WLevel::CreateActor(const WClass * in_class) {
    assert(in_class == WActor::StaticClass() ||
           WActor::StaticClass()->IsBaseOf(in_class));
    
    std::string actor_path =
        Name() + ":" + in_class->Name() + "_" +
        std::format("{}", object_manager_.Size(in_class));

    WId id = CreateActorId(in_class);

    object_manager_.Create(
        in_class,
        id,
        actor_path.c_str()
        );

    object_manager_.Get<WActor>(id)->Level(this);

    return id;
}

TWRef<WActor> WLevel::GetActor(const WId & in_id) const {
    assert(id_actorclass_.contains(in_id));
    
    return static_cast<WActor*>(object_manager_.Get(
                                    id_actorclass_.at(in_id),
                                    in_id)
                                .Ptr());
}

void WLevel::ForEachActor(const WClass * in_class, TFunction<void(WActor*)> in_predicate) const {
    assert(in_class == WActor::StaticClass() || WActor::StaticClass()->IsBaseOf(in_class));
    
    for(auto& c : object_manager_.Classes()) {
        if(in_class == c || in_class->IsBaseOf(c)) {
            object_manager_.ForEach(in_class,
                                    [&in_predicate](WObject* _obj) {
                                        in_predicate(static_cast<WActor*>(_obj));
                                    }
                );
        }
    }
}

WId WLevel::CreateComponent(const WId & in_actor_id,
                            const WClass * in_class) {
    
    assert(WComponent::StaticClass()->IsBaseOf(in_class));

    assert(object_manager_.Contains(id_actorclass_[in_actor_id], in_actor_id));

    std::string component_path=ComponentPath(in_actor_id, in_class);
    
    object_manager_.Create(in_class,
                           in_actor_id,
                           component_path.c_str());

    return {};
}

TWRef<WComponent> WLevel::GetComponent(const WClass * in_class,
                                    const WId & in_component_id) const {

    assert(object_manager_.Contains(in_class, in_component_id));

    return static_cast<WComponent*>(object_manager_.Get(
                                        in_class,
                                        in_component_id
                                        ).Ptr());
}

void WLevel::Update(const WEngineCycleData & in_cycle_data) {
    update_fn_(this, in_cycle_data);
}

void WLevel::Close() {
    close_fn_(this);
}

std::string WLevel::Name() const {
    return name_;
}

std::string WLevel::ActorPath(const WClass * in_class) const {
    assert(in_class == WActor::StaticClass() || WActor::StaticClass()->IsBaseOf(in_class));
    
    return Name() + ":" + in_class->Name() + "_" +
        std::format("{}", object_manager_.Size(in_class));
}

std::string WLevel::ComponentPath(const WId & in_actor_id,
                                  const WClass * in_class) const {
    assert(object_manager_.Contains(id_actorclass_.at(in_actor_id), in_actor_id));
    assert((in_class == WActor::StaticClass() || WActor::StaticClass()->IsBaseOf(in_class)));

    return object_manager_.Get(id_actorclass_.at(in_actor_id),
                               in_actor_id)->Name() +
        ":" +
        in_class->Name();
}

WId WLevel::CreateActorId(const WClass * in_class) {
    assert(WActor::StaticClass() == in_class ||
           WActor::StaticClass()->IsBaseOf(in_class));

    WId id = actor_id_pool_.Generate();
    id_actorclass_[id] = in_class;

    return id;
}
