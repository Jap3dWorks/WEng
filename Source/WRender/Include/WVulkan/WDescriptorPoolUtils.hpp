#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"

namespace WDescPoolUtils {
    
    void CreateGraphicsDescSetPool(
        WVkDescriptorPoolInfo & out_descriptor_pool_info,
        const WVkDeviceInfo & device
        );

    void Destroy(
        WVkDescriptorPoolInfo & out_descriptor_pool_info,
        const WVkDeviceInfo & in_device
        );
}
