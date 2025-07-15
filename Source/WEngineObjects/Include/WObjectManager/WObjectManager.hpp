#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WEngineObjects/TWRef.hpp"

#include <memory>
#include <unordered_map>
#include <memory>
#include <concepts>

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

    constexpr WObjectManager() noexcept = default;
    virtual ~WObjectManager() = default;

    // You can create a global instance of WObjectManager

    WObjectManager(const WObjectManager&) = delete;
    
    WObjectManager(WObjectManager && other) noexcept :
        containers_(std::move(other.containers_))
        {}
    
    constexpr WObjectManager & operator=(const WObjectManager&) = delete;

    constexpr WObjectManager & operator=(WObjectManager && other) noexcept {
        Move(std::move(other));
        return *this;
    }
    
public:

    /**
     * @brief Create a new WObject of type T.
     */
    template<std::derived_from<WObject> T>
    TWRef<T> CreateObject(const char* in_object_path) {
        
        WClass * object_class = T::GetStaticClass();

        WId id = CreateObject(object_class, in_object_path);

        void* result;

        containers_[object_class]->Get(id, result);

        T* asset = reinterpret_cast<T*>(result);
        asset->WID(id);

        return asset;
    }

    WId CreateObject(WClass * in_class, const char * in_object) {
        
        if (!containers_.contains(in_class)) {
            containers_[in_class] =
                in_class->CreateObjectDatabase();
            
            containers_[in_class]->Reserve(
                WOBJECTMANAGER_INITIAL_MEMORY
                );
        }

        WId id = containers_[in_class]->Create();

        return id;
    }

    template <std::derived_from<WObject> T>
    TWRef<T> GetObject(WId in_id) {
        WClass * object_class = T::GetDefaultObject()->GetClass();

        void * result;
        containers_[object_class]->Get(in_id, result);

        return reinterpret_cast<T*>(result);
    }

private:

    constexpr void Move(WObjectManager && other) noexcept {
        containers_ = std::move(other.containers_);
    }

private:    

    std::unordered_map<WClass *, std::unique_ptr<IObjectDataBase>> containers_{};

};

