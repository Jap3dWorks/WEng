#pragma once

#include "WVulkan/WVk/WVkTypes.hpp"

#include <array>
#include <vulkan/vulkan_core.h>


namespace wvr::offscreen_pipelines {

    template<std::size_t N>
    inline VkPipelineLayout PipelineLayout(
        const std::array<VkDescriptorSetLayout, N> & in_layouts,
        VkDevice in_device
        ) {

        ::VkPipelineLayout result;
        
        VkPipelineLayoutCreateInfo pipeline_layout_info =
            wvk::types::CreateVkPipelineLayoutCreateInfo();

        pipeline_layout_info.setLayoutCount = in_layouts.size();
        pipeline_layout_info.pSetLayouts = in_layouts.data();

        wvk::vulkan::ExecVkProcChecked(vkCreatePipelineLayout,
                                       "Failed to create pipeline layout!",
                                       in_device,
                                       &pipeline_layout_info,
                                       nullptr,
                                       &result);

        return result;
    }
}
