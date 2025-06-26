#pragma once

#include "WCore/CoreMacros.h"
#include "WCore/WCore.h"
#include "WVulkan/WVkRenderCore.h"

class WRENDER_API WVkRenderCommandPool
{

public:

    WVkRenderCommandPool();

    WVkRenderCommandPool(
        WVkCommandPoolInfo in_command_pool_info,
        const WVkDeviceInfo & in_device_info,
        const WVkSurfaceInfo & in_surface_info
	);

    ~WVkRenderCommandPool();

    WVkRenderCommandPool(WVkRenderCommandPool&& other) noexcept;
    WVkRenderCommandPool & operator=(WVkRenderCommandPool&& other) noexcept;

    WVkRenderCommandPool(const WVkRenderCommandPool & in_other) = delete;
    WVkRenderCommandPool & operator=(const WVkRenderCommandPool & in_other) = delete;

    WVkCommandBufferInfo CreateCommandBuffer();

    WNODISCARD const WVkCommandPoolInfo & CommandPoolInfo() const noexcept
    { return command_pool_info_; }

    void Destroy();

private:

    void Move(WVkRenderCommandPool&& out_other) noexcept;

    WVkCommandPoolInfo command_pool_info_;
    WVkDeviceInfo device_info_;

    std::vector<WVkCommandBufferInfo> command_buffers_;
    
};

