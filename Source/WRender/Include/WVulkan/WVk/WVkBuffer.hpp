#pragma once

#include "WVulkan/WVulkanStructs.hpp"

namespace wvk::buffer {

    void CreateVkBuffer(
        VkBuffer & out_buffer, 
        VkDeviceMemory & out_buffer_memory,
        const VkDevice& device,
        const VkPhysicalDevice& physical_device,
        VkDeviceSize size, 
        VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties
        );

    void CreateUBO(
        WVkUBOInfo & out_uniform_buffer_info,
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device
        );

    void MapUBO(
        WVkUBOInfo & out_uniform_buffer_info,
        const VkDevice & in_device
        );

    void UnmapUBO(
        WVkUBOInfo & out_uniform_buffer_info,
        const VkDevice & in_device
        );

    void Destroy(
        WVkUBOInfo & out_ubo_info,
        const VkDevice & in_device
        );

    inline void CopyVkBuffer(
        const VkDevice & device,
        const VkCommandPool & command_pool,
        const VkQueue & graphics_queue,
        const VkBuffer & src_buffer,
        const VkBuffer & dst_buffer,
        const VkDeviceSize & size
        ) {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = command_pool;
        alloc_info.commandBufferCount = 1;
        alloc_info.pNext = nullptr;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);

        VkBufferCopy copy_region{};
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphics_queue);

        vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
    }

        
    inline bool UpdateUBO(
        WVkUBOInfo & in_ubo_info,
        const void * in_data,
        const std::size_t & in_size,
        const std::size_t & in_offset=0
        ) {

        char * mapped_mem = reinterpret_cast<char*>(in_ubo_info.mapped_memory);
        memcpy((mapped_mem + in_offset),
               in_data,
               in_size               
            );

        return true;
    }

    inline bool UpdateUBOModel(
        WVkUBOInfo & uniform_buffer_object_info_,
        const glm::mat4 & model
        ) {
        wct::render::WGraphicsUBO ubo{};

        ubo.model = model;
        
        memcpy(uniform_buffer_object_info_.mapped_memory,
               &ubo,
               sizeof(wct::render::WGraphicsUBO));

        return true;
    }

    inline bool UpdateUBOModel(
        WVkUBOInfo & uniform_buffer_object_info_,
        const wct::render::WGraphicsUBO & in_ubo_model_struct
        ) {
        
        memcpy(uniform_buffer_object_info_.mapped_memory,
               &in_ubo_model_struct,
               sizeof(wct::render::WGraphicsUBO));

        return true;
    }
    
}
