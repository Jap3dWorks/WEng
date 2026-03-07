#pragma once

#include "WCore/WCoreMacros.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"

#include <vulkan/vulkan_core.h>

using WVkCommandBufferInfo =
    std::array<VkCommandBuffer, WENG_MAX_FRAMES_IN_FLIGHT>;

class WRENDER_API WVkRenderCommandPoolRAII
{

public:

    WVkRenderCommandPoolRAII();

    WVkRenderCommandPoolRAII(
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device,
        const VkSurfaceKHR & in_surface
        );

    ~WVkRenderCommandPoolRAII();

    WVkRenderCommandPoolRAII(WVkRenderCommandPoolRAII&& other) noexcept;
    WVkRenderCommandPoolRAII & operator=(WVkRenderCommandPoolRAII&& other) noexcept;

    WVkRenderCommandPoolRAII(const WVkRenderCommandPoolRAII & in_other) = delete;
    WVkRenderCommandPoolRAII & operator=(const WVkRenderCommandPoolRAII & in_other) = delete;

    WVkCommandBufferInfo CreateCommandBuffer();

    WNODISCARD const VkCommandPool & CommandPoolInfo() const noexcept
    { return command_pool_; }

private:

    void Destroy();

    VkDevice device_;
    VkCommandPool command_pool_;
    std::vector<WVkCommandBufferInfo> command_buffers_;
    
};

