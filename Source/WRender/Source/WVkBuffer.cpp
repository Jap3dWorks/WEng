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

}

WVkUBO wvk::buffer::CreateUBO(
    VkDeviceSize in_size,
    VkDevice in_device,
    VkPhysicalDevice in_physical_device
    ) {
    WVkUBO result;
    result.range = in_size;

    CreateVkBuffer(
        result.buffer,
        result.device_memory,
        in_device,
        in_physical_device,
        in_size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

    return result;
}

void * wvk::buffer::MapUBO(
    const WVkUBO & in_ubo,
    VkDevice in_device
    ) {
    void * ptr;

    vkMapMemory(
        in_device,
        in_ubo.device_memory,
        0,
        in_ubo.range,
        0,
        &ptr
        );
    
    return ptr;
}

void wvk::buffer::UnmapUBO(
    const WVkUBO & in_ubo,
    VkDevice in_device
    ) {
    vkUnmapMemory(
        in_device,
        in_ubo.device_memory
        );
}

void wvk::buffer::Destroy(
    WVkUBO & out_ubo_info,
    VkDevice in_device
    ) {

    vkDestroyBuffer(
        in_device,
        out_ubo_info.buffer,
        nullptr
        );

    vkFreeMemory(
        in_device,
        out_ubo_info.device_memory,
        nullptr
        );

    out_ubo_info.buffer = VK_NULL_HANDLE;
    out_ubo_info.device_memory = VK_NULL_HANDLE;
    out_ubo_info.range = 0;
}


