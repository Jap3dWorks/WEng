#pragma once

#include "WVulkan/RAII/VkRAII.hpp"
#include "WVulkan/Vk/WVulkan.hpp"

#include <vulkan/vulkan_core.h>
#include <array>
#include <cstdint>

namespace wvk::raii {

    template<std::uint8_t Bindings>
    struct DescriptorSetLayoutCrtr {
    public:

        VkDescriptorSetLayout Create(
            std::array<VkDescriptorSetLayoutBinding, Bindings> bindings
            ) {
            VkDescriptorSetLayoutCreateInfo layout_info{};
            layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layout_info.bindingCount = static_cast<std::uint32_t>(bindings.size());
            layout_info.pBindings=bindings.data();

            VkDescriptorSetLayout result;
            wvk::vulkan::ExecVkProcChecked(
                vkCreateDescriptorSetLayout,
                "Failed to create Descriptor Set Layout!",
                device,
                &layout_info,
                nullptr,
                &result
                );

            return result;
        }

        void Destroy(VkDescriptorSetLayout desc_lay) {
            vkDestroyDescriptorSetLayout(
                device,
                desc_lay,
                nullptr
                );
        }

    public:

        VkDevice device{VK_NULL_HANDLE};

    };

    template<std::uint8_t Bindings>
    using DescriptorSetLayout = wvk::raii::VkRAII<VkDescriptorSetLayout,
                                                  DescriptorSetLayoutCrtr<Bindings>>;
}
