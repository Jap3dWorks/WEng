#pragma once

#include "WVulkan/WVulkanStructs.hpp"

namespace wvk::mesh {
    /**
     * @brief Create a vulkan  mesh
     */
    void CreateMeshBuffers(
        WVkMeshInfo & out_mesh_info,
        const void * vertex_buffer,
        const std::uint32_t & vertes_buffer_size,
        const void * index_buffer,
        const std::uint32_t & index_buffer_size,
        const std::uint32_t & index_count,
        const VkDevice & device,
        const VkPhysicalDevice & in_physical_device,
        const VkQueue & in_queue,
        const VkCommandPool & command_pool_info
        );

    void Destroy(
        WVkMeshInfo & out_mesh_info,
        const VkDevice & in_device_info
        );
    
}
