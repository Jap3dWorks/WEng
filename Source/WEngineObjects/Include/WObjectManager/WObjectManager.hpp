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
class WObjectManager {

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
     * @brief Create a new WObject of type T.
     */
    template<std::derived_from<WObject> T>
    TWRef<T> CreateObject(const char * in_fullname) {
        
        const WClass * w_class = T::StaticClass();
        return static_cast<T*>(
            CreateObject(w_class, in_fullname).Ptr()
            );
    }

    TWRef<WObject> CreateObject(const WClass * in_class, const char * in_fullname);

    template <std::derived_from<WObject> T>
    TWRef<T> GetObject(WId in_id) {
        WClass * object_class = T::StaticClass();

        return static_cast<T*>(GetObject(object_class, in_id).Ptr());
    }

    TWRef<WObject> GetObject(const WClass * in_class, WId in_id);

    template<std::derived_from<WObject> T>
    void ForEach(TFunction<void(T*)> in_predicate) {
        assert(containers_.contains(T::StaticClass()));

        containers_[T::StaticClass()]->ForEach(
            [&in_predicate](WObject* ptr_) {
                in_predicate(
                    static_cast<T*>(ptr_)
                    );
            }
            );
    }

    void ForEach(const WClass * in_class, TFunction<void(WObject*)> in_predicate);

    /**
     * Returns the classes that are (or have been) present in this object.
     */
    std::vector<const WClass *> Classes() const;

    void InitialMemorySize(size_t in_ammount);

    WNODISCARD size_t InitialMemorySize() const;

    WNODISCARD size_t Size(const WClass * in_class) const;

private:

    std::unordered_map<const WClass *, std::unique_ptr<IObjectDataBase<WObject>>> containers_;

    size_t initial_memory_size_;

};

