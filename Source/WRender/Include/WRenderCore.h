#pragma once

#include "WCore.h"
#include <vulkan/vulkan.h>


struct WDevice{
    WId id;
    VkDevice vk_device;
};
