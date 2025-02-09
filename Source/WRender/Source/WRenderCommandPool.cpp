#include "WRenderCommandPool.h"
#include "WRenderCore.h"
#include "WVulkan.h"
#include <vulkan/vulkan_core.h>

WRenderCommandPool::WRenderCommandPool(
    WCommandPoolInfo in_command_pool_info,
    const WDeviceInfo & in_device_info,
    const WSurfaceInfo & in_surface_info
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

WRenderCommandPool::~WRenderCommandPool()
{
    WVulkan::Destroy(command_pool_info_, device_info_);
}

WRenderCommandPool::WRenderCommandPool(WRenderCommandPool&& other) noexcept
{
    Move(std::move(other));
}

WRenderCommandPool& WRenderCommandPool::operator=(WRenderCommandPool&& other) noexcept
{
    Move(std::move(other));
    
    return *this;
}

void WRenderCommandPool::Move(WRenderCommandPool && out_other) noexcept
{
    if(command_pool_info_.vk_command_pool != VK_NULL_HANDLE)
    {
	WVulkan::Destroy(command_pool_info_, device_info_);
    }

    device_info_ = std::move(out_other.device_info_);
    command_pool_info_ = std::move(out_other.command_pool_info_);
}
