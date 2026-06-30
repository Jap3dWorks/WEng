#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVk/WVkTypes.hpp"
#include "WVulkan/WVk/WVulkan.hpp"

#include <vulkan/vulkan_core.h>

class WVkSemaphoreRAII {

public:
    
    WVkSemaphoreRAII() = default;

    WVkSemaphoreRAII(const WVkSemaphoreRAII&) = delete;
    WVkSemaphoreRAII & operator=(const WVkSemaphoreRAII&) = delete;

    WVkSemaphoreRAII(WVkSemaphoreRAII && other) noexcept :
        device_(other.device_),
        semaphore_(other.semaphore_) {}

    WVkSemaphoreRAII & operator=(WVkSemaphoreRAII && other) noexcept {
        if (this != &other) {
            Destroy();

            device_ = other.device_;
            semaphore_ = other.semaphore_;

            other.device_ = nullptr;
            other.semaphore_ = nullptr;
        }
        return *this;
    }

    virtual ~WVkSemaphoreRAII() = default;

    WVkSemaphoreRAII(VkDevice in_device) :
        device_(in_device) {

        VkSemaphoreCreateInfo create_info =
            wvk::types::VkSemaphoreCreateInfo();

        wvk::vulkan::ExecVkProcChecked(
            vkCreateSemaphore,
            "Fail while creating a Semaphore!",
            device_,
            &create_info,
            nullptr,
            &semaphore_
            );
    }

    WNODISCARD VkSemaphore Semaphore() const {
        return semaphore_;
    }

private:

    void Destroy() {
        if (device_ != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                device_,
                semaphore_,
                nullptr
                );

            device_ = VK_NULL_HANDLE;
        }
    }

private:    

    VkDevice device_{VK_NULL_HANDLE};
    VkSemaphore semaphore_{VK_NULL_HANDLE};

};
