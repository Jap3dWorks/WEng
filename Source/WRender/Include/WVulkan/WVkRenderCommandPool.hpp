#pragma once

#include "WCore/WCoreMacros.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"

#include <vulkan/vulkan_core.h>

class WRENDER_API WVkRenderCommandPool
{

public:

    WVkRenderCommandPool();

    WVkRenderCommandPool(
        WVkCommandPoolInfo in_command_pool_info,
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device,
        const VkSurfaceKHR & in_surface
	);

    ~WVkRenderCommandPool();

    WVkRenderCommandPool(WVkRenderCommandPool&& other) noexcept;
    WVkRenderCommandPool & operator=(WVkRenderCommandPool&& other) noexcept;

    WVkRenderCommandPool(const WVkRenderCommandPool & in_other) = delete;
    WVkRenderCommandPool & operator=(const WVkRenderCommandPool & in_other) = delete;

    WVkCommandBufferInfo CreateCommandBuffer();

    WNODISCARD const VkCommandPool & CommandPoolInfo() const noexcept
    { return command_pool_; }

private:

    void Destroy();

    VkDevice device_;
    VkCommandPool command_pool_;
    std::vector<WVkCommandBufferInfo> command_buffers_;
    
};

