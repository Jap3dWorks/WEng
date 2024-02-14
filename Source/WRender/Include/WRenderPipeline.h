// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore.h"
#include "WRenderCore.h"
#include "vulkan/vulkan.h"
#include "WShader.h"
#include <vector>
#include <unordered_map>

// TODO move srtucts to WRenderCore.h


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

/*
 * A render pipeline is a collection of shaders and render states.
 * This class
*/
class WRENDER_API WRenderPipelinesManager
{

public:
    ~WRenderPipelinesManager();
    WRenderPipelinesManager(
        WDeviceInfo& device, 
        WRenderPassInfo& render_pass_info
    );

    WDescriptorSetLayoutInfo& CreateDescriptorSetLayout(
        WDescriptorSetLayoutInfo descriptor_set_layout_info
    );
    WRenderPipelineInfo& CreateRenderPipeline(
        WRenderPipelineInfo render_pipeline_info, 
        const WDescriptorSetLayoutInfo& descriptor_set_layout_info
    );

private:
    WDeviceInfo& device_info_;
    WRenderPassInfo& render_pass_info_;
    std::vector<WDescriptorSetLayoutInfo> descriptor_set_layouts_;
    std::unordered_map<WPipelineType, std::vector<WRenderPipelineInfo>> render_pipelines_;

};
