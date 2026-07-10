#pragma once

#include "WCore/WCore.hpp"
#include "WCore/IdPool.hpp"
#include "WCore/WId.hpp"
#include "WObjectDb/WObjectDb.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WAssets/Level.hpp"

#include <concepts>
#include <limits>
#include <unordered_map>

class WAsset;

/**
 * @brief Facade class for a WObjectDb specialized in the asset management.
 * Core functionalities are derived to WObjectDb.
 */
class WENGINEOBJECTS_API WAssetDb {
    
public:

    using WAssetDbType = WObjectDb<WAsset, WAssetId>;

    constexpr WAssetDb() noexcept = default;

    virtual ~WAssetDb() = default;

    WAssetDb(WAssetDb const & other) = default;
    
    WAssetDb(WAssetDb && other) = default;

    WAssetDb & operator=(WAssetDb const & other) = default;

    WAssetDb & operator=(WAssetDb && other) = default;

    template<std::derived_from<WAsset> T, CCallable<void, T&> TFn>
    void ForEach(TFn && in_fn) const {
        object_manager_.ForEach<T>(
            std::forward<TFn>(in_fn)
            );
    }

    template<std::derived_from<WAsset> T>
    WAssetId Create(std::string_view in_fullname) {
        WAssetId id = GetIdPool(T::StaticClass()).Generate();

        id_class_[id] = T::StaticClass();
    
        object_manager_.CreateAt<T>(id);
        
        object_manager_.Get<T>(id).Set_asset_id(id);
        object_manager_.Get<T>(id).Set_name(in_fullname);

        return id;
    }

    // WAssetId Create(WClass const * in_class,
    //                 std::string_view in_fullname) {
    //     WAssetId id = GetIdPool(in_class).Generate();

    //     id_class_[id] = in_class;
    
    //     object_manager_.CreateAt(in_class, id);
        
    //     object_manager_.Get(in_class, id)->Set_asset_id(id);
    //     object_manager_.Get(in_class, id)->Set_name(in_fullname);

    //     return id;
    // }

    template<std::derived_from<WAsset> T>
    WAssetId CreateFrom(std::string_view in_fullname, T const & other) {
        WAssetId asset_id = Create<T>(in_fullname);
        T * ptr = Get<T>(asset_id);

        *ptr = other;
        ptr -> Set_asset_id(asset_id);
        ptr -> Set_name(in_fullname);

        return asset_id;
    }

    template<std::derived_from<WAsset> T>
    WAssetId CreateFrom(std::string_view in_fullname, T && other) {
        WAssetId asset_id = Create<T>(in_fullname);
        T * ptr = Get<T>(asset_id);

        *ptr = std::move(other);
        ptr -> Set_asset_id(asset_id);
        ptr -> Set_name(in_fullname);

        return asset_id;
    }

    // TODO ptr or reference?
    template<std::derived_from<WAsset> T>
    T * Get(WAssetId const & in_id) const {
        assert(id_class_.contains(in_id));

        return static_cast<T*>(object_manager_.Get(
                                   id_class_.at(in_id),
                                   in_id));
    }

    WAsset * Get(WAssetId const & in_id) const {
        assert(id_class_.contains(in_id));
        return object_manager_.Get(id_class_.at(in_id), in_id);
    }

    WAsset * Get(std::string_view asset_name) const;

private:

    wcr::IdPool<WAssetId::IdType> & GetIdPool(WClass const * in_class) {
        if (was::Level::StaticClass()->IsEqual(in_class)) {
            return id_level_pool_;
        }
        else {
            return id_pool_;
        }
    }

    WAssetDbType object_manager_{};
    
    wcr::IdPool<WAssetId::IdType> id_level_pool_{
        {
            {
                .first=1,
                .last = WEntityComponentId::BitMaskV<WAssetId>
            }
        }
    };

    wcr::IdPool<WAssetId::IdType> id_pool_{
        {
            {
                .first=WEntityComponentId::BitMaskV<WAssetId> + 1,
                .last=std::numeric_limits<WAssetId::IdType>::max()
            }
        }
    };

    std::unordered_map<WAssetId, WClass const *> id_class_{};

    struct NameData{
        std::string_view name;
        WAssetId asset_id;
        WClass const * asset_class;
    };


};
