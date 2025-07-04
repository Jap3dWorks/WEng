#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WAssets/WAsset.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WVulkan/WVkRenderCore.hpp"
#include "WVulkan/WVulkan.hpp"

#include <unordered_map>
#include <cassert>
#include <concepts>

// Manage the aquirement and release of texture or mesh buffer and stuffs like that.

template<typename D,  std::derived_from<WAsset> A>
class WRENDER_API WVkAssetCollection
{
public:

    constexpr WVkAssetCollection()=default;

    virtual ~WVkAssetCollection() {
        Clear();
    }

    WVkAssetCollection(const WVkAssetCollection & other) {
        Copy(other);
    }

    WVkAssetCollection(WVkAssetCollection && other) noexcept {
        Move(std::move(other));
    }

    WVkAssetCollection & operator=(const WVkAssetCollection & other) {
        Copy(other);
        return *this;
    }

    WVkAssetCollection & operator=(WVkAssetCollection && other) noexcept {
        Move(std::move(other));
        return *this;
    }

    /** Assign a WId to identify the asset */
    void RegisterAsset(A & in_asset) {
        assets_.insert({in_asset.WID(), TRef<A>(&in_asset)});
    }

    void UnregisterAsset(WId in_id) {
        assert(assets_.contains(in_id));
        assets_.erase(in_id);
    }

    void UnregisterAsset(const A & in_asset) {
        UnregisterAsset(in_asset.Wid());
    }

    /** put the data in the graphical memory */
    void LoadAsset(WId in_id) {
        assert(assets_.contains(in_id));
        data_.insert({in_id, LoadAssetImpl(assets_[in_id].Get())});
    }

    void LoadAsset(const A & in_asset) {
        LoadAsset(in_asset.Wid());
    }

    /** pop from graphical memory */
    void UnloadAsset(WId in_id) {
        assert(assets_.contains(in_id));
        
        UnloadAssetImpl(data_[in_id]);
        data_.extract(in_id);
    }

    void UnloadAsset(const A & in_asset) {
        UnloadAsset(in_asset.WID());
    }

    const D & GetData(WId in_id) const noexcept {
        assert(data_.contains(in_id));
        return data_.at(in_id);
    }

protected:

    virtual D LoadAssetImpl(const A & in_asset) = 0;

    virtual void UnloadAssetImpl(D & in_data) = 0;

private:

    void Move(WVkAssetCollection && other) {
        data_ = std::move(other.data_);
        assets_ = std::move(other.assets_);
    }

    void Copy(const WVkAssetCollection & other) {
        data_ = other.data_;
        assets_ = other.assets_;
    }

    void Clear() {
        for (auto & p : data_) {
            UnloadAssetImpl(p.second);
        }

        data_.clear();
    }

    std::unordered_map<WId, TRef<A>> assets_{};    
    
    std::unordered_map<WId, D> data_{};

};

// TODO staic Create methods and the constructor with parameters private.

class WVkTextureCollection : public WVkAssetCollection<WVkTextureInfo, WTextureAsset> {

public:

    WVkTextureCollection()=default;

    WVkTextureCollection(
        const WVkDeviceInfo & in_device_info, const WVkCommandPoolInfo & in_command_pool_info
        ) :
        device_info_(in_device_info), command_pool_info_(in_command_pool_info) {
        
    }

    ~WVkTextureCollection() override = default;

    WVkTextureCollection(const WVkTextureCollection & other) = delete;
    WVkTextureCollection(WVkTextureCollection && other);

    WVkTextureCollection & operator=(const WVkTextureCollection & other) = delete;
    WVkTextureCollection & operator=(WVkTextureCollection && other);

protected:

    WVkTextureInfo LoadAssetImpl(const WTextureAsset & in_asset) override;

    void UnloadAssetImpl(WVkTextureInfo & in_data) override;

private:

    void Move(WVkTextureCollection && other);

    WVkDeviceInfo device_info_{};
    WVkCommandPoolInfo command_pool_info_{};

};

class WVkStaticMeshCollection : public WVkAssetCollection<WVkMeshInfo, WStaticMeshAsset> {

public:

    WVkStaticMeshCollection()=default;

    WVkStaticMeshCollection(
        const WVkDeviceInfo & in_device_info, const WVkCommandPoolInfo & in_command_pool_info
        );

    ~WVkStaticMeshCollection() override = default;

    WVkStaticMeshCollection(const WVkStaticMeshCollection & other) = delete;
    WVkStaticMeshCollection(WVkStaticMeshCollection && other);

    WVkStaticMeshCollection & operator=(const WVkStaticMeshCollection & in_other) = delete;
    WVkStaticMeshCollection & operator=(WVkStaticMeshCollection && other);

protected:

    WVkMeshInfo LoadAssetImpl(const WStaticMeshAsset & in_asset) override;

    void UnloadAssetImpl(WVkMeshInfo & in_data) override;

private:

    void Move(WVkStaticMeshCollection && other);

    WVkDeviceInfo device_info_{};
    WVkCommandPoolInfo command_pool_info_{};
};

