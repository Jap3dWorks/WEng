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
using WVkMeshDb = TObjectDataBase<WVkMesh, void, wcr::wid::WTypeAssetIndexId::IdType>;

// WARNING UBOs can be frame dependent
using WVkUBODb = TObjectDataBase<WVkUBO, void, wcr::wid::WEngId::IdType>;

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

    WNODISCARD WVkTextureInfo const & TextureInfo(const wcr::wid::WAssetId & id) const {
        return texture_collection_.Get(id.GetId());
    }

    // Static Mesh

    void LoadStaticMesh(const wcr::wid::WTypeAssetIndexId & in_id,
                        const wct::geometry::WMesh & in_mesh) {

        static_mesh_collection_.CreateAt(
            in_id,
            [this, &in_mesh] (const wcr::wid::WTypeAssetIndexId & in_id) -> WVkMesh {
                WVkMesh result;
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

    const WVkMesh & StaticMeshInfo(const wcr::wid::WTypeAssetIndexId & in_id) const;

    // --

    void Clear();

    // UBOs

    /**
     * Create an UBO buffer associated to input ubo_set_id
     */
    WNODISCARD
    std::size_t CreateUBO(wcr::wid::WEngId ubo_set_id,
                          std::size_t ubo_size,
                          void const * initial_data_ptr) {
        return ubo_data_.CreateUBO(vkn_.device, vkn_.physical_device,
                                   ubo_set_id, ubo_size, initial_data_ptr);
    }

    void DestroyUBOs(wcr::wid::WEngId wid) {
        ubo_data_.DestroyUBOs(wid, vkn_.device);
    }

    bool ContainsUBOs(wcr::wid::WEngId wid) const {
        return ubo_data_.ubo_sets.contains(wid);
    }

    WVkUBO const & GetUBO(std::size_t id) const {
        return ubo_data_.ubo_collection.Get(id);
    }

    std::vector<std::size_t> GetUBOs(wcr::wid::WEngId wid) const;

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

    struct UboData {
        WVkUBODb ubo_collection{};

        std::unordered_map<
            wcr::wid::WEngId,
            std::variant<std::size_t, std::vector<std::size_t>>
            > ubo_sets{};

        void Clear(VkDevice device);

        void Reg(wcr::wid::WEngId set_id, std::size_t ubo_id);

        std::size_t CreateUBO(VkDevice device, VkPhysicalDevice pdevice,
                       wcr::wid::WEngId id,
                       std::size_t ubo_size, void const * initial_data);

        void DestroyUBOs(wcr::wid::WEngId wid, VkDevice device);
        
    } ubo_data_{};

};

