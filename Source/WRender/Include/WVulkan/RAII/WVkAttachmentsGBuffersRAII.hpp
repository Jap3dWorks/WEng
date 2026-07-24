#pragma once

#include "WCore/WCoreMacros.hpp"

#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/RAII/Attachment.hpp"

#include <array>
#include <vulkan/vulkan_core.h>

template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
class WVkAttachmentsGBuffersRAII {

public:

    // TODO Read write layouts of each attachment.

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
        VkFormat in_emission_format,
        VkFormat in_normal_format,
        VkFormat in_orm_format,
        VkFormat in_depth_format,
        VkFormat in_extra01_format
        ) : extent_(in_extent) {

        Initialize(
            in_device,
            in_physical_device,
            in_extent,
            in_color_format,
            in_emission_format,
            in_normal_format,
            in_orm_format,
            in_depth_format,
            in_extra01_format
            );
    }

    WNODISCARD const wvk::raii::Attachment & Albedo(std::uint8_t frame_index) const noexcept {
        return frame_attachments_[frame_index].albedo;
    }

    WNODISCARD const wvk::raii::Attachment & Emission(std::uint8_t frame_index) const noexcept {
        return frame_attachments_[frame_index].emission;
    }

    WNODISCARD const  wvk::raii::Attachment & Normal(std::uint8_t frame_index) const noexcept {
        return frame_attachments_[frame_index].normal;
    }

    WNODISCARD const wvk::raii::Attachment & ORM(std::uint8_t frame_index) const noexcept {
        return frame_attachments_[frame_index].orm;
    }

    WNODISCARD const wvk::raii::Attachment & Depth(std::uint8_t frame_index) const noexcept {
        return frame_attachments_[frame_index].depth;
    }

    WNODISCARD const wvk::raii::Attachment & Extra01(std::uint8_t frame_index) const noexcept {
        return frame_attachments_[frame_index].extra01;
    }

    WNODISCARD VkExtent2D Extent() const noexcept {
        return extent_;
    }

private:

    void Initialize(
        VkDevice device,
        VkPhysicalDevice physical_device,
        VkExtent2D in_extent,
        VkFormat in_color_format,
        VkFormat in_emission_format,
        VkFormat in_normal_format,
        VkFormat in_orm_format,
        VkFormat in_depth_format,
        VkFormat in_extra01_format
        ) {
        for(Attachments & attchs : frame_attachments_) {

            // albedo
            attchs.albedo = wvk::raii::Attachment(
                device,
                physical_device,
                in_color_format,
                in_extent
                );

            // emission
            attchs.emission = wvk::raii::Attachment(
                device,
                physical_device,
                in_emission_format,
                in_extent
                );

            // normal
            attchs.normal = wvk::raii::Attachment(
                device,
                physical_device,
                in_normal_format,
                in_extent
                );

            // orm
            attchs.orm = wvk::raii::Attachment(
                device,
                physical_device,
                in_orm_format,
                in_extent
                );

            // depth
            attchs.depth = wvk::raii::Attachment(
                device,
                physical_device,
                in_depth_format,
                extent_,
                wvk::raii::Attachment::DEPTH_USAGE_FLAGS,
                wvk::raii::Attachment::DEPTH_ASPECT_FLAGS
                );
            
            // extra01
            attchs.extra01 = wvk::raii::Attachment(
                device,
                physical_device,
                in_emission_format,
                in_extent
                );
        }
    }

private:

    struct Attachments {
        wvk::raii::Attachment albedo{};
        wvk::raii::Attachment emission{};
        wvk::raii::Attachment normal{};
        wvk::raii::Attachment orm{};          // /metallic/roughness/ambientOclusion
        wvk::raii::Attachment depth{};
        wvk::raii::Attachment extra01{};
    };
    
    std::array<Attachments, FramesInFlight> frame_attachments_{};

    VkExtent2D extent_{};

};
