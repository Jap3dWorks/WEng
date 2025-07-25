#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WIdPool.hpp"
#include "WObjectDb/WObjectDb.hpp"
#include "WEngineObjects/WAsset.hpp"

#include <unordered_map>

class WAsset;

/**
 * @brief Facade class for a WObjectDb specialized in the asset management.
 * Core functionalities are derived to WObjectDb.
 */
class WENGINEOBJECTS_API WAssetDb {
    
public:

    constexpr WAssetDb() noexcept :
    object_manager_(),
    id_pool_(),
    id_class_() {
        InitializeIdPool();
    }

    virtual ~WAssetDb() = default;

    WAssetDb(const WAssetDb & other) :
    object_manager_(other.object_manager_),
    id_pool_(other.id_pool_),
    id_class_(other.id_class_) {}
    
    WAssetDb(WAssetDb && other) :
    object_manager_(std::move(other.object_manager_)),
    id_pool_(std::move(other.id_pool_)),
    id_class_(std::move(other.id_class_)) {}

    WAssetDb & operator=(const WAssetDb & other) {
        if (this != &other) {
            object_manager_ = other.object_manager_;
            id_pool_ = other.id_pool_;
            id_class_ = other.id_class_;
        }
        return *this;
    }

    WAssetDb & operator=(WAssetDb && other) {
        if (this != &other) {
            object_manager_ = std::move(other.object_manager_);
            id_pool_ = std::move(other.id_pool_);
            id_class_ = std::move(other.id_class_);
        }
        return *this;
    }

    template<std::derived_from<WAsset> T>
    void ForEach(TFunction<void(T*)> in_predicate) const {
        object_manager_.ForEach<T>([&in_predicate](WObject* in_object) {
            in_predicate(static_cast<T*>(in_object));
        });
    }

    template<std::derived_from<WObject> T>
    WId Create(const char * in_fullname) {
        return Create(T::StaticClass(), in_fullname);
    }

    WId Create(const WClass * in_class,
               const char * in_fullname) {
        WId id = id_pool_.Generate();
        id_class_[id] = in_class;
    
        object_manager_.Create(in_class, id, in_fullname);

        return id;
    }

    WAsset * Get(const WId & in_id) const {
        assert(id_class_.contains(in_id));
        return static_cast<WAsset*>(object_manager_.Get(id_class_.at(in_id), in_id));
    }

    TWRef<WAsset> GetRef(const WId & in_id) const {
        return Get(in_id);
    }

    WObjectDb & ObjectManager() noexcept {
        return object_manager_;
    }

private:

    void InitializeIdPool();

    WObjectDb object_manager_;

    WIdPool id_pool_;

    std::unordered_map<WId, const WClass *> id_class_;

};
