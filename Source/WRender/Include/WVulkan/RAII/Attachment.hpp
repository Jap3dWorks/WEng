#pragma once

#include "WCore/WCoreMacros.hpp"
#include "WVulkan/Vk/WVkImage.hpp"

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <vector>
#include <cstring>

namespace wvk::raii {

    class Attachment {
    public:

        static inline constexpr VkImageUsageFlags DEFAULT_USAGE_FLAGS {
            VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                VK_IMAGE_USAGE_SAMPLED_BIT
                };

        static inline constexpr VkImageAspectFlags DEFAULT_ASPECT_FLAGS {
            VK_IMAGE_ASPECT_COLOR_BIT
        };

        static inline constexpr VkImageUsageFlags DEPTH_USAGE_FLAGS {
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT
        };

        static inline constexpr VkImageAspectFlags DEPTH_ASPECT_FLAGS {
            VK_IMAGE_ASPECT_DEPTH_BIT
        };

        Attachment() = default;

        Attachment(const Attachment&) = delete;
        Attachment& operator=(const Attachment&) = delete;

        Attachment(VkDevice in_device,
                   VkPhysicalDevice in_physical_device,
                   VkFormat in_format,
                   VkExtent2D in_extent,
                   VkImageUsageFlags in_usage_flags=DEFAULT_USAGE_FLAGS,
                   VkImageAspectFlags in_aspect_flags=DEFAULT_ASPECT_FLAGS
            ) :
            device_(in_device)
            {
                Initialize(in_physical_device,
                           in_format,
                           in_extent,
                           in_usage_flags,
                           in_aspect_flags
                    );
            }

        ~Attachment() {
            Destroy();
        }
    
        Attachment(Attachment&& other) noexcept :
            device_(other.device_),
            image_(other.image_),
            memory_(other.memory_),
            view_(other.view_)
            {
                other.device_ = VK_NULL_HANDLE;
                other.image_ = VK_NULL_HANDLE;
                other.memory_ = VK_NULL_HANDLE;
                other.view_ = VK_NULL_HANDLE;
            }

        Attachment& operator=(Attachment&& other) noexcept { 
            if (this != &other) {
                Destroy();

                device_ = other.device_;
                image_ = other.image_;
                memory_ = other.memory_;
                view_ = other.view_;

                other.device_ = VK_NULL_HANDLE;
                other.image_ = VK_NULL_HANDLE;
                other.memory_ = VK_NULL_HANDLE;
                other.view_ = VK_NULL_HANDLE;
            }

            return *this;
        }


        WNODISCARD VkImage Image() const { return image_; }

        WNODISCARD VkDeviceMemory Memory() const { return memory_; }

        WNODISCARD VkImageView View() const { return view_; }

    private:

        void Initialize(
            VkPhysicalDevice in_physical_device,
            VkFormat in_format,
            VkExtent2D in_extent,
            VkImageUsageFlags in_usage_flags,
            VkImageAspectFlags in_aspect_flags
            ){

            wvk::image::CreateImage(
                image_,
                memory_,
                device_,
                in_physical_device,
                in_extent.width, in_extent.height,
                1,
                VK_SAMPLE_COUNT_1_BIT,
                in_format,
                VK_IMAGE_TILING_OPTIMAL,
                in_usage_flags,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                );
            
            view_ = wvk::image::CreateImageView(
                image_,
                in_format,
                in_aspect_flags,
                1,
                device_
                );
        }

        void Destroy() {
            if (device_ != VK_NULL_HANDLE) {

                vkDestroyImageView(device_,
                                   view_,
                                   nullptr);
            
                vkDestroyImage(device_,
                               image_,
                               nullptr);
            
                vkFreeMemory(device_,
                             memory_,
                             nullptr);

                view_ = VK_NULL_HANDLE;
                image_ = VK_NULL_HANDLE;
                memory_ = VK_NULL_HANDLE;

                device_ = VK_NULL_HANDLE;
            }
        }

    private:

        VkDevice device_ {VK_NULL_HANDLE};

        VkImage image_{VK_NULL_HANDLE};
        VkDeviceMemory memory_{VK_NULL_HANDLE};
        VkImageView view_{VK_NULL_HANDLE};
    };
}
