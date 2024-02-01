#pragma once

#include "WCore.h"
#include <vulkan/vulkan.h>


struct WDeviceInfo{
    WId wid;
    VkPhysicalDevice vk_physical_device;
    VkDevice vk_device;
    VkSampleCountFlagBits msaa_samples = VK_SAMPLE_COUNT_1_BIT;
};
