#include "WVulkan/RAII/WVkCommandPoolRAII.hpp"
// #include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include <vulkan/vulkan_core.h>


WVkCommandPoolRAII::WVkCommandPoolRAII(
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device,
    const VkSurfaceKHR & in_surface
    ) :
    command_pool_({in_device}, in_physical_device, in_surface)
{}

VkCommandPool WVkCommandPoolRAII::WVkCommandPoolCreator::Create(
    VkPhysicalDevice in_physical_device,
    VkSurfaceKHR in_surface) {
        wvk::vulkan::QueueFamilyIndices queue_family_indices =
        wvk::vulkan::FindQueueFamilies(in_physical_device, in_surface);

    VkCommandPoolCreateInfo pool_info = wvk::types::VkCommandPoolCreateInfo();
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

    VkCommandPool result;

    wvk::vulkan::ExecVkProcChecked(
        vkCreateCommandPool,
        "Failed to create command pool!",
        device,
        &pool_info,
        nullptr,
        &result
        );

    return result;
}

void WVkCommandPoolRAII::WVkCommandPoolCreator::Destroy(VkCommandPool command_pool) {
    vkDestroyCommandPool(
        device,
        command_pool,
        nullptr
        );
}
