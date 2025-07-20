#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TFunction.hpp"
#include "TSparseSet.hpp"

template<typename T=void>
class IObjectDataBase {
public:
    virtual ~IObjectDataBase()=default;

    /** Create and assign an WId */
    virtual WId Create() = 0;
    /** Insert at WId  */
    virtual void Insert(WId)=0;
    virtual void Insert(WId, T* &)=0;
    virtual void Remove(WId) =0;
    virtual void Clear() = 0;
    virtual void Get(WId, T* &) = 0;
    virtual void Get(WId, const T* &) const = 0;
    virtual size_t Count() const = 0;
    virtual bool Contains(WId in_id) const = 0;
    virtual void Reserve(size_t in_value) = 0;
    virtual std::vector<WId> Indexes() = 0;
    virtual void ForEach(TFunction<void(T*)> in_function)=0;
    
};

template<typename T, typename P=void, typename Allocator=std::allocator<T>>
class TObjectDataBase : public IObjectDataBase<P> {
public:

    using ObjectsType = TSparseSet<T, Allocator>;

    using IterIndex = TIterator<WId, typename ObjectsType::IndexIterator>;

public:

    constexpr TObjectDataBase() noexcept :
        create_fn_([](WId) -> T {return T{};}),
        clear_fn_([](T&)->void {}),
        id_pool_(),
        objects_()
        {}

    constexpr TObjectDataBase(
        const Allocator & in_allocator
        ) :
        create_fn_([](WId) -> T { return T{}; }),
        clear_fn_([](T&) -> void {}),
        id_pool_(),
        objects_(in_allocator)
        {}

    constexpr TObjectDataBase(
        const TFunction<T(const WId &)> & in_create_fn,
        const TFunction<void(T&)> & in_destroy_fn
        ) :
        create_fn_(in_create_fn),
        clear_fn_(in_destroy_fn),
        id_pool_(),
        objects_()
        {}

    constexpr TObjectDataBase(
        const TFunction<T(const WId &)> & in_create_fn,
        const TFunction<void(T&)> & in_destroy_fn,
        const Allocator & in_allocator
        ) :
        create_fn_(in_create_fn),
        clear_fn_(in_destroy_fn),
        id_pool_(),
        objects_(in_allocator)
        {}

    virtual ~TObjectDataBase() {
        Clear();
    }

    TObjectDataBase(const TObjectDataBase & other) = delete;

    constexpr TObjectDataBase(TObjectDataBase && other) noexcept :
        create_fn_(std::move(other.create_fn_)),
        clear_fn_(std::move(other.clear_fn_)),
        id_pool_(std::move(other.id_pool_)),
        objects_(std::move(other.objects_))
        {}

    TObjectDataBase & operator=(const TObjectDataBase & other) = delete;

    TObjectDataBase & operator=(TObjectDataBase && other) noexcept {
        if (this != &other) {
            Clear();
            create_fn_ = std::move(other.create_fn_);
            clear_fn_ = std::move(other.clear_fn_);
            objects_ = std::move(other.objects_);
            id_pool_ = std::move(other.id_pool_);            
        }
        
        return *this;
    }

    WId Create(const TFunction<T(const WId &)> & in_predicate) {
        WId oid = id_pool_.Generate();
        objects_.Insert(oid, in_predicate(oid));

        return oid;
    }

    WId Create() override {
        WId oid = id_pool_.Generate();
        objects_.Insert(oid, create_fn_(oid));

        return oid;
    }

    void Insert(WId in_id) override {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id, create_fn_(in_id));
    }

    void Insert(WId in_id, P* & in_value) override {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id, *static_cast<T*>(in_value));
    }

    void Insert(WId in_id, const TFunction<T(const WId &)> & in_predicate) {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id, in_predicate(in_id));
    }

    void Remove(WId in_id, const TFunction<void(T&)> & in_destroy_fn) {
        in_destroy_fn(objects_.Get(in_id));
        id_pool_.Release(in_id);
        objects_.Delete(in_id);
    }

    void Remove(WId in_id) override final {
        clear_fn_(objects_.Get(in_id));
        id_pool_.Release(in_id);
        objects_.Delete(in_id);
    }

    void Clear(const TFunction<void(T&)> & in_destroy_fn) {
        for (auto & o : objects_) {
            in_destroy_fn(o);
        }
        
        id_pool_.Clear();
        objects_.Clear();
    }

    void Clear() override final {
        for (auto & o : objects_) {
            clear_fn_(o);
        }
        
        id_pool_.Clear();
        objects_.Clear();
    }

    T & Get(WId in_id) {
        return objects_.Get(in_id);
    }

    const T & Get(WId in_id) const {
        return objects_.Get(in_id);
    }

    void Get(WId in_id, P* & out_value) override final {
        out_value = &objects_.Get(in_id);
    }

    void Get(WId in_id, const P* & out_value) const override final {
        out_value = &objects_.Get(in_id);
    }

    size_t Count() const override final {
        return objects_.Count();
    }

    bool Contains(WId in_id) const override final {
        return objects_.Contains(in_id);
    }

    void SetCreateFn(const TFunction<T(WId)> & in_create_fn) {
        create_fn_ = in_create_fn;
    }

    void SetClearFn(const TFunction<void(T&)> & in_destroy_fn) {
        clear_fn_ = in_destroy_fn;
    }

    void Reserve(size_t in_value) override {
        objects_.Reserve(in_value);
    }

    IterIndex Indexes() override {
        // std::vector<WId> result;
        // result.reserve(objects_.Count());

        objects_.IterIndex();

        return IterIndex(
            objects_.IterIndex().begin(),
            objects_.IterIndex().end(),
            [](TSparseSet<T,Allocator>::IndexIterator & _itr, const size_t & _val) {
                return *_itr;
            }
            );
    }

    void ForEach(TFunction<void(P*)> in_function) override {
        for (auto& v : objects_) {
            in_function(static_cast<P*>(&v));
        }
    }

private:

    TFunction<T(const WId &)> create_fn_;
    TFunction<void(T&)> clear_fn_; 

    WIdPool id_pool_;
    TSparseSet<T, Allocator> objects_;
    
};
