#pragma once

#include "WVulkan/WVulkanStructs.hpp"
#include "WCoreTypes/Geometry.hpp"

#include <vulkan/vulkan_core.h>

namespace wvk::pipeline {
    
    void Destroy(
        VkPipeline pipeline,
        VkDevice device
        );

    void Destroy(
        VkPipelineLayout pipeline_layout,
        VkDevice device
        );

    static inline constexpr std::array const GEO_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION {
        VkVertexInputAttributeDescription{
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(wct::geometry::Vertex, position)
        },
        VkVertexInputAttributeDescription{
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(wct::geometry::Vertex, tex_coords),
        },
        VkVertexInputAttributeDescription{
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(wct::geometry::Vertex, color),
        },
        VkVertexInputAttributeDescription{
            .location = 3,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(wct::geometry::Vertex, normal),
        },
        VkVertexInputAttributeDescription{
            .location = 4,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(wct::geometry::Vertex, tangent),
        }
    };

    static inline constexpr std::array const GEO_VERTEX_INPUT_BINDING_DESCRIPTION {
        VkVertexInputBindingDescription{
            .binding=0,
            .stride=sizeof(wct::geometry::Vertex),
            .inputRate=VK_VERTEX_INPUT_RATE_VERTEX
        }
    };

}
