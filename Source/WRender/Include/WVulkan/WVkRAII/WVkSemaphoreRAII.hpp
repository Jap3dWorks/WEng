#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVk/WVkTypes.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include "WVulkan/WVkRAII/WVkRAII.hpp"

#include <vulkan/vulkan_core.h>

struct SemaphoreCreator {

    VkDevice device{VK_NULL_HANDLE};

    template<typename ...Args>
    WNODISCARD VkSemaphore Create(Args && ...) {

        VkSemaphore semaphore;

        VkSemaphoreCreateInfo create_info =
            wvk::types::VkSemaphoreCreateInfo();

        wvk::vulkan::ExecVkProcChecked(
            vkCreateSemaphore,
            "Fail while creating a Semaphore!",
            device,
            &create_info,
            nullptr,
            &semaphore
            );

        return semaphore;
    }

    void Destroy(VkSemaphore in_semaphore) {
        vkDestroySemaphore(
            device,
            in_semaphore,
            nullptr
            );
    }
};

using WVkSemaphoreRAII = WVkRAII<VkSemaphore, SemaphoreCreator>;
