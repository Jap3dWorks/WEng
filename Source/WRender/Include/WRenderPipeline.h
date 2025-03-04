// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore/CoreMacros.h"
#include "WCore/WCore.h"
#include "WRenderCore.h"
#include "vulkan/vulkan.h"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <set>

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

    WNODISCARD WId WID() const;

    void WID(WId in_wid);

private:

    WId wid_;

    WRenderPipelineInfo render_pipeline_info_;
    WDeviceInfo device_info_;

    std::vector<WShaderStageInfo> shader_stage_infos_;

    void Move(WRenderPipeline && out_other);

    bool operator<(const WRenderPipeline & in_other) {
        return wid_.GetId() < in_other.wid_.GetId();
    }

    bool operator==(const WRenderPipeline & in_other) {
        return wid_.GetId() == in_other.wid_.GetId();
    }
    
};

/**
 * @brief A render pipeline is a collection of shaders and render states.
*/
class WRENDER_API WRenderPipelinesManager
{

    using WPipelineDataMaps = std::unordered_map<EPipelineType, std::vector<WRenderPipeline>>;

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

    const WDescriptorSetInfo & CreateDescriptorSet(
        const WDescriptorSetLayoutInfo & in_descriptor_set_layout
        );

    WRenderPipeline & CreateRenderPipeline(
        WRenderPipelineInfo in_render_pipeline_info,
        std::vector<WShaderStageInfo> in_shader_stages,
        const WDescriptorSetLayoutInfo & in_descriptor_set_layout_info
        );

    void AddBinding(
        WId in_pipeline_id,
        const WDescriptorSetInfo & in_descriptor_set_info,
        const WMeshInfo & in_mesh_info
        );

    WNODISCARD WPipelineDataMaps & RenderPipelines() WNOEXCEPT;

    WNODISCARD const std::vector<WPipelineBinding> & PipelineBindings(WId in_pipeline_id) const;

private:

    void Move(WRenderPipelinesManager && out_other);

    WDeviceInfo device_info_ {};
    WRenderPassInfo render_pass_info_ {};
    WDescriptorPoolInfo descriptor_pool_info_ {};

    std::vector<WDescriptorSetLayoutInfo> descriptor_set_layouts_ {};
    std::vector<WDescriptorSetInfo> descriptor_sets_ {};
    WPipelineDataMaps render_pipelines_ {};

    std::unordered_map<WId, std::vector<WPipelineBinding>> pipeline_bindings_ {};

    uint32_t pipelines_count_{0};
};

