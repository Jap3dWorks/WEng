// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "DllDef.h"
#include "WCore/WCore.h"
#include "WRenderCore.h"
#include "vulkan/vulkan.h"
#include "WShader.h"
#include <vector>
#include <unordered_map>

class WRENDER_API WRenderPipeline
{
public:

    WRenderPipeline()=delete;

    WRenderPipeline(
	const WDeviceInfo& in_device_info,
	const WDescriptorSetLayoutInfo& in_descriptor_set_layout_info,
	const WRenderPassInfo& in_render_pass_info,
	const WRenderPipelineInfo& in_pipeline_info,
	std::vector<WShaderStageInfo> in_shader_stages
	);

    ~WRenderPipeline();

private:

    WRenderPipelineInfo render_pipeline_info_;
    WDeviceInfo device_info_;

    std::vector<WShaderStageInfo> shader_stage_infos_;
    
};

/**
 * @brief A render pipeline is a collection of shaders and render states.
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

    WRenderPipeline& CreateRenderPipeline(
        WRenderPipelineInfo render_pipeline_info, 
        const WDescriptorSetLayoutInfo& descriptor_set_layout_info
    );

private:

    WDeviceInfo& device_info_;
    WRenderPassInfo& render_pass_info_;
    std::vector<WDescriptorSetLayoutInfo> descriptor_set_layouts_;
    std::unordered_map<WPipelineType, std::vector<WRenderPipeline>> render_pipelines_;

};
