#pragma once

#include "IRenderResources.hpp"
#include "WVulkan/WVkAssetCollection.h"
#include "WVulkan/WVkRenderCore.h"

class WVkRenderResources : public IRenderResources {

public:

    WVkRenderResources();

    WVkRenderResources(const WVkDeviceInfo & in_device_info, const WVkCommandPoolInfo & in_command_pool_info);

    virtual ~WVkRenderResources();

    WVkRenderResources(const WVkRenderResources & other) = delete;

    WVkRenderResources(WVkRenderResources && other);

    WVkRenderResources & operator=(const WVkRenderResources & other) = delete;

    WVkRenderResources & operator=(WVkRenderResources && other);

    // Texture

    WId RegisterTexture(const WTextureStruct & in_texture_struct) override;

    void UnregisterTexture(WId in_id) override;

    void LoadTexture(WId in_id) override;

    void UnloadTexture(WId in_id) override;

    // Static Mesh

    WId RegisterStaticMesh(const WMeshStruct & in_mesh_struct) override;

    void UnregisterStaticMesh(WId in_id) override;

    void LoadStaticMesh(WId in_id) override;

    void UnloadStaticMesh(WId in_id) override;

private:

    void Move(WVkRenderResources && other);

    WVkTextureCollection texture_collection;
    WVkStaticMeshCollection static_mesh_collection;
  
};

