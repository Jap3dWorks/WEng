#pragma once

#include "WCore/WCore.hpp"
#include "WCore/IdPool.hpp"
#include "WCore/WId.hpp"
#include "WCore/TPathTree.hpp"
#include "WString/WString.hpp"
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

using WAssetDbType = WObjectDb<WAsset, wid::WAssetId>;

    constexpr WAssetDb() noexcept = default;
    ~WAssetDb() = default;
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
    wid::WAssetId Create(std::string_view in_fullname) {
        wid::WAssetId id = GetIdPool(T::StaticClass()).Generate();

        wclass_track_.RegAsset(id, T::StaticClass());

        object_manager_.CreateAt<T>(id);
        
        object_manager_.Get<T>(id).Set_asset_id(id);
        object_manager_.Get<T>(id).Set_name(in_fullname);

        InsertPath(in_fullname, id);

        return id;
    }

    template<std::derived_from<WAsset> T>
    wid::WAssetId CreateFrom(std::string_view in_fullname, T const & other) {
        wid::WAssetId asset_id = Create<T>(in_fullname);

        T & ptr = Get<T>(asset_id);

        ptr = other;
        ptr.Set_asset_id(asset_id);
        ptr.Set_name(in_fullname);

        return asset_id;
    }

    template<std::derived_from<WAsset> T>
    wid::WAssetId CreateFrom(std::string_view in_fullname, T && other) {
        wid::WAssetId asset_id = Create<T>(in_fullname);

        T & ptr = Get<T>(asset_id);

        ptr = std::move(other);
        ptr.Set_asset_id(asset_id);
        ptr.Set_name(in_fullname);

        return asset_id;
    }

    /**
     * Exact T class.
     */
    template<std::derived_from<WAsset> T>
    T & Get(wid::WAssetId const & in_id) const {
        return object_manager_.Get<T>(in_id);
    }

    WAsset * Get(wid::WTypeAssetIndexId in_id) const {

        wid::WAssetTypeId atype;
        wid::WAssetId assetid;
        wid::WSubIdxId indx;
        in_id.ExtractWIds(atype, assetid, indx);

        WClass const * wclass = wclass_track_.GetWClass(atype);

        return object_manager_.Get(wclass, assetid);
    }

    WAsset * Get(wid::WAssetId in_id) const {
        wid::WAssetTypeId atype = wclass_track_.GetAssetTypeId(in_id);
        WClass const * aclass = wclass_track_.GetWClass(atype);

        return object_manager_.Get(aclass, in_id);
    }

    template<std::derived_from<WAsset> T>
    T * Get(std::string_view asset_name) const {

        WAsset * result = Get(asset_name);
        
        if (!result) return nullptr;

        // TODO FIXING
        auto asssname = result->Get_name();
        std::string clssname = result->Class()->Name();
        std::string othername = T::StaticClass()->Name();

        assert(result->Class()->IsEqual(T::StaticClass()));

        return static_cast<T*> (result);
    }

    WAsset * Get(std::string_view asset_name) const;

private:

    void InsertPath(std::string_view in_path, wid::WAssetId in_id) {
        auto split_path = wstr::SplitAssetPath(in_path);

        path_tree_.Insert(split_path, in_id);
    }

    wcr::IdPool<wid::WAssetId::IdType> & GetIdPool(WClass const * in_class) {
        if (was::Level::StaticClass()->IsEqual(in_class)) {
            return id_level_pool_;
        }
        else {
            return id_pool_;
        }
    }

    WAssetDbType object_manager_{};
    
    wcr::IdPool<wid::WAssetId::IdType> id_level_pool_{
        {
            {
                .first=1,
                .last = wid::WEntityComponentId::BitMaskV<wid::WAssetId>
            }
        }
    };

    wcr::IdPool<wid::WAssetId::IdType> id_pool_{
        {
            {
                .first=wid::WEntityComponentId::BitMaskV<wid::WAssetId> + 1,
                .last=std::numeric_limits<wid::WAssetId::IdType>::max()
            }
        }
    };

    struct {
        wid::WAssetTypeId::IdType id_counter{0};

        std::vector<WClass const *> class_list{};
        std::unordered_map<WClass const *,std::uint32_t> wclass_id{};
        std::unordered_map<wid::WAssetId, wid::WAssetTypeId::IdType> asset_typeid{};

        bool Contains(WClass const * in_class) const {
            return wclass_id.contains(in_class);
        }

        void RegAsset(wid::WAssetId assetid, WClass const * in_class) {
            if (!wclass_id.contains(in_class)) {
                class_list.push_back(in_class);
                wclass_id[in_class] = id_counter;
                
                id_counter++;
            }

            asset_typeid[assetid] = wclass_id[in_class];
            
        }

        wid::WAssetTypeId GetTypeId(WClass const * in_class) const {
            return wclass_id.at(in_class);
        }

        WClass const * GetWClass(wid::WAssetTypeId in_id) const {
            return class_list[in_id.GetId()];
        }

        wid::WAssetTypeId GetAssetTypeId(wid::WAssetId in_id) const {
#ifndef NDEBUG
            auto idtype = asset_typeid.at(in_id);
#endif
            return asset_typeid.at(in_id);
        }

    } wclass_track_{};

    wcr::TPathTree<wid::WAssetId> path_tree_{};

};
