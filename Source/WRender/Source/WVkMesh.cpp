#include "WVulkan/WVk/WVkMesh.hpp"
#include "WVulkan/WVk/WVkBuffer.hpp"

void wvk::mesh::CreateMeshBuffers(
    WVkMeshInfo & out_mesh_info,
    const void * vertex_buffer,
    const std::uint32_t & vertex_buffer_size,
    const void * index_buffer,
    const std::uint32_t & index_buffer_size,
    const std::uint32_t & index_count,
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device,
    const VkQueue & in_graphics_queue,
    const VkCommandPool & in_command_pool
    )
{
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    // vertex buffer

    VkDeviceSize buffer_size = vertex_buffer_size;

    wvk::buffer::CreateVkBuffer(
        staging_buffer,
        staging_buffer_memory,
        in_device,
        in_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

    void * data;
    vkMapMemory(in_device, staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, vertex_buffer, static_cast<size_t>(buffer_size));
    vkUnmapMemory(in_device, staging_buffer_memory);

    wvk::buffer::CreateVkBuffer(
        out_mesh_info.vertex_buffer,
        out_mesh_info.vertex_buffer_memory,
        in_device,
        in_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

    wvk::buffer::CopyVkBuffer(
        in_device,
        in_command_pool,
        in_graphics_queue,
        staging_buffer,
        out_mesh_info.vertex_buffer,
        buffer_size);

    vkDestroyBuffer(in_device, staging_buffer, nullptr);
    vkFreeMemory(in_device, staging_buffer_memory, nullptr);

    // index buffer

    wvk::buffer::CreateVkBuffer(
        staging_buffer,
        staging_buffer_memory,
        in_device,
        in_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

    vkMapMemory(in_device, staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, index_buffer, static_cast<size_t>(buffer_size));
    vkUnmapMemory(in_device, staging_buffer_memory);

    wvk::buffer::CreateVkBuffer(
        out_mesh_info.index_buffer,
        out_mesh_info.index_buffer_memory,
        in_device,
        in_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT, // |
          // VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

    wvk::buffer::CopyVkBuffer(
        in_device,
        in_command_pool,
        in_graphics_queue,
        staging_buffer,
        out_mesh_info.index_buffer,
        buffer_size);

    vkDestroyBuffer(in_device, staging_buffer, nullptr);
    vkFreeMemory(in_device, staging_buffer_memory, nullptr);

    out_mesh_info.index_count = index_count;
}

void wvk::mesh::Destroy(
    WVkMeshInfo & out_mesh_info,
    const VkDevice & in_device
    ) {
    
    vkDestroyBuffer(in_device,
                    out_mesh_info.index_buffer,
                    nullptr);
    
    vkFreeMemory(in_device,
                 out_mesh_info.index_buffer_memory,
                 nullptr);

    vkDestroyBuffer(in_device,
                    out_mesh_info.vertex_buffer,
                    nullptr);
    
    vkFreeMemory(in_device,
                 out_mesh_info.vertex_buffer_memory,
                 nullptr);

}
