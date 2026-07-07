#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TFunction.hpp"
#include "TSparseSet.hpp"
#include "WCore/WConcepts.hpp"

#include <memory>
#include <concepts>
#include <utility>

template<typename B=void, std::integral IdBase=std::uint64_t>
class IObjectDataBase {
public:
    virtual ~IObjectDataBase()=default;

    virtual std::unique_ptr<IObjectDataBase> Clone() const=0;
    // virtual 
    /** Create and assign an WIdClass */
    virtual IdBase Create() = 0;
    /** Create at WIdClass */
    virtual void CreateAt(const IdBase &)=0;
    /** Insert At WIdClass */
    virtual void InsertAt(const IdBase &, B* &)=0;
    virtual void Remove(const IdBase &) =0;
    virtual void Clear() = 0;
    virtual void Get(const IdBase &, B* &) = 0;
    virtual void Get(const IdBase &, const B* &) const = 0;

    virtual void BGetFirst(B* &, IdBase &) = 0;
    virtual void BGetFirst(const B*&, IdBase &) const =0;
    
    virtual size_t Count() const = 0;
    virtual bool Contains(const IdBase & in_id) const = 0;
    virtual void Reserve(size_t in_value) = 0;
    virtual std::vector<IdBase> Indexes() = 0;

    // TODO: TFunction contains 1 virtual call. function ptr should be more efficient.
    virtual void BForEach(TFunction<void(B*)> in_function)=0;
    virtual void BForEachIdValue(TFunction<void(const IdBase &, B *)>)=0;
    
};

/**
 * @brief Container for objects of type T.
 * Objects are aligned continously in memory.
 * Object in the containe can be accesed by WId.
 * You can specify a create_fn to create objects inside the container.
 * And a destroy_fn called when remove objects in the container.
 */
template<typename T,
         typename CreateFn,
         typename DestroyFn,
         CConvertibleTo<T> B=void,
         std::integral IdBase=std::uint64_t,
         typename Allocator=std::allocator<T>>
class TObjDb : public IObjectDataBase<B, IdBase> {
public:

    using Super = IObjectDataBase<B, IdBase>;
    using ObjectsType = TSparseSet<T, Allocator>;

    template<typename IndexIterator, typename ValueFn, typename IncrFn>
    using ConstWIdIterator = TIterator<IdBase,
                                       IndexIterator,
                                       IdBase,
                                       ValueFn,
                                       IncrFn>;

public:

    constexpr TObjDb() noexcept :
        create_fn_(),
        destroy_fn_(),
        id_pool_(),
        objects_()
        {}

    constexpr TObjDb(
        const Allocator & in_allocator
        ) :
        create_fn_(),
        destroy_fn_(),
        id_pool_(),
        objects_(in_allocator)
        {}

    constexpr TObjDb(
        const CreateFn & in_create_fn,
        const DestroyFn & in_destroy_fn
        ) :
        create_fn_(in_create_fn),
        destroy_fn_(in_destroy_fn),
        id_pool_(),
        objects_()
        {}

    constexpr TObjDb(
        const CreateFn & in_create_fn,
        const DestroyFn & in_destroy_fn,
        const Allocator & in_allocator
        ) :
        create_fn_(in_create_fn),
        destroy_fn_(in_destroy_fn),
        id_pool_(),
        objects_(in_allocator)
        {}

    virtual ~TObjDb() {
        Clear();
    }

    TObjDb(const TObjDb & other) :
        create_fn_(other.create_fn_),
        destroy_fn_(other.destroy_fn_),
        id_pool_(other.id_pool_),
        objects_(other.objects_)
        {}

    constexpr TObjDb(TObjDb && other) noexcept :
        create_fn_(std::move(other.create_fn_)),
        destroy_fn_(std::move(other.destroy_fn_)),
        id_pool_(std::move(other.id_pool_)),
        objects_(std::move(other.objects_))
        {}

    TObjDb & operator=(const TObjDb & other) {
        if (this != &other) {
            Clear();
            create_fn_ = other.create_fn_;
            destroy_fn_ = other.destroy_fn_;
            objects_ = other.objects_;
            id_pool_ = other.id_pool_;
        }

        return *this;
    }

    TObjDb & operator=(TObjDb && other) noexcept {
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
        return std::make_unique<TObjDb>(*this);
    }

    template<CCallable<T, const IdBase &> TCreateFn>
    IdBase Create(TCreateFn && in_create_fn) {
        IdBase oid = id_pool_.Generate();
        objects_.Insert(oid, std::forward<TCreateFn>(in_create_fn) (oid));

        return oid;
    }

    /**
     * @brief Create an object in the container and return the assigned id.
     * create_fn is used to create the object.
     */
    IdBase Create() override {
        IdBase oid = id_pool_.Generate();
        objects_.Insert(oid, create_fn_(oid));

        return oid;
    }

    void CreateAt(const IdBase & in_id) override {
        id_pool_.Release(in_id);
        objects_.Insert(in_id, create_fn_(in_id));
    }

    template<CCallable<T, const IdBase &> TCreateFn>
    void CreateAt(const IdBase & in_id, TCreateFn && in_create_fn) {
        id_pool_.Release(in_id);
        objects_.Insert(in_id, std::forward<TCreateFn>(in_create_fn)(in_id));
    }

    template<typename D> requires std::is_same_v<std::remove_cvref_t<D>, T>
    IdBase Insert(T && in_value) {
        IdBase oid = id_pool_.Generate();
        objects_.Insert(oid, std::forward(in_value));

        return oid;
    }

    template<typename D> requires std::is_same_v<std::remove_cvref_t<D>, T>
    void InsertAt(const IdBase & in_id, D && in_value) {
        id_pool_.Release(in_id);
        objects_.Insert(in_id, std::forward<D>(in_value));
    }

    void InsertAt(const IdBase & in_id, B* & in_value) override {
        id_pool_.Release(in_id);
        objects_.Insert(in_id, *static_cast<T*>(in_value));
    }

    template<CCallable<void, T&> TDestroyFn>
    void Remove(const IdBase & in_id, TDestroyFn && in_destroy_fn) {
        std::forward<TDestroyFn>(in_destroy_fn)(objects_.Get(in_id));
        id_pool_.Release(in_id);
        objects_.Remove(in_id);
    }

    void Remove(const IdBase & in_id) override final {
        destroy_fn_(objects_.Get(in_id));
        id_pool_.Release(in_id);
        objects_.Remove(in_id);
    }

    template<CCallable<void, T&> TFn>
    void Clear(TFn && in_destroy_fn) {
        for (auto & o : objects_) {
            std::forward<TFn>(in_destroy_fn)(o);
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

    T & Get(const IdBase & in_id) {
        return objects_.Get(in_id);
    }

    const T & Get(const IdBase & in_id) const {
        return objects_.Get(in_id);
    }

    void Get(const IdBase & in_id, B* & out_value) override final {
        out_value = &objects_.Get(in_id);
    }

    void Get(const IdBase & in_id, const B* & out_value) const override final {
        out_value = &objects_.Get(in_id);
    }

    T & GetFirst(IdBase & out_id) {
        out_id = objects_.DensePosition(0);
        return objects_.Get(out_id);
    }

    T & GetFirst(IdBase & out_id) const {
        out_id = objects_.DensePosition(0);
        return objects_.Get(out_id);
    }

    void BGetFirst(B* & out_first, IdBase & out_id) override {
        out_id = objects_.DensePosition(0);
        out_first = &(objects_.Get(out_id));
    }
    
    void BGetFirst(const B*& out_first, IdBase & out_id) const override {
        out_id = objects_.DensePosition(0);
        out_first = &(objects_.Get(out_id));
    }

    size_t Count() const override final {
        return objects_.Count();
    }

    bool Contains(const IdBase & in_id) const override final {
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

    auto IterIndexes() const {
        return ConstWIdIterator(
            objects_.IterIndexes().begin(),
            objects_.IterIndexes().end(),
            [](auto & _it, const std::int32_t & _id) -> IdBase {
                return IdBase(*_it);
            },
            [](auto & _it, const std::int32_t & _id) {
                _it++;
                return _it;
            }
            );
    }

    std::vector<IdBase> Indexes() override {
        std::vector<IdBase> result;
        result.reserve(objects_.Count());
        
        for (IdBase d : IterIndexes()) {
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

    // TODO: const versions

    template<CCallable<void, T&> TFn>
    void ForEach(TFn && in_fn) {
        for (auto & v : objects_) {
            in_fn(v);
        }
    }

    void BForEach(TFunction<void(B*)> in_function) override {
        for (auto& v : objects_) {
            in_function(static_cast<B*>(&v));
        }
    }

    template<CCallable<void, const IdBase &, T&> TFn>
    void ForEachIdValue(TFn && in_fn) {
        std::size_t i=0;
        for (auto& v : objects_) {
            in_fn(objects_.IndexAt(i++), v);
        }
    }

    void BForEachIdValue(TFunction<void(const IdBase & _id, B* _value)> in_predicate) override {
        std::size_t i=0;
        for (auto& v : objects_) {
            in_predicate(objects_.IndexAt(i++),
                         static_cast<B*>(&v));
        }
    }

private:

    CreateFn create_fn_;
    DestroyFn destroy_fn_; 

    WIdPool<IdBase> id_pool_;

    ObjectsType objects_;
    
};

template<typename T,
         CConvertibleTo<T> B=void,
         std::integral IdBase=std::uint64_t,
         typename Allocator=std::allocator<T>>
using TObjectDataBase = TObjDb<T,
                               TFunction<T(IdBase const &)>,
                               TFunction<void(T&)>,
                               B,
                               IdBase,
                               Allocator>;



