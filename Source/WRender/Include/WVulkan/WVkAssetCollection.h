#pragma once

#include <unordered_map>
#include <cassert>

#include "WCore/WCore.h"
#include "WCore/TRef.h"
#include "WCore/WIdPool.h"
#include "WVulkan/WVkRenderCore.hpp"
#include "WVulkan/WVulkan.h"

// Manage the aquirement and release of texture or mesh buffer and stuffs like that.

template<typename D, typename A>
class WRENDER_API WVkAssetCollection
{
public:

    WVkAssetCollection()=default;

    virtual ~WVkAssetCollection() {
        Clear();
    }

    WVkAssetCollection(const WVkAssetCollection & other) {
        Copy(other);
    }

    WVkAssetCollection(WVkAssetCollection && other) noexcept {
        Move(other);
    }

    WVkAssetCollection & operator=(const WVkAssetCollection & other) {
        Copy(other);
        return *this;
    }

    WVkAssetCollection & operator=(WVkAssetCollection && other) noexcept {
        Move(other);
        return *this;
    }

    /** Assign a WId to identify the asset */
    WId RegisterAsset(const A & in_asset) {
        
        WId id = id_pool_.Generate();

        assets_.insert(id, in_asset);

        return id;
    }

    void UnregisterAsset(WId in_id) {
        assets_.erase(in_id);
    }

    /** put the data in the graphical memory */
    void LoadAsset(WId in_id) {
        D d = LoadAssetImpl(assets_[in_id].Get());
        data_.insert(in_id, d);
    }

    /** pop from graphical memory */
    void UnloadAsset(WId in_id) {
        UnloadAssetImpl(data_[in_id]);
        data_.extract(in_id);
    }

    const D & GetData(WId in_id) const noexcept {
        assert(data_.contains(in_id));
        return data_[in_id];
    }

protected:

    virtual D LoadAssetImpl(const A & in_asset) = 0;

    virtual void UnloadAssetImpl(D & in_data) = 0;

private:

    void Move(WVkAssetCollection && other) {
        data_ = std::move(other.data_);
        assets_ = std::move(other.assets_);
        id_pool_ = std::move(other.id_pool_);
    }

    void Copy(const WVkAssetCollection & other) {
        data_ = other.data_;
        assets_ = std::move(other.assets_);
        id_pool_ = other.id_pool_;
    }

    void Clear() {
        for (auto & p : data_) {
            UnloadAssetImpl(p.second);
        }

        data_.clear();
    }

    std::unordered_map<WId, D> data_{};
    std::unordered_map<WId, TRef<A>> assets_{};
    WIdPool id_pool_{};  // TODO: use asset WId?

};

// TODO staic Create methods and the constructor with parameters private.

struct WTextureStruct;

class WVkTextureCollection : public WVkAssetCollection<WVkTextureInfo, WTextureStruct> {

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

    WVkTextureInfo LoadAssetImpl(const WTextureStruct & in_asset) override;

    void UnloadAssetImpl(WVkTextureInfo & in_data) override;

private:

    void Move(WVkTextureCollection && other);

    WVkDeviceInfo device_info_{};
    WVkCommandPoolInfo command_pool_info_{};

};

struct WMeshStruct;

class WVkStaticMeshCollection : public WVkAssetCollection<WVkMeshInfo, WMeshStruct> {

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

    WVkMeshInfo LoadAssetImpl(const WMeshStruct & in_asset) override;

    void UnloadAssetImpl(WVkMeshInfo & in_data) override;

private:

    void Move(WVkStaticMeshCollection && other);

    WVkDeviceInfo device_info_{};
    WVkCommandPoolInfo command_pool_info_{};
};

