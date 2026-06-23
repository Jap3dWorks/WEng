#pragma once

#include "WCore/WCoreMacros.hpp"

#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVkRAII/WVkAttachmentRAII.hpp"

#include <array>
#include <vulkan/vulkan_core.h>

template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkAttachmentsGBuffersRAII {

public:
    
    WVkAttachmentsGBuffersRAII() = default;
    WVkAttachmentsGBuffersRAII(const WVkAttachmentsGBuffersRAII&) = delete;
    WVkAttachmentsGBuffersRAII& operator=(const WVkAttachmentsGBuffersRAII&) = delete;
    WVkAttachmentsGBuffersRAII(WVkAttachmentsGBuffersRAII&& other) noexcept =default;
    WVkAttachmentsGBuffersRAII& operator=(WVkAttachmentsGBuffersRAII&& other) noexcept =default;
    ~WVkAttachmentsGBuffersRAII()=default;
    
    WVkAttachmentsGBuffersRAII(
        VkDevice in_device,
        VkPhysicalDevice in_physical_device,
        VkExtent2D in_extent,
        VkFormat in_color_format,
        VkFormat in_normal_format,
        VkFormat in_ws_position_format,
        VkFormat in_mrAO_format,
        VkFormat in_emission_format,
        VkFormat in_extra01_format,
        VkFormat in_depth_format
        ) : extent_(in_extent) {

        Initialize(
            in_device,
            in_physical_device,
            in_extent,
            in_color_format,
            in_normal_format,
            in_ws_position_format,
            in_mrAO_format,
            in_emission_format,
            in_extra01_format,
            in_depth_format
            );
    }


    WNODISCARD const WVkAttachmentRAII & Albedo(std::uint8_t frame_index) const noexcept {
        return attachments_[frame_index].albedo;
    }

    WNODISCARD const  WVkAttachmentRAII & Normal(std::uint8_t frame_index) const noexcept {
        return attachments_[frame_index].normal;
    }

    WNODISCARD const WVkAttachmentRAII & WsPosition(std::uint8_t frame_index) const noexcept {
        return attachments_[frame_index].ws_position;
    }

    WNODISCARD const WVkAttachmentRAII & MrAO(std::uint8_t frame_index) const noexcept {
        return attachments_[frame_index].mrAO;
    }

    WNODISCARD const WVkAttachmentRAII & Emission(std::uint8_t frame_index) const noexcept {
        return attachments_[frame_index].emission;
    }

    WNODISCARD const WVkAttachmentRAII & Extra01(std::uint8_t frame_index) const noexcept {
        return attachments_[frame_index].extra01;
    }

    WNODISCARD const WVkAttachmentRAII & Depth(std::uint8_t frame_index) const noexcept {
        return attachments_[frame_index].depth;
    }

    WNODISCARD VkExtent2D Extent() const noexcept {
        return extent_;
    }

private:

    void Initialize(
        VkDevice in_device,
        VkPhysicalDevice in_physical_device,
        VkExtent2D in_extent,
        VkFormat in_color_format,
        VkFormat in_normal_format,
        VkFormat in_ws_position_format,
        VkFormat in_mrAO_format,
        VkFormat in_emission_format,
        VkFormat in_extra01_format,
        VkFormat in_depth_format
        ) {
        for(Attachments & attchs : attachments_) {

            attchs.albedo = WVkAttachmentRAII(
                in_device,
                in_physical_device,
                in_color_format,
                in_extent
                );

            // normal
            attchs.normal = WVkAttachmentRAII(
                in_device,
                in_physical_device,
                in_normal_format,
                in_extent
                );

            // ws_position
            attchs.ws_position = WVkAttachmentRAII(
                in_device,
                in_physical_device,
                in_ws_position_format,
                in_extent
                );

            // mrAO
            attchs.mrAO = WVkAttachmentRAII(
                in_device,
                in_physical_device,
                in_mrAO_format,
                in_extent
                );
            
            // emission
            attchs.emission = WVkAttachmentRAII(
                in_device,
                in_physical_device,
                in_emission_format,
                in_extent
                );

            // extra01
            attchs.extra01 = WVkAttachmentRAII(
                in_device,
                in_physical_device,
                in_emission_format,
                in_extent
                );
            
            // depth
            attchs.depth = WVkAttachmentRAII(
                in_device,
                in_physical_device,
                in_depth_format,
                extent_,
                WVkAttachmentRAII::DEPTH_USAGE_FLAGS,
                WVkAttachmentRAII::DEPTH_ASPECT_FLAGS
                );
        }
    }

private:

    struct Attachments {
        WVkAttachmentRAII albedo{};
        WVkAttachmentRAII normal{};
        WVkAttachmentRAII ws_position{};
        WVkAttachmentRAII mrAO{};          // /metallic/roughness/ambientOclusion
        WVkAttachmentRAII emission{};
        WVkAttachmentRAII extra01{};
        WVkAttachmentRAII depth{};
    }; 
    std::array<Attachments, FramesInFlight> attachments_{};

    VkExtent2D extent_{};

    // TODO remove
    // struct WVkGBuffersRenderStruct

};
