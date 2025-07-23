#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WCore/WIdPool.hpp"
#include "WObjectManager/WObjectManager.hpp"

#include <unordered_map>

class WAsset;

/**
 * @brief Facade class for a WObjectManager specialized in the asset management.
 * Core functionalities are derived to WObjectManager.
 */
class WENGINEOBJECTS_API WAssetManager {
    
public:

    constexpr WAssetManager() noexcept :
    object_manager_(),
    id_pool_(),
    id_class_() {
        InitializeIdPool();
    }

    virtual ~WAssetManager() = default;

    WAssetManager(const WAssetManager & other) :
    object_manager_(other.object_manager_),
    id_pool_(other.id_pool_),
    id_class_(other.id_class_) {}
    
    WAssetManager(WAssetManager && other) :
    object_manager_(std::move(other.object_manager_)),
    id_pool_(std::move(other.id_pool_)),
    id_class_(std::move(other.id_class_)) {}

    WAssetManager & operator=(const WAssetManager & other) {
        if (this != &other) {
            object_manager_ = other.object_manager_;
            id_pool_ = other.id_pool_;
            id_class_ = other.id_class_;
        }
        return *this;
    }

    WAssetManager & operator=(WAssetManager && other) {
        if (this != &other) {
            object_manager_ = std::move(other.object_manager_);
            id_pool_ = std::move(other.id_pool_);
            id_class_ = std::move(other.id_class_);
        }
        return *this;
    }

    template<std::derived_from<WObject> T>
    WId Create(const char * in_fullname) {
        return Create(T::StaticClass(), in_fullname);
    }

    WId Create(const WClass * in_class,
               const char * in_fullname);

    TWRef<WAsset> Get(const WId & in_id);

    TRef<WObjectManager> ObjectManager() noexcept;

private:

    void InitializeIdPool();

    WObjectManager object_manager_;

    WIdPool id_pool_;

    std::unordered_map<WId, const WClass *> id_class_;

};
