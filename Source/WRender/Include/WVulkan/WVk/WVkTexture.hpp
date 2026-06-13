#pragma once

#include "WStructs/WTextureStructs.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include <vulkan/vulkan_core.h>

namespace wvk::texture {

    void CreateTexture(
        WVkTextureInfo& out_texture_info, 
        const WTextureStruct& texture_struct,
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
}


