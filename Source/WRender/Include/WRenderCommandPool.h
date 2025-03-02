#pragma once

#include "WCore/CoreMacros.h"
#include "WCore/WCore.h"
#include "WRenderCore.h"

class WRENDER_API WRenderCommandPool
{

public:

    WRenderCommandPool();

    WRenderCommandPool(
        WCommandPoolInfo in_command_pool_info,
        const WDeviceInfo & in_device_info,
        const WSurfaceInfo & in_surface_info
	);

    ~WRenderCommandPool();

    WRenderCommandPool(WRenderCommandPool&& other) noexcept;
    WRenderCommandPool & operator=(WRenderCommandPool&& other) noexcept;

    WRenderCommandPool(const WRenderCommandPool & in_other) = delete;
    WRenderCommandPool & operator=(const WRenderCommandPool & in_other) = delete;

    WCommandBufferInfo CreateCommandBuffer();

    WNODISCARD const WCommandPoolInfo & CommandPoolInfo() const noexcept
    { return command_pool_info_; }

private:

    void Move(WRenderCommandPool&& out_other) noexcept;

    WCommandPoolInfo command_pool_info_;
    WDeviceInfo device_info_;

    std::vector<WCommandBufferInfo> command_buffers_;
    
};

