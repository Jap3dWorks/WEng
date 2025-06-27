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


