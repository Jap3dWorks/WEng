#pragma once

#include "WCore/WCore.h"
#include "WRenderCore.h"

class WRENDER_API WRenderCommandPool
{

public:

    WRenderCommandPool(
	WCommandPoolInfo in_command_pool_info,
	const WDeviceInfo & in_device_info,
	const WSurfaceInfo & in_surface_info
	);

    ~WRenderCommandPool();

private:

    WCommandPoolInfo command_pool_info_;
    WDeviceInfo device_info_;
    
};

