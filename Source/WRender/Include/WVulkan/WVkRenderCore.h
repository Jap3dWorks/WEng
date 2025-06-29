#pragma once

#include "WCore/WCore.h"
#include <cstdint>
#include <vulkan/vulkan.h>
#include "WVulkan/WVkRenderConfig.h"
#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

enum class EShaderType : uint8_t
{
    Vertex,
    Fragment,
    // Geometry,
    Compute,
    // TessellationControl,
    // TessellationEvaluation
};

enum class EPipelineType : uint8_t
{
    Graphics,       // Default
    Transparency,   // Alpha Blending
    Compute,        // GPGPU
    RayTracing      // Ray Tracing
};

/**
 * Uniform buffer data structure
*/
struct WVkUBOStruct
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct WVkDeviceInfo{
    WId wid;
    VkPhysicalDevice vk_physical_device { VK_NULL_HANDLE };
    VkSampleCountFlagBits msaa_samples { VK_SAMPLE_COUNT_1_BIT };

    std::vector<const char*> device_extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkQueue vk_graphics_queue {VK_NULL_HANDLE};
    VkQueue vk_present_queue {VK_NULL_HANDLE};

    VkDevice vk_device {VK_NULL_HANDLE};
};

struct WVkWindowInfo
{
    WId wid;
    std::string title {"WEngine"};
    uint32_t width {800};
    uint32_t height {600};

    GLFWframebuffersizefun framebuffer_size_callback {nullptr};
    void * user_pointer {nullptr};
    GLFWwindow * window {nullptr};

};

struct WVkInstanceInfo
{
    WId wid;
    VkInstance instance {nullptr};
};

struct WVkSurfaceInfo
{
    WId wid;
    VkSurfaceKHR surface{nullptr};
};

struct WVkRenderDebugInfo
{
    WId wid;
    bool enable_validation_layers{false};
    std::vector<const char*> validation_layers{
        "VK_LAYER_KHRONOS_validation",
    };

    PFN_vkDebugUtilsMessengerCallbackEXT debug_callback{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT debug_messenger{VK_NULL_HANDLE};
};

struct WVkCommandPoolInfo
{
    WId wid;
    VkCommandPool vk_command_pool{ VK_NULL_HANDLE };
};

struct WVkTextureInfo
{
    WId id;
    VkImage image{VK_NULL_HANDLE};
    VkDeviceMemory image_memory{VK_NULL_HANDLE};
    VkImageView image_view{VK_NULL_HANDLE};
    VkSampler sampler{VK_NULL_HANDLE};

    uint32_t mip_levels{1};
};

/**
 * @brief: Shader related data.
 */
struct WVkShaderStageInfo
{
    WId id;

    std::vector<char> code;
    EShaderType type;
    std::string entry_point{"main"};
    
    std::vector<VkVertexInputBindingDescription> binding_descriptors{};
    std::vector<VkVertexInputAttributeDescription> attribute_descriptors{};

};

struct WVkRenderPassInfo
{
    WId wid;
    VkRenderPass render_pass{nullptr};
};

struct WVkSwapChainInfo
{
    WId wid;
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;
    
    std::vector<VkImage> swap_chain_images;
    std::vector<VkImageView> swap_chain_image_views;
    
    VkImage color_image{VK_NULL_HANDLE};
    VkDeviceMemory color_image_memory{VK_NULL_HANDLE};
    VkImageView color_image_view{VK_NULL_HANDLE};    

    VkImage depth_image{VK_NULL_HANDLE};
    VkDeviceMemory depth_image_memory{VK_NULL_HANDLE};
    VkImageView depth_image_view{VK_NULL_HANDLE};

    std::vector<VkFramebuffer> swap_chain_framebuffers{};

    VkSwapchainKHR swap_chain{VK_NULL_HANDLE};
};

struct WVkDescriptorSetLayoutInfo
{
    WId wid;
    std::vector<VkDescriptorSetLayoutBinding> bindings{};
    VkDescriptorSetLayout descriptor_set_layout{VK_NULL_HANDLE};
};

/**
 * Descriptor pool is used to allocate descriptors memory., 
 * Represents the maximum number of descriptors that can be allocated.
*/
struct WVkDescriptorPoolInfo
{
    WId wid;
    std::vector<VkDescriptorPoolSize> pool_sizes {};
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
    WId wid;
    // The len of this vector is the number of frames in flight
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptor_sets{VK_NULL_HANDLE};
};

struct WVkMeshInfo
{
    WId wid {0};
    VkBuffer vertex_buffer {VK_NULL_HANDLE};
    VkDeviceMemory vertex_buffer_memory {VK_NULL_HANDLE};
    VkBuffer index_buffer {VK_NULL_HANDLE};
    VkDeviceMemory index_buffer_memory {VK_NULL_HANDLE};
    uint32_t index_count {0};
};

/**
 * @brief Render Pipeline Bindings data
 */
struct WVkPipelineBindingInfo
{
    WVkDescriptorSetInfo descriptor {};
    WVkMeshInfo mesh_info{};
};


struct WVkRenderPipelineInfo
{
    WId wid;
    EPipelineType type{EPipelineType::Graphics};

    // std::vector<WShaderStageInfo> shaders{};  // We need the shaders at creation time

    // VkSampleCountFlagBits sample_count{VK_SAMPLE_COUNT_1_BIT};  // No Multisampling

    uint32_t subpass{0}; // Index of the subpass where this pipeline will be used

    VkPipeline pipeline{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};

    WId descriptor_set_layout_id{0};
};

/**
 * single vulkan uniform buffer object, 
 * Create a vector of these for multiple frames in flight
*/
struct WVkUniformBufferObjectInfo
{
    WId wid{};

    VkBuffer uniform_buffer{VK_NULL_HANDLE};
    VkDeviceMemory uniform_buffer_memory{VK_NULL_HANDLE};
    void* mapped_data{nullptr};
};

/**
 * @brief Helper struxt to store command buffer data
 */
struct WVkCommandBufferInfo
{
    WId wid;
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> command_buffers {VK_NULL_HANDLE};
};

struct WVkSemaphoreInfo
{
    WId wid;
    
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> semaphores {VK_NULL_HANDLE};
};

struct WVkFenceInfo
{
    WId wid;

    std::array<VkFence, MAX_FRAMES_IN_FLIGHT> fences {VK_NULL_HANDLE};

    VkFenceCreateFlagBits creation_flags {VK_FENCE_CREATE_SIGNALED_BIT};
};

