#include "WVulkan/WVkRenderCommandPool.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include <vulkan/vulkan_core.h>

WVkRenderCommandPool::WVkRenderCommandPool() :
    device_info_(),
    command_pool_info_(),
    command_buffers_()
{}

WVkRenderCommandPool::WVkRenderCommandPool(
    WVkCommandPoolInfo in_command_pool_info,
    const WVkDeviceInfo & in_device_info,
    const WVkSurfaceInfo & in_surface_info
    ) :
    device_info_(in_device_info),
    command_pool_info_(in_command_pool_info),
    command_buffers_()
{
    WVulkan::Create(
        command_pool_info_,
        device_info_,
        in_surface_info
        );
}

WVkRenderCommandPool::~WVkRenderCommandPool()
{
    Clear();
}

WVkRenderCommandPool::WVkRenderCommandPool(WVkRenderCommandPool && other) noexcept :
    device_info_(std::move(other.device_info_)),
    command_pool_info_(std::move(other.command_pool_info_)),
    command_buffers_(std::move(other.command_buffers_))
{
    other.device_info_={};
    other.command_pool_info_={};
    other.command_buffers_={};
}

WVkRenderCommandPool& WVkRenderCommandPool::operator=(WVkRenderCommandPool && other) noexcept
{
    if (this != &other) {

        Clear();
        device_info_ = std::move(other.device_info_);
        command_pool_info_ = std::move(other.command_pool_info_);
        command_buffers_ = std::move(other.command_buffers_);

        other.device_info_={};
        other.command_pool_info_={};
        other.command_buffers_={};
        
    }
    
    return *this;
}

WVkCommandBufferInfo WVkRenderCommandPool::CreateCommandBuffer()
{
    command_buffers_.push_back({});
    WVulkan::Create(command_buffers_.back(), device_info_, command_pool_info_);

    return command_buffers_.back();
}

void WVkRenderCommandPool::Clear()
{
    if(command_pool_info_.vk_command_pool != VK_NULL_HANDLE) {
        WVulkan::Destroy(command_pool_info_, device_info_);
        command_pool_info_ = {};
    }

    device_info_ = {};
    command_buffers_.clear();
}
