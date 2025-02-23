// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore/WCore.h"
#include "WRenderCore.h"
#include "vulkan/vulkan.h"
#include <vector>
#include <unordered_map>

class WRENDER_API WRenderPipeline
{
public:

    WRenderPipeline();

    WRenderPipeline(
        const WDeviceInfo & in_device_info,
        const WDescriptorSetLayoutInfo & in_descriptor_set_layout_info,
        const WRenderPassInfo & in_render_pass_info,
        const WRenderPipelineInfo & in_pipeline_info,
        std::vector<WShaderStageInfo> in_shader_stages
	);

    ~WRenderPipeline();

    WRenderPipeline(WRenderPipeline && out_other);

    WRenderPipeline & operator=(WRenderPipeline && out_other);

    WRenderPipeline & operator=(const WRenderPipeline & in_render) = delete;
    WRenderPipeline(const WRenderPipeline & in_render) = delete;

    WNODISCARD WRenderPipelineInfo RenderPipelineInfo() WCNOEXCEPT;

private:

    WRenderPipelineInfo render_pipeline_info_;
    WDeviceInfo device_info_;

    std::vector<WShaderStageInfo> shader_stage_infos_;

    void Move(WRenderPipeline && out_other);
    
};

/**
 * @brief A render pipeline is a collection of shaders and render states.
*/
class WRENDER_API WRenderPipelinesManager
{

    using WPipelineData = std::unordered_map<EPipelineType, std::vector<WRenderPipeline>>;

public:

    WRenderPipelinesManager();

    ~WRenderPipelinesManager();

    WRenderPipelinesManager(
        WDeviceInfo & device, 
        WRenderPassInfo & render_pass_info
        );

    WRenderPipelinesManager(
        WRenderPipelinesManager && other
        );

    WRenderPipelinesManager & operator=(
        WRenderPipelinesManager && other
        );

    WDescriptorSetLayoutInfo & CreateDescriptorSetLayout();

    WRenderPipeline & CreateRenderPipeline(
        WRenderPipelineInfo render_pipeline_info,
        std::vector<WShaderStageInfo> in_shader_stages,
        const WDescriptorSetLayoutInfo& descriptor_set_layout_info
        );

    WPipelineData & RenderPipelines() WNOEXCEPT;

private:

    void Move(WRenderPipelinesManager && out_other);

    WDeviceInfo device_info_;
    WRenderPassInfo render_pass_info_;
    std::vector<WDescriptorSetLayoutInfo> descriptor_set_layouts_;
    WPipelineData render_pipelines_;

};

