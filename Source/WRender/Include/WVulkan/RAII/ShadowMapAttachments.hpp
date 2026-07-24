#pragma once

#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/RAII/Attachment.hpp"

#include <vulkan/vulkan_core.h>

namespace wvk::raii {

    template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
    class ShadowMapAttachments {

    public:

        static inline constexpr VkFormat SHADOW_MAP_FORMAT
            { VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT };

        static inline constexpr std::uint32_t DEFAULT_SHADOW_MAP_SIZE {1024};

    public:

        ShadowMapAttachments() = default;
        ShadowMapAttachments(const ShadowMapAttachments&) = delete;
        ShadowMapAttachments(ShadowMapAttachments&&) = default;
        ShadowMapAttachments& operator=(const ShadowMapAttachments&) = delete;
        ShadowMapAttachments& operator=(ShadowMapAttachments&&) = default;
        ~ShadowMapAttachments() = default;

        ShadowMapAttachments(
            VkDevice device,
            VkPhysicalDevice physical_device,
            VkExtent2D extent_2d
            )  {
            Initialize(device, physical_device, SHADOW_MAP_FORMAT);
        }

    public:

        WNODISCARD
        wvk::raii::Attachment const & ShadowMap(std::uint8_t frame_index) const noexcept {
            return attachments_[frame_index];
        }

    private:

        void Initialize(
            VkDevice device,
            VkPhysicalDevice physical_device,
            VkFormat format,
            VkExtent2D extent_2d
            ) {
            for(std::uint32_t i=0 ; i<attachments_.size(); i++) {
                attachments_[i]= {
                    device,
                    physical_device,
                    format,
                    extent_2d
                };
            }
        }

    private:

        std::array<wvk::raii::Attachment, FramesInFlight> attachments_{};

    };

}
