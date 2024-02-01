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

    VkDevice vk_device = nullptr;
};
