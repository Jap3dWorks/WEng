#pragma once

#include "WVulkan/Vk/WVkTypes.hpp"
#include "WVulkan/RAII/VkRAII.hpp"

#include <vulkan/vulkan_core.h>

struct VkFenceCreator {
    VkDevice device{VK_NULL_HANDLE};

    VkFence Create() const {
        VkFence fence;

        VkFenceCreateInfo create_info =
            wvk::types::VkFenceCreateInfo();
        create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        vkCreateFence(
            device,
            &create_info,
            nullptr,
            &fence
            );

        return fence;
    }

    void Destroy(VkFence in_fence) const {
        vkDestroyFence(
            device,
            in_fence,
            nullptr
            );
    }
};

using WVkFenceRAII = wvk::raii::VkRAII<VkFence, VkFenceCreator>;

