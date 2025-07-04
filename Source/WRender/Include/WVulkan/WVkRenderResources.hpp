#pragma once

#include "WCore/WCore.hpp"

#include "IRenderResources.hpp"
#include "WVulkan/WVkAssetCollection.hpp"
#include "WVulkan/WVkRenderCore.hpp"

class WRENDER_API WVkRenderResources : public IRenderResources {

public:

    WVkRenderResources();

    WVkRenderResources(const WVkDeviceInfo & in_device_info, const WVkCommandPoolInfo & in_command_pool_info);

    virtual ~WVkRenderResources();

    WVkRenderResources(const WVkRenderResources & other) = delete;

    WVkRenderResources(WVkRenderResources && other);

    WVkRenderResources & operator=(const WVkRenderResources & other) = delete;

    WVkRenderResources & operator=(WVkRenderResources && other);

    // Texture

    void RegisterTexture(WTextureAsset & in_texture_struct) override;

    void UnregisterTexture(WId in_id) override;

    void LoadTexture(WId in_id) override;

    void UnloadTexture(WId in_id) override;

    // Static Mesh

    void RegisterStaticMesh(WStaticMeshAsset & in_mesh_struct) override;

    void UnregisterStaticMesh(WId in_id) override;

    void LoadStaticMesh(WId in_id) override;

    void UnloadStaticMesh(WId in_id) override;

    const WVkTextureInfo & TextureInfo(WId in_id) const;

    const WVkMeshInfo & StaticMeshInfo(WId in_id) const;

private:

    void Move(WVkRenderResources && other);

    WVkTextureCollection texture_collection{};
    WVkStaticMeshCollection static_mesh_collection{};
  
};

