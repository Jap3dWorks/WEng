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

struct WVkDescriptorSetLayoutInfo
{
    // TODO it is not required to store the descriptor bindings.
    //  Remove this struct.
    std::vector<VkDescriptorSetLayoutBinding> bindings{};
    VkDescriptorSetLayout descset_layout{VK_NULL_HANDLE};
};

struct WVkMesh
{
    VkBuffer vertex_buffer {VK_NULL_HANDLE};
    VkDeviceMemory vertex_buffer_memory {VK_NULL_HANDLE};
    VkBuffer index_buffer {VK_NULL_HANDLE};
    VkDeviceMemory index_buffer_memory {VK_NULL_HANDLE};
    uint32_t index_count {0};
};

struct WVkUBO
{
    VkBuffer buffer{VK_NULL_HANDLE};
    VkDeviceMemory device_memory{VK_NULL_HANDLE};
    VkDeviceSize range{16};
};

struct WVkRenderPipeline
{
    wct::render::ERPipeType type{wct::render::ERPipeType::GBuffer};

    VkPipeline pipeline{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};    
};

// Pipeline Bindings
// -----------------

// TODO is this struct required?
struct WVkDescSetUBOWrite {
    std::uint8_t binding{0};
    
    const void * data{nullptr};
    std::size_t size{0};
    /** Offset applied to mapped pointer when a vulkan buffer is mapped. */
    std::size_t offset{0};      
};

struct WVkDescSetTextureBinding {
    std::uint8_t binding{0};

    VkDescriptorImageInfo image_info{
        .sampler=VK_NULL_HANDLE,
        .imageView=VK_NULL_HANDLE,
        .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };
};

struct WVkDescUBOInfo {
    /**
     * index in AssetRenderData.
     */
    std::size_t index{0};

    VkDescriptorBufferInfo desc_buffer; // <- avoid search buffer during rendering
};

template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
struct WVkDescSetUBOBinding {
    std::uint8_t binding{0};

    std::array<WVkDescUBOInfo, FramesInFlight> ubo_desc{};
};

template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
struct _new_WVkDescSetUBOBinding {
    std::uint8_t binding{0};
    std::uint32_t dynamic_offset{0};
    std::uint32_t range{0};

    std::array<VkBuffer, FramesInFlight> buffers;
    // struct ubodt {
    //     std::uint32_t dynamic_offset;
    //     VkDescriptor descriptor{};
    // };
};

template<std::uint8_t Frames>
struct WVkPipelineBinding {
    wcr::wid::WAssetId pipeline_id{0};
    wcr::wid::WTypeAssetIndexId mesh_asset_id{0};

    std::vector<WVkDescSetUBOBinding<Frames>> ubos{};
    std::vector<WVkDescSetTextureBinding> textures{};
};

