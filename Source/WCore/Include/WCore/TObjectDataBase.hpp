#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TFunction.hpp"
#include "TSparseSet.hpp"

#include <memory>

template<typename T=void, typename WIdClass=WId>
class IObjectDataBase {
public:
    virtual ~IObjectDataBase()=default;

    virtual std::unique_ptr<IObjectDataBase> Clone() const=0;
    // virtual 
    /** Create and assign an WId */
    virtual WIdClass Create() = 0;
    /** Insert at WId  */
    virtual void Insert(WIdClass)=0;
    virtual void Insert(WIdClass, T* &)=0;
    virtual void Remove(WIdClass) =0;
    virtual void Clear() = 0;
    virtual void Get(WIdClass, T* &) = 0;
    virtual void Get(WIdClass, const T* &) const = 0;
    virtual size_t Count() const = 0;
    virtual bool Contains(WIdClass in_id) const = 0;
    virtual void Reserve(size_t in_value) = 0;
    virtual std::vector<WIdClass> Indexes() = 0;
    virtual void ForEach(TFunction<void(T*)> in_function)=0;
    
};

/**
 * @brief Container for objects of type T.
 * Objects are aligned continously in memory.
 * Object in the containe can be accesed by WId.
 * You can specify a create_fn to create objects inside the container.
 * And a destroy_fn called when remove objects in the container.
 */
template<typename T, typename P=void, typename WIdClass=WId, typename Allocator=std::allocator<T>>
class TObjectDataBase : public IObjectDataBase<P, WIdClass> {
public:

    using Super = IObjectDataBase<P>;
    using Type = TObjectDataBase<T, P, Allocator>;
    using ObjectsType = TSparseSet<T, Allocator>;
    using ConstIterIndexType = TIterator<WIdClass,
                                         typename ObjectsType::ConstIndexIterator,
                                         WIdClass,
                                         WIdClass>;

    using CreateFn = TFunction<T(const WIdClass &)>;
    using DestroyFn = TFunction<void(T&)>;

public:

    constexpr TObjectDataBase() noexcept :
        create_fn_([](WIdClass) -> T {return T{};}),
        destroy_fn_([](T&)->void {}),
        id_pool_(),
        objects_()
        {}

    constexpr TObjectDataBase(
        const Allocator & in_allocator
        ) :
        create_fn_([](WIdClass) -> T { return T{}; }),
        destroy_fn_([](T&) -> void {}),
        id_pool_(),
        objects_(in_allocator)
        {}

    constexpr TObjectDataBase(
        const CreateFn & in_create_fn,
        const DestroyFn & in_destroy_fn
        ) :
        create_fn_(in_create_fn),
        destroy_fn_(in_destroy_fn),
        id_pool_(),
        objects_()
        {}

    constexpr TObjectDataBase(
        const CreateFn & in_create_fn,
        const DestroyFn & in_destroy_fn,
        const Allocator & in_allocator
        ) :
        create_fn_(in_create_fn),
        destroy_fn_(in_destroy_fn),
        id_pool_(),
        objects_(in_allocator)
        {}

    virtual ~TObjectDataBase() {
        Clear();
    }

    TObjectDataBase(const TObjectDataBase & other) :
        create_fn_(other.create_fn_),
        destroy_fn_(other.destroy_fn_),
        id_pool_(other.id_pool_),
        objects_(other.objects_) {}

    constexpr TObjectDataBase(TObjectDataBase && other) noexcept :
        create_fn_(std::move(other.create_fn_)),
        destroy_fn_(std::move(other.destroy_fn_)),
        id_pool_(std::move(other.id_pool_)),
        objects_(std::move(other.objects_))
        {}

    TObjectDataBase & operator=(const TObjectDataBase & other) {
        if (this != &other) {
            Clear();
            create_fn_ = other.create_fn_;
            destroy_fn_ = other.destroy_fn_;
            objects_ = other.objects_;
            id_pool_ = other.id_pool_;
        }

        return *this;
    }

    TObjectDataBase & operator=(TObjectDataBase && other) noexcept {
        if (this != &other) {
            Clear();
            create_fn_ = std::move(other.create_fn_);
            destroy_fn_ = std::move(other.destroy_fn_);
            objects_ = std::move(other.objects_);
            id_pool_ = std::move(other.id_pool_);            
        }

        return *this;
    }

    virtual std::unique_ptr<Super> Clone() const override {
        return std::make_unique<Type>(*this);
    }

    WId Create(const CreateFn & in_predicate) {
        WIdClass oid = id_pool_.Generate();
        objects_.Insert(oid, in_predicate(oid));

        return oid;
    }

    /**
     * @brief Create an object in the container and return the assigned id.
     * create_fn is used to create the object.
     */
    WId Create() override {
        WIdClass oid = id_pool_.Generate();
        objects_.Insert(oid, create_fn_(oid));

        return oid;
    }

    void Insert(WIdClass in_id) override {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id, create_fn_(in_id));
    }

    void Insert(WIdClass in_id, P* & in_value) override {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id, *static_cast<T*>(in_value));
    }

    void Insert(WIdClass in_id, const CreateFn & in_predicate) {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id, in_predicate(in_id));
    }

    void Remove(WIdClass in_id, const DestroyFn & in_destroy_fn) {
        in_destroy_fn(objects_.Get(in_id));
        id_pool_.Release(in_id);
        objects_.Delete(in_id);
    }

    void Remove(WIdClass in_id) override final {
        destroy_fn_(objects_.Get(in_id));
        id_pool_.Release(in_id);
        objects_.Delete(in_id);
    }

    void Clear(const DestroyFn & in_destroy_fn) {
        for (auto & o : objects_) {
            in_destroy_fn(o);
        }
        
        id_pool_.Clear();
        objects_.Clear();
    }

    void Clear() override final {
        for (auto & o : objects_) {
            destroy_fn_(o);
        }
        
        id_pool_.Clear();
        objects_.Clear();
    }

    T & Get(WIdClass in_id) {
        return objects_.Get(in_id);
    }

    const T & Get(WIdClass in_id) const {
        return objects_.Get(in_id);
    }

    void Get(WIdClass in_id, P* & out_value) override final {
        out_value = &objects_.Get(in_id);
    }

    void Get(WIdClass in_id, const P* & out_value) const override final {
        out_value = &objects_.Get(in_id);
    }

    size_t Count() const override final {
        return objects_.Count();
    }

    bool Contains(WIdClass in_id) const override final {
        return objects_.Contains(in_id);
    }

    void SetCreateFn(const CreateFn & in_create_fn) {
        create_fn_ = in_create_fn;
    }

    void SetDestroyFn(const DestroyFn & in_destroy_fn) {
        destroy_fn_ = in_destroy_fn;
    }

    void Reserve(size_t in_value) override {
        objects_.Reserve(in_value);
    }

    ConstIterIndexType IterIndexes() const {
        return ConstIterIndexType(
            objects_.IterIndexes().begin(),
            objects_.IterIndexes().end(),
            [](ConstIterIndexType::IterType & _itr, const size_t & _val) {
                return *_itr;
            }
            );
    }

    std::vector<WIdClass> Indexes() override {
        std::vector<WIdClass> result;
        result.reserve(objects_.Count());
        
        for (WIdClass d : IterIndexes()) {
            result.push_back(d);
        }

        return result;
    }

    void ForEach(TFunction<void(P*)> in_function) override {
        for (auto& v : objects_) {
            in_function(static_cast<P*>(&v));
        }
    }

private:

    CreateFn create_fn_;

    DestroyFn destroy_fn_; 

    WIdPool<WIdClass> id_pool_;

    ObjectsType objects_;
    
};
