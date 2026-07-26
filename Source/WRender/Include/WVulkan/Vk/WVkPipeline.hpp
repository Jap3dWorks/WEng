#pragma once

#include "WVulkan/WVulkanStructs.hpp"
#include "WCoreTypes/WGeometry.hpp"

#include <vulkan/vulkan_core.h>

namespace wvk::pipeline {
    
    void Destroy(
        WVkRenderPipeline &pipeline_info,
        const VkDevice & device);


    static inline constexpr std::array const GBUFFER_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION {
        VkVertexInputAttributeDescription{
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(wct::geometry::WVertex, position)
        },
        VkVertexInputAttributeDescription{
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(wct::geometry::WVertex, tex_coords),
        },
        VkVertexInputAttributeDescription{
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(wct::geometry::WVertex, color),
        },
        VkVertexInputAttributeDescription{
            .location = 3,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(wct::geometry::WVertex, normal),
        },
        VkVertexInputAttributeDescription{
            .location = 4,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(wct::geometry::WVertex, tangent),
        }
    };

    static inline constexpr std::array const GBUFFER_VERTEX_INPUT_BINDING_DESCRIPTION {
        VkVertexInputBindingDescription{
            .binding=0,
            .stride=sizeof(wct::geometry::WVertex),
            .inputRate=VK_VERTEX_INPUT_RATE_VERTEX
        }
    };

    // static inline constexpr std::array const PPCESS_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION {
    //     VkVertexInputAttributeDescription{
    //         .binding=0,
    //         .location=0,
    //         .format = VK_FORMAT_R32G32_SFLOAT,
    //         .offset = 0
    //     },
    //     VkVertexInputAttributeDescription{
    //         .binding=0,
    //         .location=1,
    //         .format=VK_FORMAT_R32G32_SFLOAT,
    //         .offset= offsetof(WVkPostprocessVertex, tex_coords)
    //     }
    // };

}
