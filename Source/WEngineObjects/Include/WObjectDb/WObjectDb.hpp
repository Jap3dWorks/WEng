#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WCore/WConcepts.hpp"
#include "WLog.hpp"
#include "WObjectDb/WDbBuilder.hpp"

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <memory>
#include <concepts>
#include <vector>

/**
 * @brief This class is a container for WObjects and derived types.
 */
template<CWObjectDerived WObjClass, CIsWId WIdType=WId<>>
class WENGINEOBJECTS_API WObjectDb {

public:

    using ObjectDbType = IObjectDataBase<WObjClass, typename WIdType::IdType>;

    using DbType =
        std::unordered_map<const WClass *, std::unique_ptr<ObjectDbType>>;

    template<typename ValueFn, typename IncrFn>
    using ClassIterator = TIterator<const WClass *,
                                    typename DbType::const_iterator,
                                    const WClass *,
                                    ValueFn,
                                    IncrFn>;

public:

    constexpr WObjectDb() = default;
    
    virtual ~WObjectDb() = default;

    WObjectDb(WObjectDb const & other) :
    db_(),
    initial_memory_size_(other.initial_memory_size_) {
        CopyContainersFrom(other);
    }
    
    constexpr WObjectDb(WObjectDb && other) noexcept = default;
    
    constexpr WObjectDb & operator=(WObjectDb const & other) {
        if (this != &other) {
            initial_memory_size_ = other.initial_memory_size_;
            CopyContainersFrom(other);
        }
        return *this;
    }

    constexpr WObjectDb & operator=(WObjectDb && other) noexcept = default;
    
public:

    /**
     * @brief Create a new object of type WClass at the specified in_id.
     * Asserts that the WId doesn't exists for the indicated class already (only in debug).
     * If your replace an existing WId for the indicated class behaviour is undefined.
     */
    template<std::derived_from<WObjClass> T>
    void CreateAt(const WIdType & in_id) {
        EnsureClassStorage<T>();
        
        assert(!db_[T::StaticClass()]->Contains(in_id));

        db_[T::StaticClass()]->CreateAt(in_id);
    }

    WObjClass * Get(const WClass * in_class, const WIdType & in_id) const {
        WObjClass * result;
        db_.at(in_class)->Get(in_id, result);

        return result;
    }

    /**
     * @brief get element at in_id.
     */
    template<std::derived_from<WObjClass> T>
    T & Get(WIdType in_id) const {
        return T::StaticClass()->DbBuilder()
            .template DbCast<T,WObjClass, typename WIdType::IdType>(
                db_.at(T::StaticClass()).get()
                )->Get(in_id);
    }

    WObjClass * GetFirst(const WClass * in_class, WIdType & out_id) const {
        assert(db_.contains(in_class));

        auto id_value = out_id.GetId();

        WObjClass * result;
        db_.at(in_class)->BGetFirst(result, id_value);

        out_id = id_value;
        
        return result;
    }

    template<std::derived_from<WObjClass> T>
    T & GetFirst(WIdType & out_id) const {
        typename WIdType::IdType idval;

        auto & result = T::StaticClass()->DbBuilder()
            .template DbCast<T, WObjClass, typename WIdType::IdType>(
                db_.at(T::StaticClass()).get()
                )->GetFirst(idval);

        out_id = idval;
        return result;
    }

    /**
     * @brief More flexible but less performant.
     */
    template<CCallable<void, const WIdType &, WObjClass *> TFn>
    void ForEachIdValue(const WClass * in_class, TFn && in_fn) const {
        db_.at(in_class)->BForEachIdValue(std::forward<TFn>(in_fn));
    }

    /**
     * @brief Less flexible bur more performant for each.
     */
    template<std::derived_from<WObjClass> T, CCallable<void, const WIdType &, T&> TFn>
    void ForEachIdValue(TFn && in_fn) const {
        T::StaticClass()->DbBuilder()
            .template DbCast<T,WObjClass,WIdType>(
                db_.at(T::StaticClass()).get()
                )->ForEachIdValue(std::forward<TFn>(in_fn));
    }

    /**
     * @brief More flexible but less performant.
     */
    template<CCallable<void, WObjClass*> TFn>
    void ForEach(const WClass * in_class, TFn && in_fn) const {
        assert(db_.contains(in_class));
        db_.at(in_class)->BForEach(std::forward<TFn>(in_fn));
    }

    /**
     * @brief Less flexible bur more performant for each.
     */
    template<std::derived_from<WObjClass> T, CCallable<void, T&> TFn>
    void ForEach(TFn && in_fn) const {
        assert(db_.contains(T::StaticClass()));

        T::StaticClass()->DbBuilder()
            .template DbCast<T,WObjClass, typename WIdType::IdType>(
                db_.at(T::StaticClass()).get()
                )->ForEach(std::forward<TFn>(in_fn));
    }

    template<typename T>
    bool Contains(const WIdType & in_id) const {
        return db_.contains(T::StaticClass()) &&
            db_.at(T::StaticClass())->Contains(in_id.GetId());
    }

    bool Contains(const WClass * in_class, const WIdType & in_id) const {
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
    void InitialMemorySize(std::size_t in_ammount) {
        initial_memory_size_ = in_ammount;
    }

    std::vector<typename WIdType::IdType> Indexes(const WClass * in_class) const {
        assert(db_.contains(in_class));
        return db_.at(in_class)->Indexes();
    }

    WNODISCARD std::size_t InitialMemorySize() const {
        return initial_memory_size_;
    }

    WNODISCARD std::size_t Count(const WClass * in_class) const {
        if (!db_.contains(in_class)) return 0;
        
        return db_.at(in_class)->Count();
    }

private:

    void CopyContainersFrom(WObjectDb const & other) {
        db_.clear();
        for (auto & p : other.db_) {
            db_.insert(
                { p.first, p.second->Clone() }
                );
        }
    }

    template<typename T>
    static inline void OnAllocateEventManager(
        void * prev_ptr, std::size_t prev_n,
        void * new_ptr, std::size_t new_n
        ) {
        
    }

    template<typename T>
    static inline void OnDeallocateEventManager(
        void * ptr, std::size_t n
        ) {
        
    }

    template<std::derived_from<WObjClass> T>
    void EnsureClassStorage() {
        if (!db_.contains(T::StaticClass())) {
            db_[T::StaticClass()] =
                T::StaticClass()->DbBuilder()
                . template Create <WObjClass, typename WIdType::IdType>();

            db_[T::StaticClass()]->Reserve(
                initial_memory_size_
                );

            WDbBuilder::RegisterOnAllocateEvent<T>(&OnAllocateEventManager<T>);
            WDbBuilder::RegisterOnDeallocateEvent<T>(&OnDeallocateEventManager<T>);

            // db_[T::StaticClass()]->
            db_[T::StaticClass()]->BData();
        }
    }

    DbType db_{};

    std::size_t initial_memory_size_{1024};
};

template<CWObjectDerived WObjClass, CIsWId WIdType=WId<>>
struct StorageEvents {

    using WObjectDb = WObjectDb<WObjClass,WIdType>;

    static inline std::unordered_map<
        void const *,
        WObjectDb * >
    ptr_container;
    
    static inline std::unordered_map<WObjectDb*, std::vector<void const *>> container_ptrs;


    StorageEvents()=delete;

    StorageEvents(WObjectDb * in_object_db ) :
        db_ref_(in_object_db) {}
    
    StorageEvents(const StorageEvents&) {
        // db_ref_->
    };
    
    StorageEvents(StorageEvents&&) = default;
    StorageEvents& operator=(const StorageEvents&) = default;
    StorageEvents& operator=(StorageEvents&&) = default;

    ~StorageEvents() {
        if(db_ref_ && container_ptrs.contains(db_ref_)) {
            for (auto ptr : container_ptrs[db_ref_]) {
                ptr_container.erase(ptr);
            }
            container_ptrs.erase(db_ref_);
        }
    }

    void RegPtrReference(void const * ptrref) {
        ptr_container[ptrref]= db_ref_;
        container_ptrs[db_ref_].push_back(ptrref);
    }

    void DeregPtrReference(void const * ptr_ref ) {
        if(ptr_container.contains(ptr_ref)) {
            ptr_container.erase(ptr_ref);
        }

        auto it = std::find(container_ptrs[db_ref_].begin(),
                            container_ptrs[db_ref_].end(),
                            ptr_ref);
        
        if (it != container_ptrs[db_ref_].end()) {
            *it = container_ptrs[db_ref_].back();
            
            container_ptrs[db_ref_].resize(
                container_ptrs[db_ref_].size() - 1
                );
        }
    }

private:

    WObjectDb * db_ref_{nullptr};
    
};
