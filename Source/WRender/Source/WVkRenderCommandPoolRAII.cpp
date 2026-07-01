#include "WVulkan/WVkRAII/WVkRenderCommandPoolRAII.hpp"
// #include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkTypes.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include <vulkan/vulkan_core.h>


WVkRenderCommandPoolRAII::WVkRenderCommandPoolRAII(
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device,
    const VkSurfaceKHR & in_surface
    ) :
    command_pool_({in_device}, in_physical_device, in_surface),
    command_buffers_() {}

WVkCommandBufferInfo WVkRenderCommandPoolRAII::CreateCommandBuffer() {
    command_buffers_.push_back({});

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = *command_pool_;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = static_cast<uint32_t>(
        command_buffers_.back().size()
        );

    wvk::vulkan::ExecVkProcChecked(
        vkAllocateCommandBuffers,
        "Failed to allocate command buffers!",
        command_pool_.Creator().device,
        &alloc_info,
        command_buffers_.back().data()
        );

    return command_buffers_.back();
}

VkCommandPool WVkRenderCommandPoolRAII::WVkCommandPoolCreator::Create(
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

void WVkRenderCommandPoolRAII::WVkCommandPoolCreator::Destroy(VkCommandPool command_pool) {
    vkDestroyCommandPool(
        device,
        command_pool,
        nullptr
        );
}
