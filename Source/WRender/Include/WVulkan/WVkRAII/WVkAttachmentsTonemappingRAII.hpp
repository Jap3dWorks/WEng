#pragma once

#include "WVulkan/WVkRAII/WVkAttachmentRAII.hpp"
#include <vulkan/vulkan_core.h>
#include <array>
#include <cstdint>

template<std::uint8_t FramesInFlight>
class WVkAttachmentsTonemappingRAII {

public:

    WVkAttachmentsTonemappingRAII() = default;
    WVkAttachmentsTonemappingRAII(const WVkAttachmentsTonemappingRAII&) = delete;
    WVkAttachmentsTonemappingRAII& operator=(const WVkAttachmentsTonemappingRAII&) = delete;

    WVkAttachmentsTonemappingRAII(WVkAttachmentsTonemappingRAII&&) = default;
    WVkAttachmentsTonemappingRAII& operator=(WVkAttachmentsTonemappingRAII&&) = default;

    virtual ~WVkAttachmentsTonemappingRAII() = default;

    WVkAttachmentsTonemappingRAII(
        VkDevice in_device,
        VkPhysicalDevice in_physical_device,
        VkExtent2D in_extent,
        VkFormat in_color_format
        ) : extent_(in_extent) {
        Initialize(
            in_device,
            in_physical_device,
            in_extent,
            in_color_format
            );
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
        for (Attachments & attchs : attachments_) {
            attchs.color = WVkAttachmentRAII(
                in_device,
                in_physical_device,
                in_color_format,
                in_extent
                );
        }
    }

private:

    struct Attachments {
        WVkAttachmentRAII color{};
    };
    std::array<Attachments, FramesInFlight> attachments_{};

    VkExtent2D extent_;
  
};
