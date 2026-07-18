#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WId.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/WVkRenderConfig.hpp"

#include <cstdint>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <string>

#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

// TODO namespace : wvk::types | wvk::structs

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
    std::vector<char> code;
    wct::render::EShaderStageFlag type;
    std::string entry_point{"main"};
    
    std::vector<VkVertexInputBindingDescription> binding_descriptors{};     // vertex size, ...
    std::vector<VkVertexInputAttributeDescription> attribute_descriptors{}; // vertex attributes, ...
};

// // DEPRECATED
// struct WVkRenderTarget {
//     VkImage image{VK_NULL_HANDLE};
//     VkDeviceMemory memory{VK_NULL_HANDLE};
//     VkImageView view{VK_NULL_HANDLE};
//     VkFormat format{};
//     VkExtent2D extent{};
// };

// struct WVkTonemappingRenderStruct {
//     WVkRenderTarget color{};
//     VkExtent2D extent{};
// };

struct WVkSwapChainInfo
{
    VkFormat format;
    VkExtent2D extent;
    VkSwapchainKHR swap_chain{VK_NULL_HANDLE};
    
    std::vector<VkImage> images;
    std::vector<VkImageView> views;
    std::vector<VkDeviceMemory> memory;
    
    // std::uint32_t image_count{};
};

struct WVkDescriptorSetLayoutInfo
{
    // TODO it is not required to store the descriptor bindings.
    //  Remove this struct.
    std::vector<VkDescriptorSetLayoutBinding> bindings{};
    VkDescriptorSetLayout descset_layout{VK_NULL_HANDLE};
};

struct WVkMeshInfo
{
    VkBuffer vertex_buffer {VK_NULL_HANDLE};
    VkDeviceMemory vertex_buffer_memory {VK_NULL_HANDLE};
    VkBuffer index_buffer {VK_NULL_HANDLE};
    VkDeviceMemory index_buffer_memory {VK_NULL_HANDLE};
    uint32_t index_count {0};
};

struct WVkUBOInfo
{
    VkBuffer buffer{VK_NULL_HANDLE};
    VkDeviceMemory device_memory{VK_NULL_HANDLE};
    VkDeviceSize range{1};
};

struct WVkRenderPipelineInfo
{
    wcr::wid::WAssetId wid;
    wct::render::ERPipeType type{wct::render::ERPipeType::Graphics};

    VkPipeline pipeline{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};    

    wcr::wid::WAssetId descriptor_set_layout_id{0};

    // TODO: Pipeline layout bindings description
    wct::render::RPipeParamDescLayList params_descriptor{};
};

// Pipeline Bindings
// -----------------

// TODO is this struct required?
struct WVkDescSetUBOWrite {
    std::uint32_t binding{0};
    
    const void * data{nullptr};
    std::size_t size{0};
    /** Offset applied to mapped pointer when a vulkan buffer is mapped. */
    std::size_t offset{0};      
};

struct WVkDescSetTextureBinding {
    std::uint32_t binding{0};
    VkDescriptorImageInfo image_info{
        .sampler=VK_NULL_HANDLE,
        .imageView=VK_NULL_HANDLE,
        .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };
};

struct DELETE_WVkDescriptorSetUBOBinding {
    std::uint32_t binding{0};
    WVkUBOInfo ubo_info{}; // <- TODO remove

    // ubos by frame
    VkDescriptorBufferInfo buffer_info;
};

template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
struct WVkDescSetUBOBinding {
    std::uint32_t binding{0};
    std::array<VkDescriptorBufferInfo, FramesInFlight> ubo_info{};
};

template<std::uint32_t Frames>
using TVkDescriptorSetUBOBindingFrames =
    std::array<DELETE_WVkDescriptorSetUBOBinding, Frames>;

/**
 * @brief Render Pipeline Bindings data
 */
struct WVkPipelineBindingInfo
{
    wcr::wid::WAssetId pipeline_id{0};
    wcr::wid::WTypeAssetIndexId mesh_asset_id{0};

    std::vector<TVkDescriptorSetUBOBindingFrames<WENG_MAX_FRAMES_IN_FLIGHT>> ubos{};
    std::vector<WVkDescSetTextureBinding> textures{};
};

