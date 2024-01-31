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


struct WRENDER_API WRenderPipelineInfo
{

    WId wid;
    WPipelineType type;
    std::vector<WShaderStage> shaders{};
    VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT;  // No Multisampling

    VkPipeline pipeline=nullptr;
    VkPipelineLayout pipeline_layout=nullptr;
    
};


class WRENDER_API WRenderPipelines
{
private:
    WDevice& device_;

    std::unordered_map<WPipelineType, std::vector<WRenderPipelineInfo>> pipelines_;

public:
    ~WRenderPipelines();
    
    WRenderPipelineInfo* Create(WRenderPipelineInfo info);


private:

};


class WRENDER_API WVulkanPipeline
{
    static void Create(WRenderPipelineInfo& out_pipeline_info);

};


