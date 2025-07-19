#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WEngineObjects/TWRef.hpp"

#include <memory>
#include <unordered_map>
#include <memory>
#include <concepts>
#include <vector>

#ifndef WOBJECTMANAGER_INITIAL_MEMORY
#define WOBJECTMANAGER_INITIAL_MEMORY 1024
#endif

/**
 * @brief This class is a container for all WObjectContainer.
 * the manager will be the responsible of creation and storage of all WObject types.
 * Also will assign an unique id to each WObject.
*/
class WENGINEOBJECTS_API WObjectManager {

public:

    constexpr WObjectManager() :
        containers_(),
        initial_memory_size_(WOBJECTMANAGER_INITIAL_MEMORY)
        {}
    
    virtual ~WObjectManager() = default;

    WObjectManager(const WObjectManager&) = delete;
    
    constexpr WObjectManager(WObjectManager && other) noexcept :
        containers_(std::move(other.containers_)),
        initial_memory_size_(std::move(other.initial_memory_size_))
        {}
    
    constexpr WObjectManager & operator=(const WObjectManager&) = delete;

    constexpr WObjectManager & operator=(WObjectManager && other) noexcept {
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
    template<std::derived_from<WObject> T>
    WId Create(const char * in_fullname) {
        return Create(T::StaticClass(),
                      in_fullname);
    }

    /**
     * @brief Create a new object of type T at the specified in_id.
     * Asserts that the WId doesn't exists for the indicated class already (only in debug).
     * If you replace an existing WId for the indicated class behaviour is undeffined.
     */
    template<std::derived_from<WObject> T>
    WId Create(const WId in_id,
               const char * in_fullname) {
        return Create(T::StaticClass(),
                      in_id,
                      in_fullname);
    }

    /**
     * @brief Create a new object of type WClass and assign a WId.
     * Assigned WId id unique by class type.
     */
    WId Create(const WClass * in_class,
               const char * in_fullname);

    /**
     * @brief Create a new object of type WClass at the specified in_id.
     * Asserts that the WId doesn't exists for the indicated class already (only in debug).
     * If your replace an existing WId for the indicated class behaviour is undeffined.
     */
    WId Create(const WClass * in_class,
                          const WId& in_id,
                          const char * in_fullname);

    template <std::derived_from<WObject> T>
    TWRef<T> Get(WId in_id) const {
        return static_cast<T*>(Get(T::StaticClass(), in_id).Ptr());
    }

    TWRef<WObject> Get(const WClass * in_class, const WId & in_id) const;

    template<std::derived_from<WObject> T>
    void ForEach(TFunction<void(T*)> in_predicate) const {
        assert(containers_.contains(T::StaticClass()));

        containers_.at(T::StaticClass())->ForEach(
            [&in_predicate](WObject* ptr_) {
                in_predicate(
                    static_cast<T*>(ptr_)
                    );
            }
            );
    }

    void ForEach(const WClass * in_class, TFunction<void(WObject*)> in_predicate) const;

    // TODO: ForEachWId could be a faster iterator?
    //  useful for objects already in graphics memory.

    bool Contains(const WClass * in_class, WId in_id) const;

    bool Contains(const WClass * in_class) const;

    /**
     * @brief Returns present (or that have been present) classes in this object.
     */
    std::vector<const WClass *> Classes() const;

    /**
     * @brief Ammount of memory initially present for each stored class.
     */
    void InitialMemorySize(size_t in_ammount);

    std::vector<WId> Indexes(const WClass * in_class) const;

    WNODISCARD size_t InitialMemorySize() const;

    WNODISCARD size_t Size(const WClass * in_class) const;

private:

    void EnsureClassStorage(const WClass * in_class);

    std::unordered_map<const WClass *, std::unique_ptr<IObjectDataBase<WObject>>> containers_;

    size_t initial_memory_size_;

};

