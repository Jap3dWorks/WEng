#pragma once

#include "WCore/WCoreMacros.hpp"

#include "WVulkan/WVkRAII/WVkAttachmentRAII.hpp"
#include "WVulkan/WVkRenderConfig.hpp"

#include <array>
#include <cstdint>

template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkAttachmentsOffscreenRAII {

public:

    WVkAttachmentsOffscreenRAII() = default;
    WVkAttachmentsOffscreenRAII(const WVkAttachmentsOffscreenRAII&) = delete;
    WVkAttachmentsOffscreenRAII(WVkAttachmentsOffscreenRAII&&) = default;
    WVkAttachmentsOffscreenRAII& operator=(const WVkAttachmentsOffscreenRAII&) = delete;
    WVkAttachmentsOffscreenRAII& operator=(WVkAttachmentsOffscreenRAII&&) = default;
    ~WVkAttachmentsOffscreenRAII() = default;

    WVkAttachmentsOffscreenRAII(
        VkDevice in_device,
        VkPhysicalDevice in_physical_device,
        VkExtent2D in_extent,
        VkFormat in_color_format
        ) : extent_(in_extent) {
        Initialize(in_device,
                   in_physical_device,
                   in_extent,
                   in_color_format);
    }

    WNODISCARD const WVkAttachmentRAII & Color(std::uint8_t frame_index) const noexcept {
        return attachments_[frame_index].color;
    }


    WNODISCARD VkExtent2D Extent() const noexcept {
        return extent_;
    }

private:

    void Initialize(
        VkDevice in_device,
        VkPhysicalDevice in_physical_device,
        VkExtent2D in_extent,
        VkFormat in_color_format
        ) {
        for(Attachments & attchs : attachments_) {
            attchs.color = WVkAttachmentRAII(
                in_device,
                in_physical_device,
                in_color_format,
                in_extent
                );
        }
    }

    struct Attachments {
        WVkAttachmentRAII color;
    };

    std::array<Attachments, FramesInFlight> attachments_;

    VkExtent2D extent_{};
  
};
