// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore.h"
#include "WRenderCore.h"
#include "vulkan/vulkan.h"
#include "WShader.h"
#include <vector>
#include <unordered_map>


enum class WRENDER_API WPipelineType
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

struct WRENDER_API WRenderPipelineInfo
{
    WId wid;
    WPipelineType type;
    std::vector<WShaderStage> shaders{};

    VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT;  // No Multisampling
    VkPipeline pipeline = nullptr;
    VkPipelineLayout pipeline_layout = nullptr;
};

class WRENDER_API WRenderPipelines
{
public:
    ~WRenderPipelines();
    WRenderPipelines(WDeviceInfo& device) : device_(device) {}

    WRenderPipelineInfo& CreateRenderPipeline(WRenderPipelineInfo info);
    WDescriptorSetLayoutInfo& CreateDescriptorSetLayout(WDescriptorSetLayoutInfo info);

private:
    WDeviceInfo& device_;
    std::vector<WDescriptorSetLayoutInfo> descriptor_set_layouts_;
    std::unordered_map<WPipelineType, std::vector<WRenderPipelineInfo>> render_pipelines_;

};


namespace WVulkan
{

    void CreateVkRenderPipeline(const WDeviceInfo &device, const WDescriptorSetLayoutInfo& descriptor_set_layout_info, WRenderPipelineInfo& out_pipeline_info);

    void CreateVkDescriptorSetLayout(const WDeviceInfo &device, WDescriptorSetLayoutInfo& out_descriptor_set_layout_info);
    
    void DestroyVkRenderPipeline(const WDeviceInfo &device, const WRenderPipelineInfo& pipeline_info);
    
    void DestroyDescriptorSetLayout(const WDeviceInfo &device, const WDescriptorSetLayoutInfo& descriptor_set_layout_info); 

};


