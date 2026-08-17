#pragma once

#include "WVulkan/RAII/Attachment.hpp"
#include <vulkan/vulkan_core.h>
#include <array>
#include <cstdint>

namespace wvk::raii::attachments {

    template<std::uint8_t FramesInFlight>
    class Tonemapping {

    public:

        Tonemapping() = default;
        Tonemapping(const Tonemapping&) = delete;
        Tonemapping& operator=(const Tonemapping&) = delete;

        Tonemapping(Tonemapping&&) = default;
        Tonemapping& operator=(Tonemapping&&) = default;

        virtual ~Tonemapping() = default;

        Tonemapping(
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

        WNODISCARD
        wvk::raii::Attachment const & Color(std::uint8_t frame_index) const noexcept {
            return attachments_[frame_index].color;
        }

        WNODISCARD
        VkExtent2D Extent() const noexcept {
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
                attchs.color = wvk::raii::Attachment(
                    in_device,
                    in_physical_device,
                    in_color_format,
                    in_extent
                    );
            }
        }

    private:

        struct Attachments {
            wvk::raii::Attachment color{};
        };
        std::array<Attachments, FramesInFlight> attachments_{};

        VkExtent2D extent_;
  
    };
}
