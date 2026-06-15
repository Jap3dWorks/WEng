#include "WVulkan/WVk/WVkSwapChain.hpp"

#include <algorithm>


wvk::swap_chain::SwapChainSupportDetails wvk::swap_chain::QuerySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
    if (format_count != 0)
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
    if (present_mode_count != 0)
    {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
    }

    return details;
}

VkSurfaceFormatKHR wvk::swap_chain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats)
{
    for (const auto &available_format : available_formats)
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return available_format;
        }
    }
    return available_formats[0];
}

VkPresentModeKHR wvk::swap_chain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> & available_present_modes)
{
    for (const auto &available_present_mode : available_present_modes)
    {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return available_present_mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D wvk::swap_chain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & in_capabilities,
                                     const std::uint32_t & in_width,
                                     const std::uint32_t & in_height)
{
    if (in_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return in_capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actual_extent = {
            in_width,
            in_height
        };

        actual_extent.width = std::clamp(
            actual_extent.width,
            in_capabilities.minImageExtent.width,
            in_capabilities.maxImageExtent.width
            );
        actual_extent.height = std::clamp(
            actual_extent.height,
            in_capabilities.minImageExtent.height,
            in_capabilities.maxImageExtent.height
            );

        return actual_extent;
    }
}
