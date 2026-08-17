#pragma once

#include "WCore/WCoreMacros.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/RAII/VkRAII.hpp"
#include "WVulkan/Vk/WVulkan.hpp"

#include <vulkan/vulkan_core.h>

namespace wvk::raii {
    
    class WRENDER_API CommandPool
    {

    public:

    template<std::uint8_t Count=WVK_MAX_FRAMES_IN_FLIGHT>
    using CommandBuffers = std::array<VkCommandBuffer, Count>;

    CommandPool(
        VkDevice in_device,
        VkPhysicalDevice in_physical_device,
        VkSurfaceKHR in_surface
        );

    CommandPool()=default;
    ~CommandPool()=default;
    CommandPool(CommandPool&&) noexcept=default;
    CommandPool & operator=(CommandPool&&) noexcept=default;
    CommandPool(const CommandPool &) = delete;
    CommandPool & operator=(const CommandPool &) = delete;


    template<std::uint8_t Count=WVK_MAX_FRAMES_IN_FLIGHT>
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

    using WVkCommandPool = VkRAII<VkCommandPool, WVkCommandPoolCreator>;

    WVkCommandPool command_pool_{};
    
    };

}
