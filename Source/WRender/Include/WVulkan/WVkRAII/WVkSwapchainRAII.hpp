#pragma once

#include "WVulkan/WVulkan.hpp"

#include <stdatomic.h>
#include <vulkan/vulkan_core.h>
#include <cstdint>

class WVkSwapchainRAII {
public:

    WVkSwapchainRAII() = default;

    WVkSwapchainRAII(const VkDevice & in_device,
                     const VkPhysicalDevice & in_physical_device,
                     const VkSurfaceKHR & in_surface,
                     const std::uint32_t & in_width,
                     const std::uint32_t & in_height) :
        device_(in_device) {
        WVulkan::SwapChainSupportDetails swap_chain_support = WVulkan::QuerySwapChainSupport(
            in_physical_device,
            in_surface
            );

        VkSurfaceFormatKHR surface_format =
            WVulkan::ChooseSwapSurfaceFormat(swap_chain_support.formats);
        VkPresentModeKHR present_mode =
            WVulkan::ChooseSwapPresentMode(swap_chain_support.present_modes);
        VkExtent2D extent =
            WVulkan::ChooseSwapExtent(swap_chain_support.capabilities,
                             in_width,
                             in_height);

        uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
    
        if (swap_chain_support.capabilities.maxImageCount > 0 &&
            image_count > swap_chain_support.capabilities.maxImageCount)
        {
            image_count = swap_chain_support.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR create_info =
            WVulkan::VkStructs::CreateVkSwapchainCreateInfoKHR();
    
        create_info.surface = in_surface;
        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent; // Swap chain image resolution
        create_info.imageArrayLayers = 1;
        // VK_IMAGE_USAGE_TRANSFER_DST_BIT for post-processing
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        WVulkan::QueueFamilyIndices indices =
            WVulkan::FindQueueFamilies(in_physical_device, in_surface);

        uint32_t queue_family_indices[] = {
            indices.graphics_family.value(),
            indices.present_family.value()
        };

        if (indices.graphics_family != indices.present_family)
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queue_family_indices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        create_info.preTransform = swap_chain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;

        WVulkan::ExecVkProcChecked(vkCreateSwapchainKHR,
                                   "Failed to create swap chain!",
                                   in_device,
                                   &create_info,
                                   nullptr,
                                   &swapchain_);

        // Retrieve Swap Chain Images
        vkGetSwapchainImagesKHR(in_device,
                                swapchain_,
                                &image_count,
                                nullptr);
        
        images_.resize(image_count);
        vkGetSwapchainImagesKHR(
            device_,
            swapchain_,
            &image_count,
            images_.data());

        // Save Swap Chain Image Format
        format_ = surface_format.format;
        extent_ = extent;

        // Swap chain image views
        views_.resize(
            images_.size()
            );

        for (size_t i = 0; i < images_.size(); i++)
        {
            views_[i] = WVulkan::CreateImageView(images_[i],
                                                 format_,
                                                 VK_IMAGE_ASPECT_COLOR_BIT,
                                                 1,
                                                 in_device);
        }

    }

    ~WVkSwapchainRAII() {
        Destroy();
    }

    WVkSwapchainRAII(const WVkSwapchainRAII & other) = delete;
    WVkSwapchainRAII & operator=(const WVkSwapchainRAII & other) = delete;

    WVkSwapchainRAII(WVkSwapchainRAII && other) noexcept :
        device_(std::move(other.device_)),
        swapchain_(std::move(other.swapchain_)),
        format_(std::move(other.format_)),
        extent_(std::move(other.extent_)),
        images_(std::move(other.images_)),
        views_(std::move(other.views_)),
        memory_(std::move(other.memory_))
        {
            other.device_ = VK_NULL_HANDLE;
            other.swapchain_ = VK_NULL_HANDLE;
            other.format_ = {};
            other.extent_ = {};
    }

    WVkSwapchainRAII & operator=(WVkSwapchainRAII && other) noexcept {
        if (this != &other) {
            Destroy();

            device_= std::move(other.device_);
            swapchain_= std::move(other.swapchain_);
            format_= std::move(other.format_);
            extent_= std::move(other.extent_);
            images_= std::move(other.images_);
            views_= std::move(other.views_);
            memory_= std::move(other.memory_);
            
            other.device_ = VK_NULL_HANDLE;
            other.swapchain_ = VK_NULL_HANDLE;
            other.format_ = {};
            other.extent_ = {};

        }
        return *this;
    }

    constexpr const VkSwapchainKHR & Swapchain() const {
        return swapchain_;
    }

    constexpr VkFormat Format() const {
        return format_;
    }

    constexpr VkExtent2D Extent() const {
        return extent_;
    }

    constexpr const std::vector<VkImage> & Images() const {
        return images_;
    }

    constexpr const std::vector<VkImageView> & Views() const {
        return views_;
    }

    constexpr const std::vector<VkDeviceMemory> & Memory() const {
        return memory_;
    }

private:

    void Destroy() {
        if (device_ != VK_NULL_HANDLE) {
            for (auto & view: views_) {
                vkDestroyImageView(device_, view, nullptr);
            }

            for (auto & mem : memory_) {
                vkFreeMemory(device_, mem, nullptr);
            }

            vkDestroySwapchainKHR(device_, swapchain_, nullptr);
    
            swapchain_ = VK_NULL_HANDLE;
            images_.clear();
            views_.clear();
            memory_.clear();
        }
    }

    VkDevice device_{VK_NULL_HANDLE};
    
    VkSwapchainKHR swapchain_{VK_NULL_HANDLE};
    VkFormat format_{};
    VkExtent2D extent_{};

    std::vector<VkImage> images_{};
    std::vector<VkImageView> views_{};
    std::vector<VkDeviceMemory> memory_{};

};
