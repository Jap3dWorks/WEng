#pragma once

#include "WCore.h"
#include <vulkan/vulkan.h>


struct WDeviceInfo{
    WId wid;
    VkPhysicalDevice vk_physical_device= VK_NULL_HANDLE;
    VkSampleCountFlagBits msaa_samples = VK_SAMPLE_COUNT_1_BIT;
    std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkQueue vk_graphics_queue = nullptr;
    VkQueue vk_present_queue = nullptr;

    VkDevice vk_device = nullptr;
};
