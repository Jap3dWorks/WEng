#pragma once

#include "WCore/TObjectDataBase.hpp"
#include "WCore/WCore.hpp"

#include "WCoreTypes/WGeometry.hpp"
#include "WCoreTypes/WTexture.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkMesh.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include "WVulkan/WVk/WVkTexture.hpp"

#include "WVulkan/WVulkanStructs.hpp"

/**
 * @brief Manage the lifetime of asset render data like geometries and textures.
 */
class WRENDER_API WVkAssetRenderDataRAII {
private:

using WVkTextureDb = TObjectDataBase<WVkTextureInfo, void, WAssetId>;
using WVkMeshDb = TObjectDataBase<WVkMeshInfo, void, WAssetIndexId>;

public:

    WVkAssetRenderDataRAII();

    WVkAssetRenderDataRAII(
        const VkDevice & in_device_info,
        const VkPhysicalDevice & in_physical_device,
        const VkQueue & in_graphics_queue,
        const VkCommandPool & in_command_pool_info
        );

    virtual ~WVkAssetRenderDataRAII();

    WVkAssetRenderDataRAII(const WVkAssetRenderDataRAII & other) = delete;

    WVkAssetRenderDataRAII(WVkAssetRenderDataRAII && other);

    WVkAssetRenderDataRAII & operator=(const WVkAssetRenderDataRAII & other) = delete;

    WVkAssetRenderDataRAII & operator=(WVkAssetRenderDataRAII && other);

    // Texture

    void LoadTexture(const WAssetId & in_id, const WTextureAsset & in_texture) {
        texture_collection_.CreateAt(
            in_id,
            [this, &in_texture](const WAssetId & _id) -> WVkTextureInfo {
                WVkTextureInfo result;
                wvk::texture::CreateTexture(
                    result,
                    in_texture,
                    device_,
                    physical_device_,
                    graphics_queue_,
                    command_pool_
                    );
                return result;
            });
    }

    void UnloadTexture(const WAssetId & in_id);

    const WVkTextureInfo & TextureInfo(const WAssetId & in_id) const;

    // Static Mesh

    void LoadStaticMesh(const WAssetIndexId & in_id, const wct::geometry::WMesh & in_mesh) {

        static_mesh_collection_.CreateAt(
            in_id,
            [this, &in_mesh] (const WAssetIndexId & in_id) -> WVkMeshInfo {
                WVkMeshInfo result;
                wvk::mesh::CreateMeshBuffers(
                    result,
                    in_mesh.vertices.data(),
                    sizeof(decltype(in_mesh.vertices)::value_type) * in_mesh.vertices.size(),
                    in_mesh.indices.data(),
                    sizeof(decltype(in_mesh.indices)::value_type) * in_mesh.indices.size(),
                    in_mesh.indices.size(),
                    device_,
                    physical_device_,
                    graphics_queue_,
                    command_pool_
                    );
                
                return result;
            }
            );
    }

    void UnloadStaticMesh(const WAssetIndexId & in_id);

    const WVkMeshInfo & StaticMeshInfo(const WAssetIndexId & in_id) const;

    void Clear();

private:

    void Destroy();
    
    VkDevice device_;
    VkPhysicalDevice physical_device_;
    VkQueue graphics_queue_;
    VkCommandPool command_pool_;

    WVkTextureDb texture_collection_;
             
    WVkMeshDb static_mesh_collection_;

    std::unordered_map<WAssetId, std::vector<WSubIdxId>> mesh_indexes_;

};

