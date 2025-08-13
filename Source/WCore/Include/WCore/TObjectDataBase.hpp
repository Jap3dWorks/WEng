#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TFunction.hpp"
#include "TSparseSet.hpp"
#include "WCore/WConcepts.hpp"

#include <memory>
#include <concepts>

template<typename B=void, typename WIdClass=WId>
class IObjectDataBase {
public:
    virtual ~IObjectDataBase()=default;

    virtual std::unique_ptr<IObjectDataBase> Clone() const=0;
    // virtual 
    /** Create and assign an WIdClass */
    virtual WIdClass Create() = 0;
    /** Create at WIdClass */
    virtual void CreateAt(const WIdClass &)=0;
    /** Insert At WIdClass */
    virtual void InsertAt(const WIdClass &, B* &)=0;
    virtual void Remove(const WIdClass &) =0;
    virtual void Clear() = 0;
    virtual void Get(const WIdClass &, B* &) = 0;
    virtual void Get(const WIdClass &, const B* &) const = 0;
    virtual size_t Count() const = 0;
    virtual bool Contains(const WIdClass & in_id) const = 0;
    virtual void Reserve(size_t in_value) = 0;
    virtual std::vector<WIdClass> Indexes() = 0;
    
    virtual void ForEach(TFunction<void(B*)> in_function)=0;
    virtual void ForEachId(TFunction<void(const WIdClass & _id, B * _value)>) =0;
    
};

/**
 * @brief Container for objects of type T.
 * Objects are aligned continously in memory.
 * Object in the containe can be accesed by WId.
 * You can specify a create_fn to create objects inside the container.
 * And a destroy_fn called when remove objects in the container.
 */
template<typename T, CConvertibleTo<T> B=void, typename WIdClass=WId, typename Allocator=std::allocator<T>>
class TObjectDataBase : public IObjectDataBase<B, WIdClass> {
public:

    using Super = IObjectDataBase<B, WIdClass>;
    using Type = TObjectDataBase<T, B, WIdClass, Allocator>;
    using ObjectsType = TSparseSet<T, Allocator>;
    using ConstIterIndexType = TIterator<WIdClass,
                                         typename ObjectsType::ConstIndexIterator,
                                         WIdClass,
                                         WIdClass>;

    using CreateFn = TFunction<T(const WIdClass &)>;
    using DestroyFn = TFunction<void(T&)>;

public:

    constexpr TObjectDataBase() noexcept :
        create_fn_([](const WIdClass &) -> T {return T{};}),
        destroy_fn_([](T&)->void {}),
        id_pool_(),
        objects_()
        {}

    constexpr TObjectDataBase(
        const Allocator & in_allocator
        ) :
        create_fn_([](const WIdClass &) -> T { return T{}; }),
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
        objects_(other.objects_)
        {}

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
        return std::make_unique<TObjectDataBase>(*this);
    }

    WIdClass Create(const CreateFn & in_predicate) {
        WIdClass oid = id_pool_.Generate();
        objects_.Insert(oid.GetId(), in_predicate(oid));

        return oid;
    }

    /**
     * @brief Create an object in the container and return the assigned id.
     * create_fn is used to create the object.
     */
    WIdClass Create() override {
        WIdClass oid = id_pool_.Generate();
        objects_.Insert(oid.GetId(), create_fn_(oid));

        return oid;
    }

    void CreateAt(const WIdClass & in_id) override {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id.GetId(), create_fn_(in_id));
    }

    void CreateAt(const WIdClass & in_id, const CreateFn & in_predicate) {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id.GetId(), in_predicate(in_id));
    }

    WIdClass Insert(const T & in_value) {
        WIdClass oid = id_pool_.Generate();
        objects_.Insert(oid.GetId(), in_value);
        
        return oid;
    }

    WIdClass Insert(T && in_value) {
        WIdClass oid = id_pool_.Generate();
        objects_.Insert(oid.GetId(), std::move(in_value));

        return oid;
    }

    void InsertAt(const WIdClass & in_id, B* & in_value) override {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id.GetId(), *static_cast<T*>(in_value));
    }

    void Remove(const WIdClass & in_id, const DestroyFn & in_destroy_fn) {
        in_destroy_fn(objects_.Get(in_id.GetId()));
        id_pool_.Release(in_id);
        objects_.Remove(in_id.GetId());
    }

    void Remove(const WIdClass & in_id) override final {
        destroy_fn_(objects_.Get(in_id.GetId()));
        id_pool_.Release(in_id);
        objects_.Remove(in_id.GetId());
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

    T & Get(const WIdClass & in_id) {
        return objects_.Get(in_id.GetId());
    }

    const T & Get(const WIdClass & in_id) const {
        return objects_.Get(in_id.GetId());
    }

    void Get(const WIdClass & in_id, B* & out_value) override final {
        out_value = &objects_.Get(in_id.GetId());
    }

    void Get(const WIdClass & in_id, const B* & out_value) const override final {
        out_value = &objects_.Get(in_id.GetId());
    }

    size_t Count() const override final {
        return objects_.Count();
    }

    bool Contains(const WIdClass & in_id) const override final {
        return objects_.Contains(in_id.GetId());
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

    constexpr ObjectsType::Iterator begin() noexcept {
        return objects_.begin();
    }

    constexpr ObjectsType::Iterator end() noexcept {
        return objects_.end();
    }

    constexpr ObjectsType::ConstIterator cbegin() const noexcept {
        return objects_.cbegin();
    }

    constexpr ObjectsType::ConstIterator cend() const noexcept {
        return objects_.cend();
    }

    void ForEach(TFunction<void(B*)> in_function) override {
        for (auto& v : objects_) {
            in_function(static_cast<B*>(&v));
        }
    }

    void ForEachId(TFunction<void(const WIdClass & _id, B* _value)> in_predicate) override {
        std::size_t i=0;
        for (auto& v : objects_) {
            in_predicate(objects_.IndexAt(i++),
                         static_cast<B*>(&v));
        }
    }

private:

    CreateFn create_fn_;

    DestroyFn destroy_fn_; 

    WIdPool<WIdClass> id_pool_;

    ObjectsType objects_;
    
};
