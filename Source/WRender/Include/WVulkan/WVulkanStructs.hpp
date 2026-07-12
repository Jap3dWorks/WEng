#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/WVkRenderConfig.hpp"

#include <cstdint>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <string>

#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

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
    wid::WAssetId wid;
    wct::render::EPipelineType type{wct::render::EPipelineType::Graphics};

    VkPipeline pipeline{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};    

    wid::WAssetId descriptor_set_layout_id{0};

    // TODO: Pipeline layout bindings description
    wct::render::WPipeParamDescriptorList params_descriptor{};
};

// Pipeline Bindings
// -----------------

struct WVkDescriptorSetUBOWriteStruct {
    std::uint32_t binding{0};
    
    const void * data{nullptr};
    std::size_t size{0};
    std::size_t offset{0};

    std::size_t range{0};
};

// DEPRECATED
struct WVkDescriptorSetTextureWriteStruct {
    std::uint32_t binding{0};
    VkDescriptorImageInfo image_info{.sampler=VK_NULL_HANDLE,
                                     .imageView=VK_NULL_HANDLE,
                                     .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
};

struct WVkDescriptorSetUBOBinding {
    uint32_t binding{0};
    WVkUBOInfo ubo_info{};
    VkDescriptorBufferInfo buffer_info{}; // TODO: required?
    
    std::size_t size{0};
};

using WVkDescriptorSetTextureBinding =
    WVkDescriptorSetTextureWriteStruct;

template<std::uint32_t Frames>
using TVkDescriptorSetUBOBindingFrames =
    std::array<WVkDescriptorSetUBOBinding, Frames>;

/**
 * @brief Render Pipeline Bindings data
 */
struct WVkPipelineBindingInfo
{
    wid::WAssetId pipeline_id{0};
    wid::WTypeAssetIndexId mesh_asset_id{0};

    std::vector<TVkDescriptorSetUBOBindingFrames<WENG_MAX_FRAMES_IN_FLIGHT>> ubos{};
    std::vector<WVkDescriptorSetTextureBinding> textures{};
};

