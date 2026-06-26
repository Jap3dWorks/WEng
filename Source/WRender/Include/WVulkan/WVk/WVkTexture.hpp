#pragma once

#include "WCoreTypes/WTexture.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WAssets/WTextureAsset.hpp"
#include <vulkan/vulkan_core.h>

namespace wvk::texture {

    /**
     * Uploads in vram a ready to render texture.
     * Also create the image, imageview and sampler.
     * Resulting vulkan objects are stored in out_texture_info out param.
     */
    void CreateTexture(
        WVkTextureInfo & out_texture_info, 
        const WTextureAsset & texture_struct,
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device,
        const VkQueue & in_graphics_queue,
        const VkCommandPool &in_command_pool
        );

    /**
     * Destroys the vulkan texture objects.
     * also frees in use texture memory.
     */
    void DestroyTexture(
        WVkTextureInfo & out_texture_info,
        const VkDevice & in_device
        );

    VkSampler CreateTextureSampler(
        const VkDevice& device, 
        const VkPhysicalDevice& physical_device,
        const uint32_t& mip_levels
        );
    
    void DestroyVkSampler(
        VkSampler & out_sampler,
        const VkDevice & in_device_info
        );

    VkFormat ToVkFormat(wct::texture::ETextureFormat in_texture_format);

}


