#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WEngineObjects/TWRef.hpp"

#include <algorithm>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <memory>
#include <concepts>
#include <vector>

#ifndef WOBJECTMANAGER_INITIAL_MEMORY
#define WOBJECTMANAGER_INITIAL_MEMORY 1024
#endif

template<typename T>
concept has_wid = requires(T a) { a.WID(); };

template<typename T>
concept wobject_derived = std::is_base_of_v<WObject, T> && has_wid<T>;


/**
 * @brief This class is a container for all WObjectContainer.
 * the manager will be the responsible of creation and storage of all WObject types.
 * Also will assign an unique id to each WObject.
 */
template<wobject_derived WObjClass, typename WIdClass=WId>
class WENGINEOBJECTS_API WObjectDb {

public:

    using DbType =
        std::unordered_map<const WClass *,
                           std::unique_ptr<IObjectDataBase<WObjClass, WId>>>;

    using ClassIterator = TIterator<const WClass *,
        typename DbType::const_iterator,
        const WClass * const,
        const WClass * const>;

public:

    constexpr WObjectDb() :
        db_(),
        initial_memory_size_(WOBJECTMANAGER_INITIAL_MEMORY)
        {}
    
    virtual ~WObjectDb() = default;

    WObjectDb(const WObjectDb& other) :
    db_(),
    initial_memory_size_(other.initial_memory_size_) {
        CopyContainersFrom(other);
    }
    
    constexpr WObjectDb(WObjectDb && other) noexcept :
    db_(std::move(other.db_)),
    initial_memory_size_(std::move(other.initial_memory_size_))
    {}
    
    constexpr WObjectDb & operator=(const WObjectDb& other) {
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
    WIdClass Create(const char * in_fullname) {
        return Create(T::StaticClass());
    }

    /**
     * @brief Create a new object of type WClass and assign a WId.
     * Assigned WId id unique by class type.
     */
    WIdClass Create(const WClass * in_class) {
        EnsureClassStorage(in_class);
    
        WId id = db_[in_class]->Create();

        return id.GetId();
        
        // WObject * obj;
        // db_[in_class]->Get(id, obj);

        // obj->WID(id);
        // obj->Name(in_fullname);

        // return id;
    }

    /**
     * @brief Create a new object of type WClass at the specified in_id.
     * Asserts that the WId doesn't exists for the indicated class already (only in debug).
     * If your replace an existing WId for the indicated class behaviour is undeffined.
     */
    WIdClass Insert(const WClass * in_class,
                    const WIdClass& in_id,
                    const char * in_fullname) {
        EnsureClassStorage(in_class);

        assert(!containers_[in_class]->Contains(in_id));

        db_[in_class]->Insert(in_id.GetId());

        return in_id;

        // WObject * obj;
        // db_[in_class]->Get(in_id, obj);

        // obj->WID(in_id);
        // obj->Name(in_fullname);

    }

    /**
     * @brief Create a new object of type T at the specified in_id.
     * Asserts that the WId doesn't exists for the indicated class already (only in debug).
     * If you replace an existing WId for the indicated class behaviour is undeffined.
     */
    template<std::derived_from<WObjClass> T>
    WIdClass Insert(const WIdClass in_id,
                    const char * in_fullname) {
        return Insert(T::StaticClass(),
                      in_id,
                      in_fullname);
    }

    template <std::derived_from<WObjClass> T>
    TWRef<T> GetRef(WId in_id) const {
        return GetRaw<T>(in_id);
    }

    WObjClass * Get(const WClass * in_class, const WIdClass & in_id) const {
        WObjClass * result;
        db_.at(in_class)->Get(in_id.GetId(), result);

        return result;
    }

    template<std::derived_from<WObjClass> T>
    T * Get(WIdClass in_id) const {
        return static_cast<T*>(Get(T::StaticClass(), in_id));
    }

    template<std::derived_from<WObjClass> T>
    void ForEach(TFunction<void(T*)> in_predicate) const {
        assert(containers_.contains(T::StaticClass()));

        ForEach(T::StaticClass(), in_predicate);

        db_.at(T::StaticClass())->ForEach(
            [&in_predicate](WObject* ptr_) -> void {
                in_predicate(static_cast<T*>(ptr_));
            });
    }

    void ForEach(const WClass * in_class, TFunction<void(WObjClass*)> in_predicate) const {
        assert(containers_.contains(in_class));
    
        db_.at(in_class)->ForEach(
            [&in_predicate](WObject* ptr_) -> void {
                in_predicate(static_cast<WObjClass*>(ptr_));
            });
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
    ClassIterator Classes() const {
        return ClassIterator(
            db_.cbegin(),
            db_.cend(),
            [](ClassIterator::IterType & _iter, const size_t & _n) -> const WClass *const {
                return (*_iter).first;
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
        assert(containers_.contains(in_class));
        std::vector<WIdClass> r;
        r.reserve(db_.at(in_class)->Count());
        for (auto & idx : db_.at(in_class)->Indexed()) {
            r.push_back(idx.GetId());
        }
        return r;
    }

    WNODISCARD size_t InitialMemorySize() const {
        return initial_memory_size_;
    }

    WNODISCARD size_t Count(const WClass * in_class) const {
        assert(containers_.contains(in_class));
        return db_.at(in_class)->Count();
    }

private:

    void CopyContainersFrom(const WObjectDb & other) {
        db_.clear();
        for (auto & p : other.db_) {
            db_.insert(
                {p.first, p.second->Clone()}
                );
        }
    }

    void EnsureClassStorage(const WClass * in_class) {
        if (!db_.contains(in_class)) {
            db_[in_class] =
                in_class->CreateObjectDatabase();
            
            db_[in_class]->Reserve(
                initial_memory_size_
                );
        }
    }

    DbType db_;

    size_t initial_memory_size_;

};

