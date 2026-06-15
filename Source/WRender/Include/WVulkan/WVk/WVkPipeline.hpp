#pragma once
#include "WVulkan/WVulkanStructs.hpp"

namespace wvk::pipeline {
    
    void Destroy(
        WVkRenderPipelineInfo &pipeline_info,
        const VkDevice & device);

}
