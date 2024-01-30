// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore.h"
#include "WRenderCore.h"
#include "vulkan/vulkan.h"
#include "WShader.h"
#include <vector>
#include <unordered_map>


enum class WPipelineType
{
    Graphics,  // Default
    Transparency,  // Alpha Blending
    Compute,  // GPGPU
    RayTracing  // Ray Tracing
};


struct WRenderPipelineInfo
{
    WId wid;
    WPipelineType type;
    VkPipeline pipeline=nullptr;
    std::vector<WShaderStage> shaders{};
};


class WRENDER_API WRenderPipeline
{
private:
    WRenderPipelineInfo info_;
    WRenderPipeline() {}

    static std::unordered_map<WPipelineType, std::vector<WRenderPipeline>> pipelines_;

public:

    static WRenderPipeline Create(WRenderPipelineInfo& out_info)
    {
        WRenderPipeline pipeline;
        pipeline.info_ = out_info;

        return pipeline;
    }

private:

};


class WRENDER_API WVulkanPipeline
{
    static void Create(const WDevice& device, WRenderPipelineInfo& out_pipeline_info)
    {
        // Create Shader Stages
        std::vector<VkPipelineShaderStageCreateInfo> ShaderStages(
            out_pipeline_info.shaders.size()
        );

        for (uint32_t i = 0; i < out_pipeline_info.shaders.size(); i++)
        {
            ShaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            ShaderStages[i].stage = WVulkan::ToShaderStageFlagBits(
                out_pipeline_info.shaders[i].type
            );

            ShaderStages[i].module = out_pipeline_info.shaders[i].vk_shader_module;
            ShaderStages[i].pName = out_pipeline_info.shaders[i].entry_point.c_str();
        }

    }


};


