#pragma once

#include "WCore/WCore.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVkRenderConfig.hpp"

#include <cstdint>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <string>

#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

struct WVkDeviceInfo
{
    VkPhysicalDevice vk_physical_device { VK_NULL_HANDLE };
    VkSampleCountFlagBits msaa_samples { VK_SAMPLE_COUNT_1_BIT };

    std::vector<const char*> device_extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME
    };

    VkQueue vk_graphics_queue {VK_NULL_HANDLE};
    VkQueue vk_present_queue {VK_NULL_HANDLE};
    VkDevice vk_device {VK_NULL_HANDLE};
};

struct WVkInstanceInfo
{
    VkInstance instance {nullptr};
};

struct WVkSurfaceInfo
{
    VkSurfaceKHR surface{nullptr};
};

struct WVkRenderDebugInfo
{
    bool enable_validation_layers{false};
    std::vector<const char*> validation_layers{
        "VK_LAYER_KHRONOS_validation",
    };

    PFN_vkDebugUtilsMessengerCallbackEXT debug_callback{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT debug_messenger{VK_NULL_HANDLE};
};

struct WVkCommandPoolInfo
{
    VkCommandPool vk_command_pool{ VK_NULL_HANDLE };
};

struct WVkTextureInfo
{
    VkImage image{VK_NULL_HANDLE};
    VkDeviceMemory memory{VK_NULL_HANDLE};
    VkImageView view{VK_NULL_HANDLE};
    VkSampler sampler{VK_NULL_HANDLE};
    // TODO: imageLayout
    VkImageLayout layout{VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    uint32_t mip_levels{1};
};

/**
 * @brief: Shader related data.
 */
struct WVkShaderStageInfo
{
    std::vector<char> code;
    EShaderStageFlag type;
    std::string entry_point{"main"};
    
    std::vector<VkVertexInputBindingDescription> binding_descriptors{};     // vertex size, ...
    std::vector<VkVertexInputAttributeDescription> attribute_descriptors{}; // vertex attributes, ...
};

struct WVkRenderTarget {
    VkImage image{VK_NULL_HANDLE};
    VkDeviceMemory memory{VK_NULL_HANDLE};
    VkImageView view{VK_NULL_HANDLE};
    VkFormat format{};
    VkExtent2D extent{};
};

// Render Resources

struct WVkGBuffersRenderStruct
{
    WVkRenderTarget albedo{};
    WVkRenderTarget normal{};
    WVkRenderTarget ws_position{};

    // others like roughness Specular etc

    WVkRenderTarget rt_extra01{};
    WVkRenderTarget rt_extra02{};
    WVkRenderTarget rt_extra03{};

    WVkRenderTarget depth{};

    
    VkExtent2D extent{};
};

struct WVkOffscreenRenderStruct {

    WVkRenderTarget color{};

    VkExtent2D extent{};
};

struct WVkPostprocessRenderStruct
{
    WVkRenderTarget color{};
    VkExtent2D extent{};
};

struct WVkSwapChainInfo
{
    VkFormat format;
    VkExtent2D extent;
    VkSwapchainKHR swap_chain{VK_NULL_HANDLE};
    
    std::vector<VkImage> images;
    std::vector<VkImageView> views;
    std::vector<VkDeviceMemory> memory;
    
    std::uint32_t image_count{};
};

struct WVkDescriptorSetLayoutInfo
{
    WAssetId wid{0};
    std::vector<VkDescriptorSetLayoutBinding> bindings{};
    VkDescriptorSetLayout descset_layout{VK_NULL_HANDLE};
};

/**
 * Descriptor pool is used to allocate descriptors memory., 
 * Represents the maximum number of descriptors that can be allocated.
*/
struct WVkDescriptorPoolInfo
{
    VkDescriptorPool descriptor_pool{VK_NULL_HANDLE};
};

/**
 * Descriptor set is the relation between the descriptor set layout 
 * and the descriptor pool
 * you can have multiple descriptor sets with the same layout, 
 * this is used for multiple frames in flight
*/
struct WVkDescriptorSetInfo
{
    // The len of this vector is the number of frames in flight. TODO: REMOVE
    VkDescriptorSet descriptor_set{VK_NULL_HANDLE};
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
    VkDeviceMemory buffer_memory{VK_NULL_HANDLE};
    void * mapped_memory{nullptr};
    VkDeviceSize range{1};
};

struct WVkRenderPipelineInfo
{
    WAssetId wid;
    EPipelineType type{EPipelineType::Graphics};

    VkPipeline pipeline{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};    

    WAssetId descriptor_set_layout_id{0};

    // TODO: Pipeline layout bindings description
    WPipeParamDescriptorList params_descriptor{};
};

/**
 * @brief Helper struct to store command buffer data
 */
struct WVkCommandBufferInfo
{
    std::array<VkCommandBuffer, WENG_MAX_FRAMES_IN_FLIGHT> command_buffers {VK_NULL_HANDLE};
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
    WAssetId pipeline_id{0};
    WAssetIndexId mesh_asset_id{0};

    std::vector<TVkDescriptorSetUBOBindingFrames<WENG_MAX_FRAMES_IN_FLIGHT>> ubos{};
    std::vector<WVkDescriptorSetTextureBinding> textures{};

};

