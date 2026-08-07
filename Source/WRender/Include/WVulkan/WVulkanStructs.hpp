#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WId.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/WVkConfig.hpp"

#include <cstdint>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <string>

#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>
#include <variant>

// RENAME WVulkanDataTypes.hpp
// TODO namespace : wvk::datatypes | wvk::structs

struct WVkRenderDebugInfo
{
    bool enable_validation_layers{false};
    std::vector<std::string_view> validation_layers{
        "VK_LAYER_KHRONOS_validation"
    };

    PFN_vkDebugUtilsMessengerCallbackEXT debug_callback{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT debug_messenger{VK_NULL_HANDLE};
};

struct WVkTextureInfo
{
    VkImage image{VK_NULL_HANDLE};
    VkDeviceMemory memory{VK_NULL_HANDLE};
    VkImageView view{VK_NULL_HANDLE};
    VkSampler sampler{VK_NULL_HANDLE};
    // is layout required here?
    VkImageLayout layout{VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    uint32_t mip_levels{1};
};

/**
 * @brief: Shader related data.
 */
struct WVkShaderStageInfo
{
    std::variant<
        std::span<std::uint8_t>,
        std::vector<std::uint8_t>
        > code;
    wct::render::EShaderStageFlag type;
    std::string entry_point{"main"};
};

struct WVkMesh
{
    VkBuffer vertex_buffer {VK_NULL_HANDLE};
    VkDeviceMemory vertex_buffer_memory {VK_NULL_HANDLE};
    VkBuffer index_buffer {VK_NULL_HANDLE};
    VkDeviceMemory index_buffer_memory {VK_NULL_HANDLE};
    uint32_t index_count {0};
};

struct WVkBuffer
{
    VkBuffer buffer{VK_NULL_HANDLE};
    VkDeviceMemory device_memory{VK_NULL_HANDLE};
    VkDeviceSize range{16};
};

// Pipeline Bindings
// -----------------

struct WVkDescSetTextureBinding {
    std::uint8_t binding{0};

    VkDescriptorImageInfo image_info{
        .sampler=VK_NULL_HANDLE,
        .imageView=VK_NULL_HANDLE,
        .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };
};

template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
struct WVkDescSetUBOBinding {
    std::uint8_t binding{0};

    std::uint32_t offset{0};
    std::uint32_t range{0};
    std::array<WVkBuffer, FramesInFlight> buffers;
    VkDescriptorType ubo_type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
};

