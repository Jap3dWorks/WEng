#pragma once

#include "WStructs/WTextureStructs.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include <vulkan/vulkan_core.h>

namespace weng::vk::texture {

    void CreateTexture(
        WVkTextureInfo& out_texture_info, 
        const WTextureStruct& texture_struct,
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device,
        const VkQueue & in_graphics_queue,
        const VkCommandPool &in_command_pool
        );

}
