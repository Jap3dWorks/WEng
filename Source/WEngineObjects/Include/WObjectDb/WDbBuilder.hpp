#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WCore/TWAllocator.hpp"
#include "WLog.hpp"

#include <format>
#include <memory>
#include <unordered_map>
#include <typeindex>

struct WDbBuilder {
    
private:

    template<typename T>
    static inline void AllocateFn(T * pptr, std::size_t pn, T * nptr, std::size_t nn) {
        if (pptr) {
            for(size_t i=0; i<pn; i++) {
                if (!BWRef::IsInstanced(pptr + i)) continue;
                        
                for (auto & ref : BWRef::Instances(pptr + i)) {
                    if (ref == nullptr) continue;
                            
                    ref->BSet(nptr + i);
                }
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
    }

    template<typename T, typename I>
    static inline T CrtFn(const I & in_id) {
        return T{};
    }

    template<typename T>
    static inline void DstrFn(T & in_obj) {}

    template<typename T>
    using TAllocatorBld = TWAllocator<T, decltype(&AllocateFn<T>), decltype(&DeallocateFn<T>)>;

    template<typename T, CConvertibleTo<T> B, typename I>
    using TObjectDbBld = TObjDb<T,
                                decltype(&CrtFn<T,I>),
                                decltype(&DstrFn<T>),
                                B, I,
                                TAllocatorBld<T>>;

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

    virtual ~WDbBuilder()=default;

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
        // dbops_[typeid(RegKey<B,I>)] = std::make_unique<TDbOps<T,B,I>>();
    }

    template<typename T, CConvertibleTo<T> B, typename I>
    TObjectDbBld<T,B,I>* DbCast(DbType<B,I> * in_db) const {
        return static_cast<TObjectDbBld<T,B,I>*>(in_db);
    }

    // template<typename B, typename I, CCallable<void, B*> TFn>
    // void ForEachPtr(DbType<B,I> * in_db, TFn && in_fn) {
    //     // dbops_[typeid(RegKey<B,I>)]->ForEach(std::forward<TFn>(in_fn));
    // }

private:

    template<typename T, CConvertibleTo<T> B, typename I>
    static inline VoidPtr CreateObjectDb() {

        TAllocatorBld<T> a;

        a.SetAllocateFn(&AllocateFn<T>);

        a.SetDeallocateFn(&DeallocateFn<T>);

        return new TObjectDbBld<T,B,I>(&CrtFn, &DstrFn, a);
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
    // DbOpsReg dbops_;
    
};
