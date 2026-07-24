#pragma once

#include <vulkan/vulkan_core.h>

namespace wvk::image {
    void CreateImage(
        VkImage& out_image,
        VkDeviceMemory& out_image_memory,
        const VkDevice& device, 
        const VkPhysicalDevice& physical_device,
        const uint32_t& width,
        const uint32_t& height, 
        const uint32_t& mip_levels, 
        const VkSampleCountFlagBits& samples,
        const VkFormat& format, 
        const VkImageTiling& tiling, 
        const VkImageUsageFlags& usage, 
        const VkMemoryPropertyFlags& properties
        );

    VkImageView CreateImageView(
        const VkImage& image, 
        const VkFormat& format, 
        const VkImageAspectFlags& aspect_flags, 
        const uint32_t& mip_levels,
        const VkDevice& device 
        );


    void TransitionImageLayout(
        const VkDevice & in_device, 
        const VkCommandPool & in_command_pool, 
        const VkQueue & in_graphics_queue, 
        const VkImage & in_image, 
        const VkImageLayout & in_old_layout, 
        const VkImageLayout & in_new_layout, 
        const uint32_t & in_mip_levels
        );

    void CopyBufferToImage(
        VkBuffer in_buffer,
        VkImage in_image,
        uint32_t in_width,
        uint32_t in_height,
        const VkDevice & in_device,
        const VkCommandPool & in_command_pool,
        const VkQueue & in_graphics_queue
        );

    void GenerateMipmaps(
        VkImage in_image,
        VkFormat in_image_format,
        int32_t in_tex_width,
        int32_t in_tex_height,
        int32_t in_mip_levels,
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device,
        const VkCommandPool & in_command_pool,
        const VkQueue & in_graphic_queue
        );


}
