#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCore/TFunction.hpp"
#include "WCore/TObjectDataBase.hpp"

#include <unordered_map>
#include <cassert>
#include <concepts>

// Manage the aquirement and release of texture or mesh buffer and stuffs like that.

template<typename D,  std::derived_from<WAsset> A>
class WRENDER_API WVkAssetCollection
{
public:

    using WVkAssetDb = TObjectDataBase<D, void, WAssetId>;

    constexpr WVkAssetCollection() noexcept :
    assets_(), data_() {}

    constexpr WVkAssetCollection(
        TFunction<D(const WAssetId & id)> in_create_fn,
        TFunction<void(D&)> in_clear_fn) :
    data_(in_create_fn, in_clear_fn) {}

    virtual ~WVkAssetCollection() { Clear(); }

    WVkAssetCollection(const WVkAssetCollection & other) :
    assets_(other.assets_),
    data_(other.data_)
    {}

    constexpr WVkAssetCollection(WVkAssetCollection && other) noexcept :
    assets_(std::move(other.assets_)),
    data_(std::move(other.data_))
    {}

    WVkAssetCollection & operator=(const WVkAssetCollection & other) {
        if (this != &other) {
            assets_ = other.assets_;
            data_ = other.data_;            
        }

        return *this;
    }

    constexpr WVkAssetCollection & operator=(WVkAssetCollection && other) noexcept {

        if (this != &other) {
            assets_ = std::move(other.assets_);
            data_ = std::move(other.data_);
        }

        return *this;
    }

    /** Assign a WAssetId to identify the asset */
    void RegisterAsset(A & in_asset) {
        assets_.insert({in_asset.WID(), in_asset});
    }

    void UnregisterAsset(const WAssetId & in_id) {
        assert(assets_.contains(in_id));
        assets_.erase(in_id);
    }

    void UnregisterAsset(const A & in_asset) {
        UnregisterAsset(in_asset.Wid());
    }

    /** put the data in the graphical memory */
    void LoadAsset(const WAssetId & in_id) {
        assert(assets_.contains(in_id));
        data_.Insert(in_id);
    }

    void LoadAsset(const A & in_asset) {
        LoadAsset(in_asset.Wid());
    }

    /** pop from graphical memory */
    void UnloadAsset(const WAssetId & in_id) {
        assert(assets_.contains(in_id));
        data_.Remove(in_id);
    }

    void UnloadAsset(const A & in_asset) {
        UnloadAsset(in_asset.WID());
    }

    const D & GetData(const WAssetId & in_id) const noexcept {
        assert(data_.Contains(in_id));
        return data_.Get(in_id);
    }

    bool Contains(const WAssetId & in_id) const noexcept {
        // return data_.contains(in_id);
        return data_.Contains(in_id);
    }

    const A & GetAsset(const WAssetId & in_id) const {
        return assets_.at(in_id).Get();
    }

    void Clear() {
        assets_.clear();
        data_.Clear();
    }

protected:

private:

    // TWRef to ensure that the reference to asset is alive
    std::unordered_map<WAssetId, TWRef<A>> assets_;
    
    WVkAssetDb data_;

};

