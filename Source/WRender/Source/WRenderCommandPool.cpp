#include "WVulkan/WVkRenderCommandPool.h"
#include "WVulkan/WVkRenderCore.hpp"
#include "WVulkan/WVulkan.hpp"
#include <vulkan/vulkan_core.h>


WVkRenderCommandPool::WVkRenderCommandPool() :
    device_info_(), command_pool_info_()
{
    
}

WVkRenderCommandPool::WVkRenderCommandPool(
    WVkCommandPoolInfo in_command_pool_info,
    const WVkDeviceInfo & in_device_info,
    const WVkSurfaceInfo & in_surface_info
    ) :
    command_pool_info_(in_command_pool_info),
    device_info_(in_device_info)
{
    WVulkan::Create(
        command_pool_info_,
        device_info_,
        in_surface_info
	);
}

WVkRenderCommandPool::~WVkRenderCommandPool()
{
    Destroy();
}

WVkRenderCommandPool::WVkRenderCommandPool(WVkRenderCommandPool && other) noexcept
{
    Move(std::move(other));
}

WVkRenderCommandPool& WVkRenderCommandPool::operator=(WVkRenderCommandPool && other) noexcept
{
    Move(std::move(other));
    
    return *this;
}

void WVkRenderCommandPool::Move(WVkRenderCommandPool && out_other) noexcept
{
    if(command_pool_info_.vk_command_pool != VK_NULL_HANDLE)
    {
        WVulkan::Destroy(command_pool_info_, device_info_);
    }

    device_info_ = std::move(out_other.device_info_);
    command_pool_info_ = std::move(out_other.command_pool_info_);

    out_other.device_info_ = {};
    out_other.command_pool_info_ = {};
}

WVkCommandBufferInfo WVkRenderCommandPool::CreateCommandBuffer()
{
    command_buffers_.push_back({});
    WVulkan::Create(command_buffers_.back(), device_info_, command_pool_info_);

    return command_buffers_.back();
}

void WVkRenderCommandPool::Destroy()
{
    if(command_pool_info_.vk_command_pool != VK_NULL_HANDLE) {
        WVulkan::Destroy(command_pool_info_, device_info_);        
        command_pool_info_ = {};
    }

    device_info_ = {};
}
