#pragma once

#include "WCoreTypes/WTexture.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include <vulkan/vulkan_core.h>

namespace wvk::texture {

    void CreateTexture(
        WVkTextureInfo& out_texture_info, 
        const wtp::texture::WTexture& texture_struct,
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device,
        const VkQueue & in_graphics_queue,
        const VkCommandPool &in_command_pool
        );

    VkSampler CreateTextureSampler(
        const VkDevice& device, 
        const VkPhysicalDevice& physical_device,
        const uint32_t& mip_levels
        );

    void DestroyTexture(
        WVkTextureInfo & out_texture_info,
        const VkDevice & in_device
        );

    
    void DestroyVkSampler(
        VkSampler & out_sampler,
        const VkDevice & in_device_info
        );

    VkFormat ToVkFormat(wtp::texture::ETextureFormat in_texture_format );

    wtp::texture::WTexture AddRGBAPadding(
        const wtp::texture::WTexture & in_texture_struct
        );

}


