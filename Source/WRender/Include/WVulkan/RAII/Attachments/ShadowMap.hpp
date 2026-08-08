#pragma once

#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/RAII/Attachment.hpp"

#include <vulkan/vulkan_core.h>

namespace wvk::raii::attachments {

    template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
    class ShadowMap {

    public:

        static inline constexpr VkFormat SHADOW_MAP_FORMAT {
            VK_FORMAT_D32_SFLOAT
                };

        static inline constexpr std::uint32_t DEFAULT_SHADOW_MAP_SIZE {1024};

    public:

        ShadowMap() = default;
        ShadowMap(const ShadowMap&) = delete;
        ShadowMap(ShadowMap&&) = default;
        ShadowMap& operator=(const ShadowMap&) = delete;
        ShadowMap& operator=(ShadowMap&&) = default;
        ~ShadowMap() = default;

        ShadowMap(
            VkDevice device,
            VkPhysicalDevice physical_device,
            VkExtent2D extent_2d
            )  :
            extent_(extent_2d) {
            Initialize(
                device,
                physical_device,
                SHADOW_MAP_FORMAT,
                extent_2d
                );
        }

    public:

        WNODISCARD
        wvk::raii::Attachment const &
        GetDepth(std::uint8_t frame_index) const noexcept {
            return attachments_[frame_index];
        }

        WNODISCARD
        VkExtent2D GetExtent() const noexcept {
            return extent_;
        }

    private:

        void Initialize(
            VkDevice device,
            VkPhysicalDevice physical_device,
            VkFormat format,
            VkExtent2D attachment_size
            ) {
            for(std::uint32_t i=0 ; i<attachments_.size(); i++) {
                attachments_[i]= {
                    device,
                    physical_device,
                    format,
                    attachment_size,
                    wvk::raii::Attachment::DEPTH_USAGE_FLAGS,
                    wvk::raii::Attachment::DEPTH_ASPECT_FLAGS
                };
            }
        }

    private:

        std::array<wvk::raii::Attachment, FramesInFlight> attachments_{};

        VkExtent2D extent_{};

    };

}
