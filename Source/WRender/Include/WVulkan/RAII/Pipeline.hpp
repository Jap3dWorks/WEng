#pragma once

#include "WVulkan/RAII/VkRAII.hpp"
#include <vulkan/vulkan_core.h>

namespace wvk::raii {

    struct PipelineWrapperCrtr {

    public:
        
        inline VkPipeline Create(VkPipeline vk_pipeline) {
            return vk_pipeline;
        }

        inline void Destroy(VkPipeline vk_pipeline ) {
            vkDestroyPipeline(
                device,
                vk_pipeline,
                nullptr
                );
        }

    public:

        VkDevice device{VK_NULL_HANDLE};

    };

    using PipelineWrapper = wvk::raii::VkRAII<VkPipeline, PipelineWrapperCrtr>;
}
