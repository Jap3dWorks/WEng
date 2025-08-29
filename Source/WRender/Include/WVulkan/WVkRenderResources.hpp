#pragma once

#include "WCore/WCore.hpp"
#include "WStructs/WTextureStructs.hpp"
#include "WVulkan/WVkRenderStructs.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WVulkan/WVulkan.hpp"

#include "WVulkan/WVkResourceCollection.hpp"
#include "WVulkan/WVkRenderStructs.hpp"

/**
 * @brief Manage the lifetime of render resources like geometries or textures.
 */
class WRENDER_API WVkRenderResources {

public:

    WVkRenderResources();

    WVkRenderResources(
        const WVkDeviceInfo & in_device_info,
        const WVkCommandPoolInfo & in_command_pool_info
        );

    virtual ~WVkRenderResources();

    WVkRenderResources(const WVkRenderResources & other) = delete;

    WVkRenderResources(WVkRenderResources && other);

    WVkRenderResources & operator=(const WVkRenderResources & other) = delete;

    WVkRenderResources & operator=(WVkRenderResources && other);

    // Texture

    // void RegisterTexture(WTextureAsset & in_texture_struct);

    // void UnregisterTexture(const WAssetId & in_id);

    void LoadTexture(const WAssetId & in_id, const WTextureStruct & in_texture) {
        texture_collection_.LoadResource(
            in_id,
            [this, &in_texture](const WAssetId & _id) -> WVkTextureInfo {
            WVkTextureInfo result;
            WVulkan::Create(
                result, in_texture, device_info_, command_pool_info_
                );
            return result;
        });
    }

    void UnloadTexture(const WAssetId & in_id);

    const WVkTextureInfo & TextureInfo(const WAssetId & in_id) const;

    // Static Mesh

    // void RegisterStaticMesh(WStaticMeshAsset & in_mesh_struct) ;

    // void UnregisterStaticMesh(const WAssetId & in_id);

    void LoadStaticMesh(const WAssetId & in_id);

    void UnloadStaticMesh(const WAssetId & in_id);

    const WVkMeshInfo & StaticMeshInfo(const WAssetId & in_id) const;

    void Clear();

private:

    void InitializeTextureFunctions();

    void InitializeStaticMeshFunctions();

    WVkResourceCollection<WVkTextureInfo, WAssetId> texture_collection_;

    WVkResourceCollection<WVkMeshInfo, WAssetIndexId> static_mesh_collection_;

    std::unordered_map<WAssetId, std::vector<WAssIdxId>> mesh_indexes_;

    WVkDeviceInfo device_info_;
    WVkCommandPoolInfo command_pool_info_;
    
};

