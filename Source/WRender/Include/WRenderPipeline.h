// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore/WCore.h"
#include "WRenderCore.h"
#include "vulkan/vulkan.h"
#include "WShader.h"
#include <vector>
#include <unordered_map>


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
