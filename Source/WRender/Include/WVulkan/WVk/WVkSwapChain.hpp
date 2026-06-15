#pragma once

#include "WVulkan/WVulkanStructs.hpp"

namespace wvk::swap_chain {

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats);

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes);

    VkExtent2D ChooseSwapExtent(
        const VkSurfaceCapabilitiesKHR &capabilities,
        const std::uint32_t & in_width,
        const std::uint32_t & in_height
        );


    // void Destroy(WVkSwapChainInfo & swap_chain_info,
    //              const VkDevice & device_info);

}
