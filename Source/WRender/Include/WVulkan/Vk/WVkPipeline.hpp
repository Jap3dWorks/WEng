#pragma once
#include "WVulkan/WVulkanStructs.hpp"

namespace wvk::pipeline {
    
    void Destroy(
        WVkRenderPipeline &pipeline_info,
        const VkDevice & device);

}
