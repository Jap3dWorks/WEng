#pragma once

#include "WCore/WCoreMacros.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVkRAII/WVkRAII.hpp"

#include <vulkan/vulkan_core.h>

using WVkCommandBufferInfo =
    std::array<VkCommandBuffer, WENG_MAX_FRAMES_IN_FLIGHT>;

class WRENDER_API WVkRenderCommandPoolRAII
{

public:

    WVkRenderCommandPoolRAII(
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device,
        const VkSurfaceKHR & in_surface
        );

    WVkRenderCommandPoolRAII()=default;
    ~WVkRenderCommandPoolRAII()=default;

    WVkRenderCommandPoolRAII(WVkRenderCommandPoolRAII&&) noexcept=default;
    WVkRenderCommandPoolRAII & operator=(WVkRenderCommandPoolRAII&&) noexcept=default;

    WVkRenderCommandPoolRAII(const WVkRenderCommandPoolRAII &) = delete;
    WVkRenderCommandPoolRAII & operator=(const WVkRenderCommandPoolRAII &) = delete;

    WVkCommandBufferInfo CreateCommandBuffer();

    WNODISCARD const VkCommandPool & Value() const noexcept
    { return *command_pool_; }

private:

    struct WVkCommandPoolCreator {
        VkDevice device{VK_NULL_HANDLE};
        
        VkCommandPool Create(VkPhysicalDevice in_physical_device,
                             VkSurfaceKHR in_surface);

        void Destroy(VkCommandPool command_pool);
    };

    using WVkCommandPool = WVkRAII<VkCommandPool, WVkCommandPoolCreator>;

    WVkCommandPool command_pool_{};

    std::vector<WVkCommandBufferInfo> command_buffers_{};
    
};

