#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WEngineObjects/TWRef.hpp"

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
concept wobject_derived = std::is_base_of_v<WObject, T> && requires(T a) {
    a.WID();
};


/**
 * @brief This class is a container for all WObjectContainer.
 * the manager will be the responsible of creation and storage of all WObject types.
 * Also will assign an unique id to each WObject.
 */
template<wobject_derived WObjClass, typename WIdClass=WId>
class WENGINEOBJECTS_API WObjectDb {

public:

    using ContainersType = std::unordered_map<const WClass *,
        std::unique_ptr<IObjectDataBase<WObjClass>>>;

    using ClassIterator = TIterator<const WClass *,
        typename ContainersType::const_iterator,
        const WClass *const,
        const WClass *const>;

public:

    constexpr WObjectDb() :
        containers_(),
        initial_memory_size_(WOBJECTMANAGER_INITIAL_MEMORY)
        {}
    
    virtual ~WObjectDb() = default;

    WObjectDb(const WObjectDb& other) :
    containers_(),
    initial_memory_size_(other.initial_memory_size_) {
        CopyContainersFrom(other);
    }
    
    constexpr WObjectDb(WObjectDb && other) noexcept :
    containers_(std::move(other.containers_)),
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
            containers_ = std::move(other.containers_);
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
        return Create(T::StaticClass(),
                      in_fullname);
    }

    /**
     * @brief Create a new object of type T at the specified in_id.
     * Asserts that the WId doesn't exists for the indicated class already (only in debug).
     * If you replace an existing WId for the indicated class behaviour is undeffined.
     */
    template<std::derived_from<WObjClass> T>
    WIdClass Create(const WId in_id,
                    const char * in_fullname) {
        return Create(T::StaticClass(),
                      in_id,
                      in_fullname);
    }

    /**
     * @brief Create a new object of type WClass and assign a WId.
     * Assigned WId id unique by class type.
     */
    WIdClass Create(const WClass * in_class,
                    const char * in_fullname) {
        EnsureClassStorage(in_class);
    
        WIdClass id = containers_[in_class]->Create();
        
        WObject * obj;
        containers_[in_class]->Get(id, obj);

        obj->WID(id);
        obj->Name(in_fullname);

        return id;
    }

    /**
     * @brief Create a new object of type WClass at the specified in_id.
     * Asserts that the WId doesn't exists for the indicated class already (only in debug).
     * If your replace an existing WId for the indicated class behaviour is undeffined.
     */
    WIdClass Create(const WClass * in_class,
                    const WIdClass& in_id,
                    const char * in_fullname);

    template <std::derived_from<WObjClass> T>
    TWRef<T> GetRef(WId in_id) const {
        return GetRaw<T>(in_id);
    }

    WObjClass * Get(const WClass * in_class, const WId & in_id) const {
        WObjClass * result;
        containers_.at(in_class)->Get(in_id, result);

        return result;
    }

    template<std::derived_from<WObjClass> T>
    T * Get(WIdClass in_id) const {
        return static_cast<T*>(Get(T::StaticClass(), in_id));
    }

    template<std::derived_from<WObjClass> T>
    void ForEach(TFunction<void(T*)> in_predicate) const {
        assert(containers_.contains(T::StaticClass()));

        containers_.at(T::StaticClass())->ForEach(
            [&in_predicate](WObjClass* ptr_) {
                in_predicate(
                    static_cast<T*>(ptr_)
                    );
            }
            );
    }

    void ForEach(const WClass * in_class, TFunction<void(WObjClass*)> in_predicate) const {
        assert(containers_.contains(in_class));
    
        containers_.at(in_class)->ForEach(in_predicate);
    }

    // TODO: ForEachWId could be a faster iterator?
    //  useful for objects already in graphics memory.

    bool Contains(const WClass * in_class, WIdClass in_id) const;

    bool Contains(const WClass * in_class) const;

    /**
     * @brief Returns present (or that have been present) classes in this object.
     */
    ClassIterator Classes() const;

    /**
     * @brief Ammount of memory initially present for each stored class.
     */
    void InitialMemorySize(size_t in_ammount);

    std::vector<WId> Indexes(const WClass * in_class) const;

    WNODISCARD size_t InitialMemorySize() const;

    WNODISCARD size_t Count(const WClass * in_class) const;

private:

    void CopyContainersFrom(const WObjectDb & other) {
        containers_.clear();
        for (auto & p : other.containers_) {
            containers_.insert(
                {p.first, p.second->Clone()}
                );
        }
    }

    void EnsureClassStorage(const WClass * in_class);

    ContainersType containers_;

    size_t initial_memory_size_;

};

