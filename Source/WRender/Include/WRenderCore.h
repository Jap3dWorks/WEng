#pragma once

#include "WCore.h"
#include <vulkan/vulkan.h>


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

enum class WShaderType
{
    Vertex,
    Fragment,
    // Geometry,
    Compute,
    // TessellationControl,
    // TessellationEvaluation
};

struct WTextureInfo
{
    WId id;
    std::string path;
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
    VkSwapchainKHR swap_chain = nullptr;
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;
    std::vector<VkImage> swap_chain_images;
    std::vector<VkImageView> swap_chain_image_views;
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

class WRENDER_API WWindow
{
    
};
