#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WCore/WConcepts.hpp"
#include "WLog.hpp"

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <memory>
#include <concepts>
#include <vector>

#ifndef WOBJECTMANAGER_INITIAL_MEMORY
#define WOBJECTMANAGER_INITIAL_MEMORY 1024
#endif

/**
 * @brief This class is a container for WObjects and derived types.
 */
template<CWObjectDerived WObjClass, typename WIdClass=WId>
class WENGINEOBJECTS_API WObjectDb {

public:

    using DbType = std::unordered_map<const WClass *,
                                      std::unique_ptr<IObjectDataBase<WObjClass, WIdClass>>>;

    template<typename ValueFn, typename IncrFn>
    using ClassIterator = TIterator<const WClass *,
                                    typename DbType::const_iterator,
                                    const WClass *,
                                    ValueFn,
                                    IncrFn
                                    >;

public:

    constexpr WObjectDb() :
    db_(),
    initial_memory_size_(WOBJECTMANAGER_INITIAL_MEMORY)
    {}
    
    virtual ~WObjectDb() = default;

    WObjectDb(const WObjectDb & other) :
    db_(),
    initial_memory_size_(other.initial_memory_size_) {
        CopyContainersFrom(other);
    }
    
    constexpr WObjectDb(WObjectDb && other) noexcept :
    db_(std::move(other.db_)),
    initial_memory_size_(std::move(other.initial_memory_size_))
    {}
    
    constexpr WObjectDb & operator=(const WObjectDb & other) {
        if (this != &other) {
            initial_memory_size_ = other.initial_memory_size_;
            CopyContainersFrom(other);
        }
        return *this;
    }

    constexpr WObjectDb & operator=(WObjectDb && other) noexcept {
        if (this != &other) {
            db_ = std::move(other.db_);
            initial_memory_size_ = std::move(other.initial_memory_size_);
        }
        return *this;
    }
    
public:

    /**
     * @brief Create a new object of type T and assign a WId.
     * Assigned WId is unique by class type.
     */
    template<std::derived_from<WObjClass> T>
    WIdClass Create() {
        return Create(T::StaticClass());
    }

    /**
     * @brief Create a new object of type WClass and assign a WId.
     * Assigned WId id unique by class type.
     */
    WIdClass Create(const WClass * in_class) {
        EnsureClassStorage(in_class);
        return db_[in_class]->Create();
    }

    /**
     * @brief Create a new object of type WClass at the specified in_id.
     * Asserts that the WId doesn't exists for the indicated class already (only in debug).
     * If your replace an existing WId for the indicated class behaviour is undeffined.
     */
    void CreateAt(const WClass * in_class,
                  const WIdClass& in_id) {
        EnsureClassStorage(in_class);

        assert(!db_[in_class]->Contains(in_id));

        db_[in_class]->CreateAt(in_id);
    }

    template<std::derived_from<WObjClass> T>
    void CreateAt(const WIdClass & in_id) {
        EnsureClassStorage(T::StaticClass());
        
        assert(!db_[T::StaticClass()]->Contains(in_id));

        db_[T::StaticClass()]->CreateAt(in_id);
    }

    WObjClass * Get(const WClass * in_class, const WIdClass & in_id) const {
        WObjClass * result;
        db_.at(in_class)->Get(in_id, result);

        return result;
    }

    template<std::derived_from<WObjClass> T>
    T * Get(WIdClass in_id) const {
        return static_cast<T*>(Get(T::StaticClass(), in_id));
    }

    WObjClass * GetFirst(const WClass * in_class, WIdClass & out_id) const {
        WObjClass * result;
        db_.at(in_class)->GetFirst(result, out_id);
        
        return result;
    }

    template<std::derived_from<WObjClass> T>
    T * GetFirst(WIdClass & out_id) const {
        return static_cast<T*>(GetFirst(T::StaticClass(), out_id));
    }

    void ForEachIdValue(const WClass * in_class,
                        const TFunction<void(const WIdClass &, WObjClass *)> & in_predicate) const {
        db_.at(in_class)->ForEachIdValue(in_predicate);
    }

    template<std::derived_from<WObjClass> T>
    void ForEachIdValue(const TFunction<void(const WIdClass &, T *)> & in_predicate) const {
        db_.at(T::StaticClass())->ForEachIdValue(
            [&in_predicate](const WIdClass & _wid, WObjClass * _ptr) {
                in_predicate(_wid, static_cast<T*>(_ptr));
            }
            );
    }

    void ForEach(const WClass * in_class, const TFunction<void(WObjClass*)> & in_predicate) const {
        assert(db_.contains(in_class));

        db_.at(in_class)->ForEach(in_predicate);
    }

    template<std::derived_from<WObjClass> T>
    void ForEach(const TFunction<void(T*)> & in_predicate) const {
        assert(db_.contains(T::StaticClass()));

        db_.at(T::StaticClass())->ForEach(
            [&in_predicate](WObjClass * _ptr) {
                in_predicate(static_cast<T*>(_ptr));
            } );
    }

    template<typename T>
    bool Contains(const WIdClass & in_id) const {
        return db_.contains(T::StaticClass()) &&
            db_.at(T::StaticClass())->Contains(in_id.GetId());
    }

    bool Contains(const WClass * in_class, const WIdClass & in_id) const {
        return db_.contains(in_class) && db_.at(in_class)->Contains(in_id.GetId());
    }

    bool ContainsClass(const WClass * in_class) const {
        return db_.contains(in_class);
    }

    /**
     * @brief Returns present (or that have been present) classes in this object.
     */
    auto IterWClasses() const {
        return ClassIterator(
            db_.cbegin(),
            db_.cend(),
            []( auto & _it, const std::int32_t & _i) -> const WClass * {
                return (*_it).first;
            },
            [](auto & _it, const std::int32_t & _i) {
                _it++;
                return _it;
            }
            );
    }

    /**
     * @brief Ammount of memory initially present for each stored class.
     */
    void InitialMemorySize(size_t in_ammount) {
        initial_memory_size_ = in_ammount;
    }

    std::vector<WIdClass> Indexes(const WClass * in_class) const {
        assert(db_.contains(in_class));
        return db_.at(in_class)->Indexes();
    }

    WNODISCARD size_t InitialMemorySize() const {
        return initial_memory_size_;
    }

    WNODISCARD size_t Count(const WClass * in_class) const {
        if (!db_.contains(in_class)) return 0;
        
        return db_.at(in_class)->Count();
    }

private:

    void CopyContainersFrom(const WObjectDb & other) {
        db_.clear();
        for (auto & p : other.db_) {
            db_.insert(
                { p.first, p.second->Clone() }
                );
        }
    }

    void EnsureClassStorage(const WClass * in_class) {
        if (!db_.contains(in_class)) {
            db_[in_class] =
                in_class->DbBuilder().Create<WObjClass, WIdClass>();

            db_[in_class]->Reserve(
                initial_memory_size_
                );
        }
    }

    DbType db_;

    size_t initial_memory_size_;

};

