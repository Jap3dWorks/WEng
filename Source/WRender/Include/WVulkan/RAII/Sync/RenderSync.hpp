#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"
#include "WVulkan/RAII/Sync/Semaphore.hpp"
#include "WVulkan/RAII/Sync/Fence.hpp"

#include <vulkan/vulkan_core.h>
#include <vector>

namespace wvk::raii::sync {

    template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
    class RenderSync {
    private:

        struct Semaphores {
            Semaphore image_available{};
            Semaphore render_finished{};
        };

    public:

        RenderSync() = default;
    
        RenderSync(const RenderSync&) = delete;
        RenderSync& operator=(const RenderSync&)=delete;

        RenderSync(RenderSync &&) noexcept = default;
        RenderSync& operator=(RenderSync&& other) noexcept = default;

        virtual ~RenderSync() = default;

        RenderSync(VkDevice in_device,
                   std::size_t swapchain_images) {
        
            semaphores_ = std::vector<Semaphores>();
            semaphores_.reserve(swapchain_images);

            for (std::uint32_t i=0; i<swapchain_images; i++) {
                semaphores_.emplace_back(Semaphore({in_device}),
                                         Semaphore({in_device}));
            }

            for (std::uint32_t i=0; i<FramesInFlight; i++) {
                fences_[i] = {{in_device}};
            }
        }

    public:

        const VkSemaphore & GetImageAvailableSemaphore(std::uint8_t image_index) const {
            return semaphores_[image_index].image_available.Value();
        }

        const VkSemaphore & GetRenderFinishedSempahore(std::uint8_t image_index) const {
            return semaphores_[image_index].render_finished.Value();
        }

        const VkFence & GetFence(std::uint8_t frame_index) const {
            return fences_[frame_index].Value();
        }

    private:

        std::vector<Semaphores> semaphores_{};
        std::array<wvk::raii::sync::Fence, FramesInFlight> fences_{};

    };
}
