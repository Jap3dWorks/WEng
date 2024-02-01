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

struct WDescriptorSetLayoutInfo
{
    WId wid;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    VkDescriptorSetLayout descriptor_set_layout = nullptr;
};

struct WRENDER_API WRenderPipelineInfo
{
    WId wid;
    WPipelineType type;
    std::vector<WShaderStage> shaders{};
    WDescriptorSetLayoutInfo *descriptor_set_layout = nullptr;

    VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT;  // No Multisampling
    VkPipeline pipeline = nullptr;
    VkPipelineLayout pipeline_layout = nullptr;
};

class WRENDER_API WRenderPipelines
{
private:
    WDevice& device_;
    std::vector<WDescriptorSetLayoutInfo> descriptor_set_layouts_;
    std::unordered_map<WPipelineType, std::vector<WRenderPipelineInfo>> render_pipelines_;

public:
    ~WRenderPipelines();
    
    WRenderPipelineInfo& CreateRenderPipeline(WRenderPipelineInfo info);
    
    WDescriptorSetLayoutInfo& CreateDescriptorSetLayout(WDescriptorSetLayoutInfo info);

private:

};


namespace WVulkan
{

    void CreateVkRenderPipeline(WDevice device, WRenderPipelineInfo& out_pipeline_info);

    void CreateVkDescriptorSetLayout(WDevice device, WDescriptorSetLayoutInfo& out_descriptor_set_layout_info);
    
    void DestroyVkRenderPipeline(WDevice device, const WRenderPipelineInfo& pipeline_info);
    
    void DestroyDescriptorSetLayout(WDevice device, const WDescriptorSetLayoutInfo& descriptor_set_layout_info); 

};


