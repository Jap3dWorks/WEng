#include "WVulkan/WVkAssetCollection.h"
#include "WVulkan/WVulkan.h"

// WVkTextureCollection
// --------------------

WVkTextureCollection::WVkTextureCollection(WVkTextureCollection && other) :
    WVkAssetCollection(std::move(other)) {
    Move(std::move(other));
}

WVkTextureCollection & WVkTextureCollection::operator=(WVkTextureCollection && other) {
    WVkAssetCollection::operator=(std::move(other));
    Move(std::move(other));

    return *this;
}

WVkTextureInfo WVkTextureCollection::LoadAssetImpl(const WTextureStruct & in_asset) {
    WVkTextureInfo result;

    WVulkan::Create(
        result,
        in_asset,
        device_info_,
        command_pool_info_
        );

    return result;
}

void WVkTextureCollection::UnloadAssetImpl(WVkTextureInfo & in_texture_info) {
    WVulkan::Destroy(
        in_texture_info,
        device_info_
        );
}

void WVkTextureCollection::Move(WVkTextureCollection && other) {
    device_info_ = std::move(other.device_info_);
    command_pool_info_ = std::move(other.command_pool_info_);
}

// WVkStaticMeshCollection
// -----------------------

WVkStaticMeshCollection::WVkStaticMeshCollection(
    const WVkDeviceInfo & in_device_info,
    const WVkCommandPoolInfo & in_command_pool_info
    ) :
    device_info_(in_device_info), command_pool_info_(in_command_pool_info) {}

WVkStaticMeshCollection::WVkStaticMeshCollection(WVkStaticMeshCollection && other) :
    WVkAssetCollection(std::move(other)) {
    Move(std::move(other));
}

WVkStaticMeshCollection & WVkStaticMeshCollection::operator=(WVkStaticMeshCollection && other) {
    WVkAssetCollection::operator=(std::move(other));
    Move(std::move(other));

    return *this;
}

void WVkStaticMeshCollection::Move(WVkStaticMeshCollection && other) {
    device_info_ = std::move(other.device_info_);
    command_pool_info_ = std::move(other.command_pool_info_);
}

WVkMeshInfo WVkStaticMeshCollection::LoadAssetImpl(const WMeshStruct & in_asset) {
    WVkMeshInfo mesh_info;
    WVulkan::Create(
        mesh_info,
        in_asset,
        device_info_,
        command_pool_info_
        );
        
    return mesh_info;
}

void WVkStaticMeshCollection::UnloadAssetImpl(WVkMeshInfo & in_data) {
    WVulkan::Destroy(
        in_data,
        device_info_
        );
}


