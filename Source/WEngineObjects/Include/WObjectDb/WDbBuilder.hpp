#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WCore/TFunction.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WCore/TWAllocator.hpp"
#include "WLog.hpp"

#include <format>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <typeindex>
#include <unordered_set>

struct WDbBuilder {
    
private:

    using OnAllocateEventFn = void(*)(void*, std::size_t, void*, std::size_t);

    using OnDeallocateEventFn = void(*)(void*, std::size_t);

    static inline std::unordered_map<std::type_index,
                                     std::unordered_set<OnAllocateEventFn>> on_allocate_events{};

    static inline std::unordered_map<std::type_index,
                                     std::unordered_set<OnDeallocateEventFn>> on_deallocate_events{};

    template<typename T>
    static inline void AllocateFn(T * prev_ptr, std::size_t prev_n, T * new_ptr, std::size_t new_n) {
        if (prev_ptr) {
            for(size_t i=0; i<prev_n; i++) {
                if (!BWRef::IsInstanced(prev_ptr + i)) continue;
                        
                for (auto & ref : BWRef::Instances(prev_ptr + i)) {
                    if (ref == nullptr) continue;
                            
                    ref->BSet(new_ptr + i);
                }
            }
        }

        if (on_allocate_events.contains(typeid(T))) {
            for (auto * fn : on_allocate_events[typeid(T)]) {
                fn(prev_ptr, prev_n, new_ptr, new_n);
            }
        }

    }

    template<typename T>
    static inline void DeallocateFn(T * ptr, std::size_t n) {
        if (ptr) {
            for(std::size_t i=0; i<n; i++) {
                if (!BWRef::IsInstanced(ptr + i)) continue;

                for (auto & ref : BWRef::Instances(ptr + i)) {
                    if (ref == nullptr) continue;

                    ref->BSet(ptr + i);
                }
            }
        }

        if (on_deallocate_events.contains(typeid(T))) {
            for(auto * fn : on_deallocate_events[typeid(T)]) {
                fn(ptr,n);
            }
        }
    }

    template<typename T, typename I>
    static inline T CreateFn(const I & in_id) {
        return T{};
    }

    template<typename T>
    static inline void DestroyFn(T & in_obj) {}

    template<typename T>
    using TAllocatorBld = TWAllocator<T, decltype(&AllocateFn<T>), decltype(&DeallocateFn<T>)>;

    template<typename T, CConvertibleTo<T> B, typename I>
    using TObjectDbBld = TObjDb<T,
                                decltype(&CreateFn<T,I>),
                                decltype(&DestroyFn<T>),
                                B, I,
                                TAllocatorBld<T>>;

public:

    template<typename T>
    static inline void RegisterOnAllocateEvent(OnAllocateEventFn fn) {
        if (!on_allocate_events.contains(typeid(T))) {
            on_allocate_events[typeid(T)]={};
        }
        on_allocate_events[typeid(T)].insert(fn);
    }

    template<typename T>
    static inline void RegisterOnDeallocateEvent(OnDeallocateEventFn fn) {
        if (!on_deallocate_events.contains(typeid(T))) {
            on_deallocate_events[typeid(T)]={};
        }
        on_deallocate_events[typeid(T)].insert(fn);
    }

public:

    template<typename B, typename I>
    using DbType = IObjectDataBase<B,I>;
    
    template<typename B, typename I>
    using DbRawPtr = DbType<B,I>*;

    using VoidPtr = void*;

    template<typename B, typename I>
    using RegKey = std::pair<B,I>;
    
    using CreateReg = std::unordered_map<std::type_index, TFnPtr< VoidPtr() >>;
    
    constexpr WDbBuilder()=default;

    ~WDbBuilder()=default;

    constexpr WDbBuilder(const WDbBuilder & other) :
        dbcreate_(other.dbcreate_) {}

    constexpr WDbBuilder(WDbBuilder && other) noexcept :
        dbcreate_(std::move(other.dbcreate_)) {}

    constexpr WDbBuilder & operator=(const WDbBuilder & other) {
        if (this != &other) {
            dbcreate_ = other.dbcreate_;
        }
        return *this;
    }

    constexpr WDbBuilder & operator=(WDbBuilder && other) {
        if (this != &other) {
            dbcreate_ = std::move(other.dbcreate_);
        }

        return *this;
    }

    template<typename B, typename I>
    std::unique_ptr<DbType<B,I>> Create() const {
        return std::unique_ptr<DbType<B,I>>(
            CastDb<B,I>(
                CreateDb(typeid(RegKey<B,I>)))
            );
    }

    template<typename T, CConvertibleTo<T> B, typename I>
    void RegisterBuilder() {
        if (dbcreate_.contains(typeid(RegKey<B,I>))) {
            return;
        }

        dbcreate_[typeid(RegKey<B,I>)] = &CreateObjectDb<T,B,I>;
    }

    template<typename T, CConvertibleTo<T> B, std::integral I>
    TObjectDbBld<T,B,I>* DbCast(DbType<B,I> * in_db) const {
        return static_cast<TObjectDbBld<T,B,I>*>(in_db);
    }

private:

    template<typename T, CConvertibleTo<T> B, typename I>
    static inline VoidPtr CreateObjectDb() {

        TAllocatorBld<T> a;

        a.SetAllocateFn(&AllocateFn<T>);

        a.SetDeallocateFn(&DeallocateFn<T>);

        return new TObjectDbBld<T,B,I>(&CreateFn, &DestroyFn, a);
    }

    VoidPtr CreateDb(std::type_index typeindex) const {
        assert(dbcreate_.contains(typeindex));
        
        return dbcreate_.at(typeindex)();
    }

    template<typename B, typename I>
    DbRawPtr<B,I> CastDb(VoidPtr in_cast) const {
        return static_cast<DbRawPtr<B,I>>(in_cast);
    }

    CreateReg dbcreate_;
    
};
