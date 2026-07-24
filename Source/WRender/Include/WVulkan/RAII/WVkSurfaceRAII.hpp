#pragma once

#include "WVulkan/RAII/VkRAII.hpp"

#include <vulkan/vulkan_core.h>

namespace wdw { class WWindow; }

struct WVkSurfaceCreator {
    VkInstance instance_{VK_NULL_HANDLE};

    VkSurfaceKHR Create(wdw::WWindow * in_window);

    void Destroy(VkSurfaceKHR & in_value) {
        vkDestroySurfaceKHR(instance_, in_value, nullptr);        
    }
};

using WVkSurfaceRAII = wvk::raii::VkRAII<VkSurfaceKHR, WVkSurfaceCreator>;

