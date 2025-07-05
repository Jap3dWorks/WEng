#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TFunction.hpp"
#include "TSparseSet.hpp"

class WCORE_API IObjectDataBase {
public:
    virtual ~IObjectDataBase()=default;
    
    virtual WId Create() = 0;
    virtual void Remove(WId) =0;
    virtual void Clear() = 0;
    virtual void Get(WId, void* &) = 0;
    virtual void Get(WId, const void*&) const = 0;
    virtual size_t Count() const = 0;
    virtual bool Contains(WId in_id) const = 0;
};

template<typename T>
class TObjectDataBase : public IObjectDataBase {

public:

    constexpr TObjectDataBase() noexcept :
        destroy_fn_([](T&)->void{}),
        create_fn_([](WId) -> T {return T{};}),
        id_pool_(),
        objects_()
        {}

    virtual ~TObjectDataBase() {
        Clear();
    }

    TObjectDataBase(const TObjectDataBase & other) = delete;

    constexpr TObjectDataBase(TObjectDataBase && other) noexcept :
        destroy_fn_(std::move(other.destroy_fn_)),
        create_fn_(std::move(other.create_fn_)),
        id_pool_(std::move(other.id_pool_)),
        objects_(std::move(other.objects_))
        {}

    TObjectDataBase & operator=(const TObjectDataBase & other) = delete;

    constexpr TObjectDataBase & operator=(TObjectDataBase && other) noexcept {
        Move(std::move(other));
        return *this;
    }

    WId Create(TFunction<T(WId)> in_predicate) {
        WId oid = id_pool_.Generate();
        objects_.Insert(oid, in_predicate(oid));

        return oid;
    }

    WId Create() override {
        WId oid = id_pool_.Generate();
        objects_.Insert(oid, create_fn_(oid));

        return oid;
    }

    void Remove(WId in_id, TFunction<void(T&)> in_destroy_fn) {
        in_destroy_fn(objects_.Get(in_id));
        id_pool_.Release(in_id);
        objects_.Delete(in_id);
    }

    void Remove(WId in_id) override final {
        destroy_fn_(objects_.Get(in_id));
        id_pool_.Release(in_id);
        objects_.Delete(in_id);
    }

    void Clear(TFunction<void(T&)> in_destroy_fn) {
        for (auto & o : objects_) {
            in_destroy_fn(o);
        }
        
        id_pool_.Reset();
        objects_.Clear();
    }

    void Clear() override final {
        for (auto & o : objects_) {
            destroy_fn_(o);
        }
        
        id_pool_.Reset();
        objects_.Clear();
    }

    T & Get(WId in_id) {
        return objects_.Get(in_id);
    }

    const T & Get(WId in_id) const {
        return objects_.Get(in_id);
    }

    void Get(WId in_id, void* & out_value) override final {
        out_value = &objects_.Get(in_id);
    }

    void Get(WId in_id, const void*& out_value) const override final {
        out_value = &objects_.Get(in_id);
    }

    size_t Count() const override final {
        return objects_.Count();
    }

    bool Contains(WId in_id) const override final {
        return objects_.Contains(in_id);
    }

    void SetCreateFn(TFunction<T(WId)> in_create_fn) {
        create_fn_ = in_create_fn;
    }

    void SetDestroyFn(TFunction<void(T&)> in_destroy_fn) {
        destroy_fn_ = in_destroy_fn;
    }

private:

    constexpr void Move(TObjectDataBase && other) noexcept {
        destroy_fn_ = std::move(other.destroy_fn_);
        create_fn_ = std::move(other.create_fn_);
        objects_ = std::move(other.objects_);
        id_pool_ = std::move(other.id_pool_);
    }

    TFunction<void(T&)> destroy_fn_; 
    TFunction<T(const WId &)> create_fn_;
    WIdPool id_pool_;
    TSparseSet<T> objects_;
    
};
