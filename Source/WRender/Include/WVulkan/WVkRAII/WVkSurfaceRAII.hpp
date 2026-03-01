#pragma once

#include "WVulkan/WVulkan.hpp"

#include <vulkan/vulkan_core.h>

struct GLFWwindow;

class WVkSurfaceRAII {
public:

    WVkSurfaceRAII() = default;

    WVkSurfaceRAII(const VkInstance & in_instance,
                   GLFWwindow * in_window);

    ~WVkSurfaceRAII() {
        Destroy();
    }

    WVkSurfaceRAII(const WVkSurfaceRAII & other) = delete;
    WVkSurfaceRAII & operator=(const WVkSurfaceRAII & other) = delete;

    WVkSurfaceRAII(WVkSurfaceRAII && other) noexcept :
        vk_instance_(std::move(other.vk_instance_)),
        vk_surface_(std::move(other.vk_surface_))
        {
            other.vk_instance_ = VK_NULL_HANDLE;
            other.vk_surface_ = VK_NULL_HANDLE;
        }

    WVkSurfaceRAII & operator=(WVkSurfaceRAII && other) {
        if (this != &other) {

            Destroy();

            vk_instance_ = std::move(other.vk_instance_);
            vk_surface_ = std::move(other.vk_surface_);

            other.vk_instance_ = VK_NULL_HANDLE;
            other.vk_surface_ = VK_NULL_HANDLE;
        }

        return *this;
    }

public:

    VkSurfaceKHR Surface() const noexcept {
        return vk_surface_;
    }

private:

    void Destroy() {
        if (vk_instance_ != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(vk_instance_, vk_surface_, nullptr);
            vk_surface_=VK_NULL_HANDLE;
            vk_instance_=VK_NULL_HANDLE;
        }
    }

    VkInstance vk_instance_{VK_NULL_HANDLE};
    VkSurfaceKHR vk_surface_{VK_NULL_HANDLE};

};
