#pragma once

#include "WCore/WCore.h"
#include <vulkan/vulkan.h>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct WDeviceInfo{
    WId wid;
    VkPhysicalDevice vk_physical_device= VK_NULL_HANDLE;
    VkSampleCountFlagBits msaa_samples = VK_SAMPLE_COUNT_1_BIT;
    std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkQueue vk_graphics_queue = nullptr;
    VkQueue vk_present_queue = nullptr;

    VkDevice vk_device = nullptr;
};

struct WWindowInfo
{
    WId wid;
    std::string title = "WEngine";
    uint32_t width = 800;
    uint32_t height = 600;

    GLFWframebuffersizefun framebuffer_size_callback = nullptr;
    void* user_pointer = nullptr;
    GLFWwindow* window = nullptr;

};

enum class WShaderType
{
    Vertex,
    Fragment,
    // Geometry,
    Compute,
    // TessellationControl,
    // TessellationEvaluation
};

struct WInstanceInfo
{
    WId wid;
    VkInstance instance = nullptr;
};

struct WSurfaceInfo
{
    WId wid;
    VkSurfaceKHR surface = nullptr;
};

struct WRenderDebugInfo
{
    WId wid;
    bool enable_validation_layers = false;
    std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    PFN_vkDebugUtilsMessengerCallbackEXT debug_callback = nullptr;
    VkDebugUtilsMessengerEXT debug_messenger = nullptr;
};

struct WCommandPoolInfo
{
    WId wid;
    VkCommandPool vk_command_pool = nullptr;
};

struct WTextureInfo
{
    WId id;
    VkImage image;
    VkDeviceMemory image_memory;
    VkImageView image_view;
    VkSampler sampler;
    uint32_t mip_levels;
};

struct WShaderStageInfo
{
    WId id;
    std::vector<char> code;
    WShaderType type;
    std::string entry_point;
    
    std::vector<VkVertexInputBindingDescription> binding_descriptors;
    std::vector<VkVertexInputAttributeDescription> attribute_descriptors;

};

enum class WPipelineType
{
    Graphics,       // Default
    Transparency,   // Alpha Blending
    Compute,        // GPGPU
    RayTracing      // Ray Tracing
};

struct WRenderPassInfo
{
    WId wid;
    VkRenderPass render_pass = nullptr;
};

struct WSwapChainInfo
{
    WId wid;
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;
    
    std::vector<VkImage> swap_chain_images;
    std::vector<VkImageView> swap_chain_image_views;
    
    VkImage color_image;
    VkDeviceMemory color_image_memory;
    VkImageView color_image_view;    

    VkImage depth_image;
    VkDeviceMemory depth_image_memory;
    VkImageView depth_image_view;

    std::vector<VkFramebuffer> swap_chain_framebuffers;

    VkSwapchainKHR swap_chain = nullptr;
};

struct WDescriptorSetLayoutInfo
{
    WId wid;
    VkDescriptorSetLayout descriptor_set_layout = nullptr;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
};

struct WRenderPipelineInfo
{
    WId wid;
    WPipelineType type;
    std::vector<WShaderStageInfo> shaders{};

    VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT;  // No Multisampling

    uint32_t subpass = 0; // Index of the subpass where this pipeline will be used

    VkPipeline pipeline = nullptr;
    VkPipelineLayout pipeline_layout = nullptr;
};

struct WMeshInfo
{
    WId wid;
    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;
    VkBuffer index_buffer;
    VkDeviceMemory index_buffer_memory;
    uint32_t index_count;
};

/**
 * Uniform buffer data structure
*/
struct WUniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

/**
 * single vulkan uniform buffer object, 
 * Create a vector of these for multiple frames in flight
*/
struct WUniformBufferObjectInfo
{
    WId wid;
    VkBuffer uniform_buffer;
    VkDeviceMemory uniform_buffer_memory;
    void* mapped_data;
};
