#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WEngineObjects/TWRef.hpp"

#include <vector>
#include <memory>
#include <unordered_map>
#include <array>
#include <memory>
#include <type_traits>
#include <concepts>

#define WCONTAINER_MAX_OBJECTS 1024

/*
 * This class is a container for all WObjectContainer.
 * the manager will be the responsible of creation and storage of all WObject types.
 * Also will assign an unique id to each WObject.
*/
class WObjectManager
{

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

    // TODO implement TWRef reference track wrappers

    /**
     * @brief Create a new WObject of type T
    */
    template <std::derived_from<WObject> T>
    TWRef<T> CreateObject(const char* in_object_path)
    {
        const WClass& object_class = T::GetStaticClass();

        if (!containers_.contains(object_class)) {
            containers_[object_class] = std::make_unique<TObjectDataBase<T>>();
        }

        WId id = containers_[object_class]->Create();

        void* result;

        containers_[object_class]->Get(id, result);

        T* asset = reinterpret_cast<T*>(result);
        asset->WID(id);

        return asset;
    }

    template <std::derived_from<WObject> T>
    TWRef<T> GetObject(WId in_id)
    {
        WClass object_class = T::GetDefaultObject()->GetClass();

        void * result;
        containers_[object_class]->Get(in_id, result);

        return reinterpret_cast<T*>(result);
    }

private:

    constexpr void Move(WObjectManager && other) noexcept {
        containers_ = std::move(other.containers_);
    }

    std::unordered_map<WClass, std::unique_ptr<IObjectDataBase>> containers_{};

};

