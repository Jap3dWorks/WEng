#include "WVulkan/WVkAssetCollection.hpp"
#include "WVulkan/WVulkan.hpp"

// WVkTextureCollection
// --------------------

WVkTextureInfo WVkTextureCollection::LoadAssetImpl(const WTextureAsset & in_asset) {
    WVkTextureInfo result;

    WVulkan::Create(
        result,
        in_asset.GetTexture(),
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

// WVkStaticMeshCollection
// -----------------------

WVkMeshInfo WVkStaticMeshCollection::LoadAssetImpl(const WStaticMeshAsset & in_asset) {
    WVkMeshInfo mesh_info;
    WVulkan::Create(
        mesh_info,
        in_asset.GetMesh(),
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


