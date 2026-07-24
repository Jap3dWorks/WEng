#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"
#include "WVulkan/RAII/WVkSemaphoreRAII.hpp"
#include "WVulkan/RAII/WVkFenceRAII.hpp"

#include <vulkan/vulkan_core.h>
#include <vector>

template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
class WVkRenderSyncRAII {
private:

    struct Semaphores {
        WVkSemaphoreRAII image_available{};
        WVkSemaphoreRAII render_finished{};
    };

public:

    WVkRenderSyncRAII() = default;
    
    WVkRenderSyncRAII(const WVkRenderSyncRAII&) = delete;
    WVkRenderSyncRAII& operator=(const WVkRenderSyncRAII&)=delete;

    WVkRenderSyncRAII(WVkRenderSyncRAII &&) noexcept = default;
    WVkRenderSyncRAII& operator=(WVkRenderSyncRAII&& other) noexcept = default;

    virtual ~WVkRenderSyncRAII() = default;

    WVkRenderSyncRAII(VkDevice in_device,
                      std::size_t swapchain_images) {
        
        semaphores_ = std::vector<Semaphores>();
        semaphores_.reserve(swapchain_images);

        for (std::uint32_t i=0; i<swapchain_images; i++) {
            semaphores_.emplace_back(WVkSemaphoreRAII({in_device}),
                                     WVkSemaphoreRAII({in_device}));
        }

        for (std::uint32_t i=0; i<FramesInFlight; i++) {
            fences_[i] = {{in_device}};
        }
    }

public:

    const VkSemaphore & ImageAvailableSemaphore(std::uint8_t image_index) const {
        return semaphores_[image_index].image_available.Value();
    }

    const VkSemaphore & RenderFinishedSempahore(std::uint8_t image_index) const {
        return semaphores_[image_index].render_finished.Value();
    }

    const VkFence & Fence(std::uint8_t frame_index) const {
        return fences_[frame_index].Value();
    }

private:

    std::vector<Semaphores> semaphores_{};
    std::array<WVkFenceRAII, FramesInFlight> fences_{};

};
