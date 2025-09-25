#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WIdPool.hpp"
#include "WObjectDb/WObjectDb.hpp"
#include "WEngineObjects/WAsset.hpp"

#include <concepts>
#include <unordered_map>

class WAsset;

/**
 * @brief Facade class for a WObjectDb specialized in the asset management.
 * Core functionalities are derived to WObjectDb.
 */
class WENGINEOBJECTS_API WAssetDb {
    
public:

    using WAssetDbType = WObjectDb<WAsset, WAssetId>;

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

    template<std::derived_from<WAsset> T, CCallable<void, T&> TFn>
    void ForEach(TFn && in_fn) const {
        object_manager_.ForEach<T>(
            std::forward<TFn>(in_fn)
            );
    }

    template<std::derived_from<WAsset> T>
    WAssetId Create(const char * in_fullname) {
        return Create(T::StaticClass(), in_fullname);
    }

    WAssetId Create(const WClass * in_class,
                    const char * in_fullname) {
        WAssetId id = id_pool_.Generate();
        id_class_[id] = in_class;
    
        object_manager_.CreateAt(in_class, id);
        
        object_manager_.Get(in_class, id)->WID(id);
        object_manager_.Get(in_class, id)->Name(in_fullname);

        return id;
    }

    template<std::derived_from<WAsset> T>
    T * Get(const WAssetId & in_id) const {
        assert(id_class_.contains(in_id));

        return static_cast<T*>(object_manager_.Get(
                                   id_class_.at(in_id),
                                   in_id));
    }

    WAsset * Get(const WAssetId & in_id) const {
        assert(id_class_.contains(in_id));
        return object_manager_.Get(id_class_.at(in_id), in_id);
    }

private:

    void InitializeIdPool();

    WAssetDbType object_manager_;

    WIdPool<WAssetId> id_pool_;

    std::unordered_map<WAssetId, const WClass *> id_class_;

};
