#include "WObjectManager/WObjectManager.hpp"

TWRef<WObject> WObjectManager::CreateObject(const WClass * in_class, const char * in_fullname) {
    if (!containers_.contains(in_class)) {
        containers_[in_class] =
            in_class->CreateObjectDatabase();
            
        containers_[in_class]->Reserve(
            initial_memory_size_
            );
    }

    WId id = containers_[in_class]->Create();
        
    WObject * obj;
    containers_[in_class]->Get(id, obj);

    obj->WID(id);
    obj->Name(in_fullname);

    return obj;
}

TWRef<WObject> WObjectManager::GetObject(const WClass * in_class, WId in_id) {
    WObject * result;
    containers_[in_class]->Get(in_id, result);

    return static_cast<WObject*>(result);
}

void WObjectManager::ForEach(const WClass * in_class, TFunction<void(WObject*)> in_predicate) {
    containers_[in_class]->ForEach(in_predicate);
}

void WObjectManager::InitialMemorySize(size_t in_ammount) {
    initial_memory_size_ = in_ammount;
}

size_t WObjectManager::InitialMemorySize() const {
    return initial_memory_size_;
}

