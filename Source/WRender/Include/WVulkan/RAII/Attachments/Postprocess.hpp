#pragma once

#include "WVulkan/RAII/Attachment.hpp"
#include "WVulkan/WVkConfig.hpp"

#include <array>

namespace wvk::raii::attachments {
    
    template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
    class Postprocess {

    public:

        Postprocess() = default;
        Postprocess(const Postprocess&) = delete;
        Postprocess& operator=(const Postprocess&) = delete;
        Postprocess(Postprocess&&) = default;
        Postprocess& operator=(Postprocess&&) = default;
        ~Postprocess() = default;

        Postprocess(
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

        WNODISCARD const wvk::raii::Attachment & Color(std::uint8_t frame_index) const noexcept {
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

        VkExtent2D extent_{};

    };
}
