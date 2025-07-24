#include "WObjectDb/WObjectDb.hpp"

WId WObjectDb::Create(const WClass * in_class,
                           const char * in_fullname) {
    EnsureClassStorage(in_class);
    
    WId id = containers_[in_class]->Create();
        
    WObject * obj;
    containers_[in_class]->Get(id, obj);

    obj->WID(id);
    obj->Name(in_fullname);

    return id;
}

WId WObjectDb::Create(const WClass * in_class,
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

// TWRef<WObject> WObjectDb::Get(const WClass * in_class, const WId & in_id) {
//     WObject * result;
//     containers_.at(in_class)->Get(in_id, result);

//     return result;
// }

TWRef<WObject> WObjectDb::Get(const WClass * in_class, const WId & in_id) const {
    WObject * result;
    containers_.at(in_class)->Get(in_id, result);

    return result;
}

bool WObjectDb::Contains(const WClass * in_class, WId in_id) const {
    return containers_.contains(in_class) && containers_.at(in_class)->Contains(in_id);
}

bool WObjectDb::Contains(const WClass * in_class) const {
    return containers_.contains(in_class);
}

void WObjectDb::ForEach(const WClass * in_class, TFunction<void(WObject*)> in_predicate) const {
    assert(containers_.contains(in_class));
    
    containers_.at(in_class)->ForEach(in_predicate);
}

void WObjectDb::InitialMemorySize(size_t in_ammount) {
    initial_memory_size_ = in_ammount;
}

size_t WObjectDb::InitialMemorySize() const {
    return initial_memory_size_;
}

WObjectDb::ClassIterator WObjectDb::Classes() const {

    return ClassIterator(
        containers_.cbegin(),
        containers_.cend(),
        [](ClassIterator::IterType & _iter, const size_t & _n) -> const WClass *const {
            return (*_iter).first;
        }
        );
}

size_t WObjectDb::Count(const WClass * in_class) const {
    assert(containers_.contains(in_class));
    return containers_.at(in_class)->Count();
}

void WObjectDb::EnsureClassStorage(const WClass * in_class) {
    if (!containers_.contains(in_class)) {
        containers_[in_class] =
            in_class->CreateObjectDatabase();
            
        containers_[in_class]->Reserve(
            initial_memory_size_
            );
    }
}

std::vector<WId> WObjectDb::Indexes(const WClass * in_class) const {
    assert(containers_.contains(in_class));
    
    return containers_.at(in_class)->Indexes();
}

