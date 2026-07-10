#pragma once

#include "WCore/TEvent.hpp"
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
        std::unordered_map<WClass const *, std::unique_ptr<ObjectDbType>>;

    template<typename ValueFn, typename IncrFn>
    using ClassIterator = TIterator<const WClass *,
                                    typename DbType::const_iterator,
                                    WClass const *,
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

    WNODISCARD auto & OnAllocateEvent() {
        return on_allocate_events_;
    }

    WNODISCARD auto & OnDeallocateEvent() {
        return on_deallocate_events_;
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

    DbType db_{};

    std::size_t initial_memory_size_{1024};

    TEvent<void(WObjectDb *, WClass const *)> on_allocate_events_{};
    TEvent<void(WObjectDb *, WClass const *)> on_deallocate_events_{};

    struct StorageEvents {

        template<typename T>
        static inline void OnAllocateEventManager(
            void * prev_ptr, std::size_t prev_n,
            void * new_ptr, std::size_t new_n
            ) {

            WObjectDb * object_db = GetObjectDb(prev_ptr);

            if (!object_db) return;

            object_db->storage_events_.RegPtrReference(new_ptr);

            object_db->on_allocate_events_.Emit(
                object_db, T::StaticClass()
                );
        }

        template<typename T>
        static inline void OnDeallocateEventManager(
            void * ptr, std::size_t n
            ) {

            WObjectDb * object_db = GetObjectDb(ptr);

            if (!object_db) return;

            object_db->storage_events_.DeregPtrReference(ptr);

            object_db->on_deallocate_events_.Emit(
                object_db, T::StaticClass()
                );
        }

        static inline WObjectDb * GetObjectDb(void const * ref_ptr) {
            if (ptr_container.contains(ref_ptr)) {
                return ptr_container[ref_ptr];
            }
            return nullptr;
        }

        StorageEvents()=delete;

        StorageEvents(WObjectDb * in_object_db ) :
            db_ref_(in_object_db) {
            Initialize();
        }
    
        StorageEvents(const StorageEvents&) = delete;
        StorageEvents(StorageEvents&&) = delete;
        
        StorageEvents& operator=(const StorageEvents& other) {
            if (this != &other) {
                Clear();
                Initialize();
            }
            return *this;
        }
    
        StorageEvents& operator=(StorageEvents&& other) {
            if(this != &other) {
                Clear();
                Initialize();
            }
            return *this;
        }

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
            container_ptrs[db_ref_].insert(ptrref);
        }

        void DeregPtrReference(void const * ptr_ref ) {
            if(ptr_container.contains(ptr_ref)) {
                ptr_container.erase(ptr_ref);
            }

            if (container_ptrs[db_ref_].contains(ptr_ref)) {
                container_ptrs[db_ref_].erase(ptr_ref);
            }

        }

        constexpr void Initialize() {
            if (!container_ptrs.contains(db_ref_)) {
                container_ptrs[db_ref_] = {};
            }
            
            for(auto it : db_ref_->IterWClasses()) {
                RegPtrReference(db_ref_->db_[it]->BData());
            }
        }

        void Clear() {
            if(container_ptrs.contains(db_ref_)) {
                for (auto ptr : container_ptrs[db_ref_]) {
                    ptr_container.erase(ptr);
                }
                container_ptrs.erase(db_ref_);
            }            
        }

    private:

        static inline std::unordered_map<
            void const *,
            WObjectDb * > ptr_container;
    
        static inline std::unordered_map<WObjectDb*,
                                         std::unordered_set<void const *>> container_ptrs;

        WObjectDb * const db_ref_;
    
    } storage_events_{this};

    template<std::derived_from<WObjClass> T>
    void EnsureClassStorage() {
        if (!db_.contains(T::StaticClass())) {
            db_[T::StaticClass()] =
                T::StaticClass()->DbBuilder()
                . template Create <WObjClass, typename WIdType::IdType>();

            db_[T::StaticClass()]->Reserve(
                initial_memory_size_
                );

            storage_events_.RegPtrReference(
                db_[T::StaticClass()]->BData() 
                );

            WDbBuilder::RegisterOnAllocateEvent<T>(
                &StorageEvents:: template OnAllocateEventManager<T>
                );
            
            WDbBuilder::RegisterOnDeallocateEvent<T>(
                &StorageEvents:: template OnDeallocateEventManager<T>);
        }
    }
};

