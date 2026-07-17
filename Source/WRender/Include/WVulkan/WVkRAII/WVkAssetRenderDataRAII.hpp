#pragma once

#include "WCore/TObjectDataBase.hpp"
#include "WCore/WCore.hpp"

#include "WCore/WId.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WCoreTypes/WTexture.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkMesh.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include "WVulkan/WVk/WVkTexture.hpp"

#include "WVulkan/WVulkanStructs.hpp"
#include <vulkan/vulkan_core.h>

/**
 * @brief Manage the lifetime of asset render data like geometries and textures.
 */
class WRENDER_API WVkAssetRenderDataRAII {
private:

using WVkTextureDb = TObjectDataBase<WVkTextureInfo, void, wcr::wid::WTypeAssetIndexId::IdType>;
using WVkMeshDb = TObjectDataBase<WVkMeshInfo, void, wcr::wid::WTypeAssetIndexId::IdType>;

// WARNING UBOs can be frame dependent
using WVkUBOInfo = TObjectDataBase<WVkUBOInfo, void, wcr::wid::WEngId::IdType>;

public:

    WVkAssetRenderDataRAII()=default;

    WVkAssetRenderDataRAII(
        const VkDevice & in_device_info,
        const VkPhysicalDevice & in_physical_device,
        const VkQueue & in_graphics_queue,
        const VkCommandPool & in_command_pool_info
        );

    ~WVkAssetRenderDataRAII();

    WVkAssetRenderDataRAII(const WVkAssetRenderDataRAII & other) = delete;

    WVkAssetRenderDataRAII(WVkAssetRenderDataRAII && other);

    WVkAssetRenderDataRAII & operator=(const WVkAssetRenderDataRAII & other) = delete;

    WVkAssetRenderDataRAII & operator=(WVkAssetRenderDataRAII && other);

    // Texture TODO wcr::wid::WTypeAssetIndex
    
    void LoadTexture(const wcr::wid::WAssetId & in_id, const WTextureAsset & in_texture) {
        
        texture_collection_.CreateAt(
            in_id.GetId(),
            [this, &in_texture](const wcr::wid::WAssetId & _id) -> WVkTextureInfo {
                WVkTextureInfo result;
                wvk::texture::CreateTexture(
                    result,
                    in_texture,
                    vkn_.device,
                    vkn_.physical_device,
                    vkn_.graphics_queue,
                    vkn_.command_pool
                    );
                return result;
            });
    }

    void UnloadTexture(const wcr::wid::WAssetId & in_id);

    const WVkTextureInfo & TextureInfo(const wcr::wid::WAssetId & in_id) const;

    // Static Mesh

    void LoadStaticMesh(const wcr::wid::WTypeAssetIndexId & in_id,
                        const wct::geometry::WMesh & in_mesh) {

        static_mesh_collection_.CreateAt(
            in_id,
            [this, &in_mesh] (const wcr::wid::WTypeAssetIndexId & in_id) -> WVkMeshInfo {
                WVkMeshInfo result;
                wvk::mesh::CreateMeshBuffers(
                    result,
                    in_mesh.vertices.data(),
                    sizeof(decltype(in_mesh.vertices)::value_type) * in_mesh.vertices.size(),
                    in_mesh.indices.data(),
                    sizeof(decltype(in_mesh.indices)::value_type) * in_mesh.indices.size(),
                    in_mesh.indices.size(),
                    vkn_.device,
                    vkn_.physical_device,
                    vkn_.graphics_queue,
                    vkn_.command_pool
                    );
                
                return result;
            }
            );
    }

    void UnloadStaticMesh(const wcr::wid::WTypeAssetIndexId & in_id);

    const WVkMeshInfo & StaticMeshInfo(const wcr::wid::WTypeAssetIndexId & in_id) const;

    void LoadUBO(wcr::wid::WEngId id, std::size_t ubo_size, void * initial_data_ptr);

    void UnloadUBO(wcr::wid::WEngId id);

    WVkUBOInfo const & GetUBO(wcr::wid::WEngId id) const;

    void Clear();

private:

    void Destroy();

    struct Vkn {
        VkDevice device{VK_NULL_HANDLE};
        VkPhysicalDevice physical_device{VK_NULL_HANDLE};
        VkQueue graphics_queue{VK_NULL_HANDLE};
        VkCommandPool command_pool{VK_NULL_HANDLE};
    } vkn_{} ;

    WVkTextureDb texture_collection_{};
    WVkMeshDb static_mesh_collection_{};
    WVkUBOInfo ubo_collection_{};
    // ubo db;


};

