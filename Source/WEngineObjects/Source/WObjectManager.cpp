#include "WObjectManager/WObjectManager.hpp"

WId WObjectManager::Create(const WClass * in_class,
                           const char * in_fullname) {
    EnsureClassStorage(in_class);
    
    WId id = containers_[in_class]->Create();
        
    WObject * obj;
    containers_[in_class]->Get(id, obj);

    obj->WID(id);
    obj->Name(in_fullname);

    return id;
}

WId WObjectManager::Create(const WClass * in_class,
                           const WId& in_id,
                           const char * in_fullname) {
    EnsureClassStorage(in_class);

    assert(!containers_[in_class]->Contains(in_id));

    containers_[in_class]->Insert(in_id);

    WObject * obj;
    containers_[in_class]->Get(in_id, obj);

    obj->WID(in_id);
    obj->Name(in_fullname);

    return in_id;
}

TWRef<WObject> WObjectManager::Get(const WClass * in_class, const WId & in_id) const {
    WObject * result;
    containers_.at(in_class)->Get(in_id, result);

    return static_cast<WObject*>(result);
}

bool WObjectManager::Contains(const WClass * in_class, WId in_id) const {
    return containers_.contains(in_class) && containers_.at(in_class)->Contains(in_id);
}

bool WObjectManager::Contains(const WClass * in_class) const {
    return containers_.contains(in_class);
}

void WObjectManager::ForEach(const WClass * in_class, TFunction<void(WObject*)> in_predicate) const {
    assert(containers_.contains(in_class));
    
    containers_.at(in_class)->ForEach(in_predicate);
}

void WObjectManager::InitialMemorySize(size_t in_ammount) {
    initial_memory_size_ = in_ammount;
}

size_t WObjectManager::InitialMemorySize() const {
    return initial_memory_size_;
}

std::vector<const WClass*> WObjectManager::Classes() const {
    std::vector<const WClass*> r;
    r.reserve(containers_.size());

    for(auto& c : containers_) {
        r.push_back(c.first);
    }

    return r;
}

size_t WObjectManager::Size(const WClass * in_class) const {
    assert(containers_.contains(in_class));
    return containers_.at(in_class)->Count();
}

void WObjectManager::EnsureClassStorage(const WClass * in_class) {
    if (!containers_.contains(in_class)) {
        containers_[in_class] =
            in_class->CreateObjectDatabase();
            
        containers_[in_class]->Reserve(
            initial_memory_size_
            );
    }
}
