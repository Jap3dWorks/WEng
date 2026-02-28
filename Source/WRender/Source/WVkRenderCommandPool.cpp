#include "WVulkan/WVkRenderCommandPool.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include <vulkan/vulkan_core.h>

WVkRenderCommandPool::WVkRenderCommandPool() :
    device_(VK_NULL_HANDLE),
    command_pool_info_(),
    command_buffers_()
{}

WVkRenderCommandPool::WVkRenderCommandPool(
    WVkCommandPoolInfo in_command_pool_info,
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device,
    const VkSurfaceKHR & in_surface
    ) :
    device_(in_device),
    command_pool_info_(in_command_pool_info),
    command_buffers_()
{
    WVulkan::QueueFamilyIndices queue_family_indices =
        WVulkan::FindQueueFamilies(in_physical_device, in_surface);

    VkCommandPoolCreateInfo pool_info{};

    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

    WVulkan::ExecVkProcChecked(
        vkCreateCommandPool,
        "Failed to create command pool!",
        device_,
        &pool_info,
        nullptr,
        &command_pool_
        );
}

WVkRenderCommandPool::~WVkRenderCommandPool()
{
    Destroy();
}

WVkRenderCommandPool::WVkRenderCommandPool(WVkRenderCommandPool && other) noexcept :
    device_(std::move(other.device_)),
    command_pool_(std::move(other.command_pool_)),
    command_buffers_(std::move(other.command_buffers_))
{
    other.device_ = VK_NULL_HANDLE;
    other.command_pool_ = VK_NULL_HANDLE;
    other.command_buffers_={};
}

WVkRenderCommandPool& WVkRenderCommandPool::operator=(WVkRenderCommandPool && other) noexcept
{
    if (this != &other) {
        Destroy();

        device_ = std::move(other.device_);
        command_pool_ = std::move(other.command_pool_);
        command_buffers_ = std::move(other.command_buffers_);

        other.device_ = VK_NULL_HANDLE;
        other.command_pool_ = VK_NULL_HANDLE;
        other.command_buffers_={};
    }
    
    return *this;
}

WVkCommandBufferInfo WVkRenderCommandPool::CreateCommandBuffer() {
    command_buffers_.push_back({});

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool_;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = static_cast<uint32_t>(
        command_buffers_.back().command_buffers.size()
        );

    WVulkan::ExecVkProcChecked(
        vkAllocateCommandBuffers,
        "Failed to allocate command buffers!",
        device_,
        &alloc_info,
        command_buffers_.back().command_buffers.data()
        );

    return command_buffers_.back();
}

void WVkRenderCommandPool::Destroy()
{
    if (device_ != VK_NULL_HANDLE) {
        WFLOG("Destroy Command Pool");
        
        vkDestroyCommandPool(
            device_,
            command_pool_,
            nullptr
            );

        command_pool_ = VK_NULL_HANDLE;
        device_ = VK_NULL_HANDLE;
        command_buffers_.clear();
    }
}
