#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TFunction.hpp"
#include "TSparseSet.hpp"
#include "WCore/WConcepts.hpp"

#include <memory>
#include <concepts>
#include <utility>

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

    virtual void GetFirst(B* &, WIdClass &) = 0;
    virtual void GetFirst(const B*&, WIdClass &) const =0;
    
    virtual size_t Count() const = 0;
    virtual bool Contains(const WIdClass & in_id) const = 0;
    virtual void Reserve(size_t in_value) = 0;
    virtual std::vector<WIdClass> Indexes() = 0;

    // TODO can I implement here an iterator?

    virtual void ForEach(TFunction<void(B*)> in_function)=0;
    virtual void ForEachIdValue(TFunction<void(const WIdClass & _id, B * _value)>) =0;
    
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

    template<typename IndexIterator, typename ValueFn, typename IncrFn>
    using ConstWIdIterator = TIterator<WIdClass,
                                       IndexIterator,
                                       WIdClass,
                                       ValueFn,
                                       IncrFn>;

    using CreateFn = TFunction<T(const WIdClass &)>;  // TODO: class template argument
    using DestroyFn = TFunction<void(T&)>;            // TODO: class template argument

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

    template<CCallable<T, const WIdClass &> TCreateFn>
    WIdClass Create(TCreateFn && in_create_fn) {
        WIdClass oid = id_pool_.Generate();
        objects_.Insert(oid.GetId(), std::forward<TCreateFn>(in_create_fn) (oid));

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

    template<CCallable<T, const WIdClass &> TCreateFn>
    void CreateAt(const WIdClass & in_id, TCreateFn && in_create_fn) {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id.GetId(), std::forward<TCreateFn>(in_create_fn)(in_id));
    }

    template<typename D> requires std::is_same_v<std::remove_cvref_t<D>, T>
    WIdClass Insert(T && in_value) {
        WIdClass oid = id_pool_.Generate();
        objects_.Insert(oid.GetId(), std::forward(in_value));

        return oid;
    }

    template<typename D> requires std::is_same_v<std::remove_cvref_t<D>, T>
    void InsertAt(const WIdClass & in_id, D && in_value) {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id.GetId(), std::forward<D>(in_value));
    }

    void InsertAt(const WIdClass & in_id, B* & in_value) override {
        id_pool_.Reserve(in_id);
        objects_.Insert(in_id.GetId(), *static_cast<T*>(in_value));
    }

    template<CCallable<void, T&> TDestroyFn>
    void Remove(const WIdClass & in_id, TDestroyFn && in_destroy_fn) {
        std::forward<TDestroyFn>(in_destroy_fn)(objects_.Get(in_id.GetId()));
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

    void GetFirst(B* & out_first, WIdClass & out_id) override {
        out_id = objects_.IndexInPos(0);
        out_first = &(objects_.Get(out_id.GetId()));
    }
    
    void GetFirst(const B*& out_first, WIdClass & out_id) const override {
        out_id = objects_.IndexInPos(0);
        out_first = &(objects_.Get(out_id.GetId()));
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

    auto IterIndexes() const {
        return ConstWIdIterator(
            objects_.IterIndexes().begin(),
            objects_.IterIndexes().end(),
            [](auto & _it, const std::int32_t & _id) -> WIdClass {
                return WIdClass(*_it);
            },
            [](auto & _it, const std::int32_t & _id) {
                _it++;
                return _it;
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

    template<CCallable<void, T&> TPredicate>
    void ForEach(TPredicate && in_predicate) {
        for (auto & v : objects_) {
            std::forward<TPredicate>(in_predicate)(v);
        }
    }

    void ForEach(TFunction<void(B*)> in_function) override {
        for (auto& v : objects_) {
            in_function(static_cast<B*>(&v));
        }
    }

    template<CCallable<void, const WIdClass &, B*> TPredicate>
    void ForEachIdValue(TPredicate && in_predicate) {
        std::size_t i=0;
        for (auto& v : objects_) {
            std::forward<TPredicate>(in_predicate)(objects_.IndexAt(i++), v);
        }
    }

    void ForEachIdValue(TFunction<void(const WIdClass & _id, B* _value)> in_predicate) override {
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
