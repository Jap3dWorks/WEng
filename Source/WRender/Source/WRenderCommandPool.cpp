#include "WRenderCommandPool.h"
#include "WRenderCore.h"
#include "WVulkan.h"

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
	in_device_info,
	in_surface_info
	);
    
}

WRenderCommandPool::~WRenderCommandPool()
{

}
