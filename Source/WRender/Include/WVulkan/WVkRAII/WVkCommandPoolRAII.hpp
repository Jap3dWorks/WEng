#pragma once

#include "WCore/WCoreMacros.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVkRAII/WVkRAII.hpp"
#include "WVulkan/WVk/WVulkan.hpp"

#include <vulkan/vulkan_core.h>

class WRENDER_API WVkCommandPoolRAII
{

public:

    template<std::uint8_t Count=WENG_MAX_FRAMES_IN_FLIGHT>
    using CommandBuffers = std::array<VkCommandBuffer, Count>;

    WVkCommandPoolRAII(
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device,
        const VkSurfaceKHR & in_surface
        );

    WVkCommandPoolRAII()=default;
    ~WVkCommandPoolRAII()=default;
    WVkCommandPoolRAII(WVkCommandPoolRAII&&) noexcept=default;
    WVkCommandPoolRAII & operator=(WVkCommandPoolRAII&&) noexcept=default;
    WVkCommandPoolRAII(const WVkCommandPoolRAII &) = delete;
    WVkCommandPoolRAII & operator=(const WVkCommandPoolRAII &) = delete;


    template<std::uint8_t Count=WENG_MAX_FRAMES_IN_FLIGHT>
    CommandBuffers<Count> CreateCommandBuffers() {
        CommandBuffers<> result{};

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = *command_pool_;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = static_cast<uint32_t>(
            result.size()
            );

        wvk::vulkan::ExecVkProcChecked(
            vkAllocateCommandBuffers,
            "Failed to allocate command buffers!",
            command_pool_.Creator().device,
            &alloc_info,
            result.data()
            );

        return result;
    }

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
    
};

