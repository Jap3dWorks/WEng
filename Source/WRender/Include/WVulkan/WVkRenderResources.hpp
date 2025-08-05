#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderStructs.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WAssets/WStaticMeshAsset.hpp"

#include "WVulkan/WVkAssetCollection.hpp"
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

    void RegisterTexture(WTextureAsset & in_texture_struct);

    void UnregisterTexture(const WAssetId & in_id);

    void LoadTexture(const WAssetId & in_id);

    void UnloadTexture(const WAssetId & in_id);

    const WVkTextureInfo & TextureInfo(const WAssetId & in_id) const;

    // Static Mesh

    void RegisterStaticMesh(WStaticMeshAsset & in_mesh_struct) ;

    void UnregisterStaticMesh(const WAssetId & in_id);

    void LoadStaticMesh(const WAssetId & in_id);

    void UnloadStaticMesh(const WAssetId & in_id);

    const WVkMeshInfo & StaticMeshInfo(const WAssetId & in_id) const;

    void Clear();

private:

    void InitializeTextureFunctions();

    void InitializeStaticMeshFunctions();

    WVkAssetCollection<WVkTextureInfo, WTextureAsset> texture_collection_;

    WVkAssetCollection<WVkMeshInfo, WStaticMeshAsset> static_mesh_collection_;

    WVkDeviceInfo device_info_;
    WVkCommandPoolInfo command_pool_info_;
    
};

