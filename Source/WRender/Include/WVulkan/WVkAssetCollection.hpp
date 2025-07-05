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
#include "WLog.hpp"

#include <iostream>
#include <unordered_map>
#include <cassert>
#include <concepts>

// Manage the aquirement and release of texture or mesh buffer and stuffs like that.

template<typename D,  std::derived_from<WAsset> A>
class WRENDER_API WVkAssetCollection
{
public:

    constexpr WVkAssetCollection() noexcept :
    assets_(), data_() {}

    virtual ~WVkAssetCollection() {
        Clear();
    }

    WVkAssetCollection(const WVkAssetCollection & other) :
    assets_(other.assets_),
    data_(other.data_)
    {}

    constexpr WVkAssetCollection(WVkAssetCollection && other) noexcept :
    assets_(std::move(other.assets_)),
    data_(std::move(other.data_))
    {}

    WVkAssetCollection & operator=(const WVkAssetCollection & other) {
        Copy(other);
        return *this;
    }

    constexpr WVkAssetCollection & operator=(WVkAssetCollection && other) noexcept {
        Move(std::move(other));
        return *this;
    }

    /** Assign a WId to identify the asset */
    void RegisterAsset(A & in_asset) {
        assets_.insert({in_asset.WID(), TRef<A>(in_asset)});
    }

    void UnregisterAsset(const WId & in_id) {
        assert(assets_.contains(in_id));
        assets_.erase(in_id);
    }

    void UnregisterAsset(const A & in_asset) {
        UnregisterAsset(in_asset.Wid());
    }

    /** put the data in the graphical memory */
    void LoadAsset(const WId & in_id) {
        assert(assets_.contains(in_id));
        data_.insert({in_id, LoadAssetImpl(assets_[in_id].Get())});
    }

    void LoadAsset(const A & in_asset) {
        LoadAsset(in_asset.Wid());
    }

    /** pop from graphical memory */
    void UnloadAsset(const WId & in_id) {
        assert(assets_.contains(in_id));
        
        UnloadAssetImpl(data_[in_id]);
        data_.extract(in_id);
    }

    void UnloadAsset(const A & in_asset) {
        UnloadAsset(in_asset.WID());
    }

    const D & GetData(const WId & in_id) const noexcept {
        assert(data_.contains(in_id));
        return data_.at(in_id);
    }

    bool Contains(const WId & in_id) const noexcept {
        return data_.contains(in_id);
    }

protected:

    virtual D LoadAssetImpl(const A & in_asset) = 0;

    virtual void UnloadAssetImpl(D & in_data) = 0;

private:

    constexpr void Move(WVkAssetCollection && other) noexcept {
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
        assets_.clear();
    }

    std::unordered_map<WId, TRef<A>> assets_;    
    
    std::unordered_map<WId, D> data_;

};

// TODO staic Create methods and the constructor with parameters private.

class WVkTextureCollection : public WVkAssetCollection<WVkTextureInfo, WTextureAsset> {

public:

    constexpr WVkTextureCollection() noexcept = default;

    constexpr WVkTextureCollection(
        const WVkDeviceInfo & in_device_info,
        const WVkCommandPoolInfo & in_command_pool_info
        ) noexcept :
        WVkAssetCollection(),
        device_info_(in_device_info),
        command_pool_info_(in_command_pool_info) {}

    ~WVkTextureCollection() = default;

    WVkTextureCollection(const WVkTextureCollection & other) = delete;
    
    constexpr WVkTextureCollection(WVkTextureCollection && other) noexcept :
        WVkAssetCollection(std::move(other)),
        device_info_(std::move(other.device_info_)),
        command_pool_info_(std::move(other.command_pool_info_)) {}


    WVkTextureCollection & operator=(const WVkTextureCollection & other) = delete;

    constexpr WVkTextureCollection & operator=(WVkTextureCollection && other) noexcept {
        WVkAssetCollection::operator=(std::move(other));
        Move(std::move(other));

        return *this;

    }

protected:

    WVkTextureInfo LoadAssetImpl(const WTextureAsset & in_asset) override;
    void UnloadAssetImpl(WVkTextureInfo & in_data) override;

private:

    constexpr void Move(WVkTextureCollection && other) noexcept {
        device_info_ = std::move(other.device_info_);
        command_pool_info_ = std::move(other.command_pool_info_);
    }

    WVkDeviceInfo device_info_;
    WVkCommandPoolInfo command_pool_info_;

};

class WVkStaticMeshCollection : public WVkAssetCollection<WVkMeshInfo, WStaticMeshAsset> {

public:

    constexpr WVkStaticMeshCollection() noexcept =default;

    constexpr WVkStaticMeshCollection(
        const WVkDeviceInfo & in_device_info,
        const WVkCommandPoolInfo & in_command_pool_info
        ) noexcept :
        WVkAssetCollection(),
        device_info_(in_device_info),
        command_pool_info_(in_command_pool_info) {}


    ~WVkStaticMeshCollection() = default;

    WVkStaticMeshCollection(const WVkStaticMeshCollection & other) = delete;

    constexpr WVkStaticMeshCollection(WVkStaticMeshCollection && other) noexcept :
        WVkAssetCollection(std::move(other)),
        device_info_(std::move(other.device_info_)),
        command_pool_info_(std::move(other.command_pool_info_)) {}

    WVkStaticMeshCollection & operator=(const WVkStaticMeshCollection & in_other) = delete;

    constexpr WVkStaticMeshCollection & operator=(WVkStaticMeshCollection && other) noexcept {
        WVkAssetCollection::operator=(std::move(other));
        Move(std::move(other));

        return *this;
    }

protected:

    WVkMeshInfo LoadAssetImpl(const WStaticMeshAsset & in_asset) override;
    void UnloadAssetImpl(WVkMeshInfo & in_data) override;

private:

    constexpr void Move(WVkStaticMeshCollection && other) noexcept {
        device_info_ = std::move(other.device_info_);
        command_pool_info_ = std::move(other.command_pool_info_);
    }

    WVkDeviceInfo device_info_;
    WVkCommandPoolInfo command_pool_info_;
};

