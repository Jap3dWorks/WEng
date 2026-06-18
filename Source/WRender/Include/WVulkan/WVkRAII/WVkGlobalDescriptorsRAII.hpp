#pragma once

#include "WVulkan/WVulkanStructs.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>

template<std::uint8_t FramesInFlight>
class WVkGlobalDescriptorsRAII {

public:

private:

    VkDescriptorPool descpool_info{};
    WVkDescriptorSetLayoutInfo descset_layout_info{};
    std::array<VkDescriptorSet, FramesInFlight> descset_info{};
    std::array<WVkUBOInfo, FramesInFlight> camera_ubo{};

    

};
