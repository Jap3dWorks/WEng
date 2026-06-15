#include "WVulkan/WVk/WVkBuffer.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include <stdexcept>

void wvk::buffer::CreateVkBuffer(
    VkBuffer &out_buffer,
    VkDeviceMemory &out_buffer_memory,
    const VkDevice &device,
    const VkPhysicalDevice &physical_device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
    )
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &buffer_info, nullptr, &out_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create buffer!");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device, out_buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = wvk::vulkan::FindMemoryType(
        physical_device,
        mem_requirements.memoryTypeBits,
        properties);

    if (vkAllocateMemory(device, &alloc_info, nullptr, &out_buffer_memory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, out_buffer, out_buffer_memory, 0);

    // WFLOG("Create new Buffer {}", static_cast<void*>(out_buffer));
}

void wvk::buffer::CreateUBO(
    WVkUBOInfo & out_ubo_info,
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device)
{
    CreateVkBuffer(
        out_ubo_info.buffer,
        out_ubo_info.buffer_memory,
        in_device,
        in_physical_device,
        out_ubo_info.range,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
}

void wvk::buffer::MapUBO(
    WVkUBOInfo & out_uniform_buffer_info,
    const VkDevice & in_device
    )
{
    vkMapMemory(
        in_device,
        out_uniform_buffer_info.buffer_memory,
        0,
        out_uniform_buffer_info.range,
        0,
        &out_uniform_buffer_info.mapped_memory
        );
}

void wvk::buffer::UnmapUBO(
    WVkUBOInfo & out_uniform_buffer_info,
    const VkDevice & in_device
    ) {
    vkUnmapMemory(
        in_device,
        out_uniform_buffer_info.buffer_memory
        );

    out_uniform_buffer_info.mapped_memory = nullptr;
}


void wvk::buffer::Destroy(
    WVkUBOInfo & out_ubo_info,
    const VkDevice & in_device
    ) {

    if (out_ubo_info.mapped_memory) {
        vkUnmapMemory(in_device, out_ubo_info.buffer_memory);
        out_ubo_info.mapped_memory = nullptr;
    }
    
    vkDestroyBuffer(
        in_device,
        out_ubo_info.buffer,
        nullptr
        );

    vkFreeMemory(
        in_device,
        out_ubo_info.buffer_memory,
        nullptr
        );

    out_ubo_info.buffer = VK_NULL_HANDLE;
    out_ubo_info.buffer_memory = VK_NULL_HANDLE;
    out_ubo_info.range = 0;
}
