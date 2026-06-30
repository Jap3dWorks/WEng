#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVk/WVkTypes.hpp"

#include <vulkan/vulkan_core.h>
#include <vector>

template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkRenderSyncRAII {
private:

    struct SyncSemaphores {
        VkSemaphore image_available{VK_NULL_HANDLE};
        VkSemaphore render_finished{VK_NULL_HANDLE};
    };


public:

    WVkRenderSyncRAII() = default;
    
    WVkRenderSyncRAII(const WVkRenderSyncRAII&) = delete;
    WVkRenderSyncRAII& operator=(const WVkRenderSyncRAII&) =delete;

    WVkRenderSyncRAII(WVkRenderSyncRAII&& other) :
        device_(other.device_),
        sync_semaphores_(std::move(other.sync_semaphores_)),
        sync_fences_(std::move(other.sync_fences_)) {
        other.device_ = VK_NULL_HANDLE;
    }
    
    WVkRenderSyncRAII& operator=(WVkRenderSyncRAII&& other) {
        if(this != &other) {
            Destroy();

            device_ = other.device_;
            sync_semaphores_ = std::move(other.sync_semaphores_);
            sync_fences_ = std::move(other.sync_fences_);

            other.device_ = VK_NULL_HANDLE;
        }
        return *this;
    }

    virtual ~WVkRenderSyncRAII() {
        Destroy();
    }

    WVkRenderSyncRAII(VkDevice in_device,
                      std::uint8_t swapchain_images) :
        device_(in_device) {
        Initialize(device_, swapchain_images);
    }

public:

    

private:

    void Initialize(VkDevice in_device, std::uint8_t in_images) {

        // TODO WVkSemaphoreRAII and WVkFenceRAII

        sync_semaphores_ = CreateSemaphores(in_device, in_images);
    
        sync_fences_ = CreateSyncFences(in_device);
        
    }

    void Destroy() {
        if (device_ != VK_NULL_HANDLE) {

            for(auto & semaphores : sync_semaphores_) {
                vkDestroySemaphore(device_, semaphores.image_available, nullptr);
                vkDestroySemaphore(device_, semaphores.render_finished, nullptr);
            }

            for(auto& fence : sync_fences_) {
                vkDestroyFence(device_, fence, nullptr);
            }

            device_ = VK_NULL_HANDLE;
        }
    }

    WNODISCARD std::vector<SyncSemaphores> CreateSemaphores(
        VkDevice in_device,
        std::uint8_t in_images
        ) const {
        
        std::vector<SyncSemaphores> result(in_images);

        VkSemaphoreCreateInfo create_info = wvk::types::VkSemaphoreCreateInfo();

        for(std::size_t i=0; i<in_images; i++) {
            SyncSemaphores itm{};

            wvk::vulkan::ExecVkProcChecked(
                vkCreateSemaphore,
                "Failed Creating a Semaphore!",
                in_device, &create_info, nullptr, &itm.image_available
                );

            wvk::vulkan::ExecVkProcChecked(
                vkCreateSemaphore,
                "Failed Creating a Semaphore!",
                in_device, &create_info, nullptr, &itm.render_finished
                );

            result[i] = itm;
        }

        return result;
    }

    inline std::array<VkFence, FramesInFlight> CreateSyncFences(const VkDevice & in_device) {
        VkFenceCreateInfo create_info = wvk::types::VkFenceCreateInfo();
        create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        std::array<VkFence, FramesInFlight> result;

        for(std::size_t i=0; i<FramesInFlight; i++) {
            VkFence itm;
            wvk::vulkan::ExecVkProcChecked(
                vkCreateFence,
                "Failed creating a Fence!",
                in_device,
                &create_info,
                nullptr,
                &itm
                );

            result[i] = itm;
        }

        return result;
    }
    

private:

    VkDevice device_{VK_NULL_HANDLE};

    std::vector<SyncSemaphores> sync_semaphores_{};
    std::array<VkFence, FramesInFlight> sync_fences_{};

};
