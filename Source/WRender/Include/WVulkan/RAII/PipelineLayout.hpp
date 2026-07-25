#pragma once

#include "WVulkan/RAII/VkRAII.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"

#include <vulkan/vulkan_core.h>
#include <cstdint>

namespace wvk::raii {

    template<std::uint8_t LayoutsCount>
    struct PipelineLayoutCrtr {

        VkPipelineLayout Create(
            std::array<VkDescriptorSetLayout, LayoutsCount>
            layouts
            ) {

            VkPipelineLayout result;
        
            VkPipelineLayoutCreateInfo pipeline_layout_info =
                wvk::types::CreateVkPipelineLayoutCreateInfo();

            pipeline_layout_info.setLayoutCount = LayoutsCount;
            pipeline_layout_info.pSetLayouts = layouts.data();

            wvk::vulkan::ExecVkProcChecked(vkCreatePipelineLayout,
                                           "Failed to create pipeline layout!",
                                           device,
                                           &pipeline_layout_info,
                                           nullptr,
                                           &result);

            return result;

        }

        void Destroy(VkPipelineLayout pipeline_layout) {
            vkDestroyPipelineLayout(
                device,
                pipeline_layout,
                nullptr
                );
        }

    public:

        VkDevice device{VK_NULL_HANDLE};
  
    };

    template<std::uint8_t LayoutsCount>
    using PipelineLayout = wvk::raii::VkRAII<VkPipelineLayout, PipelineLayoutCrtr<LayoutsCount>>;

}
