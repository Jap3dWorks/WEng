#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TFunction.hpp"
#include "TSparseSet.hpp"

class IObjectDataBase {
public:
    virtual ~IObjectDataBase()=default;
    
    virtual WId Create() = 0;
    virtual void Remove(WId) =0;
    virtual void Clear() = 0;
    virtual void Get(WId, void* &) = 0;
    virtual void Get(WId, const void*&) const = 0;
    virtual size_t Count()=0;
    virtual bool Contains(WId in_id);
};

template<typename T>
class TObjectDataBase : public IObjectDataBase {

public:

    TObjectDataBase() :
        destroy_fn_([](T&)->void{}),
        create_fn_([](WId){return T{}; }) {}

    ~TObjectDataBase() {
        Clear();
    }

    TObjectDataBase(const TObjectDataBase & other) = delete;

    TObjectDataBase(TObjectDataBase && other) {
        Move(std::move(other));
    }

    TObjectDataBase & operator=(const TObjectDataBase & other) = delete;

    TObjectDataBase & operator=(TObjectDataBase && other) {
        Move(std::move(other));
        return *this;
    }

    WId Create(TFunction<T(WId)> in_predicate) {
        WId oid = id_pool_.Generate();
        objects_.Insert(oid, in_predicate(oid));

        return oid;
    }

    WId Create() override final {
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

    size_t Count() override final {
        return objects_.Count();
    }

    bool Contains(WId in_id) override final {
        return objects_.Contains(in_id);
    }

    void SetCreateFn(TFunction<T(WId)> in_create_fn) {
        create_fn_ = in_create_fn;
    }

    void SetDestroyFn(TFunction<void(T&)> in_destroy_fn) {
        destroy_fn_ = in_destroy_fn;
    }

private:

    void Move(TObjectDataBase && other) {
        destroy_fn_ = std::move(other.destroy_fn_);
        create_fn_ = std::move(other.create_fn_);
        objects_ = std::move(other.objects_);
        id_pool_ = std::move(other.id_pool_);
    }

    TFunction<void(T&)> destroy_fn_; // =[](T&)->void{};
    TFunction<T(WId)> create_fn_; // =[](WId) -> T {return T{};};

    WIdPool id_pool_{};
    TSparseSet<T> objects_{};
};
