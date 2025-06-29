// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore/CoreMacros.h"
#include "WCore/WCore.h"
#include "WCore/WIdPool.h"
#include "WVulkan/WVkRenderCore.h"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include "WCore/TObjectDataBase.hpp"

class WRENDER_API WVkRenderPipeline
{
public:

    WVkRenderPipeline();

    WVkRenderPipeline(
        const WVkDeviceInfo & in_device_info,
        const WVkDescriptorSetLayoutInfo & in_descriptor_set_layout_info,
        const WVkRenderPassInfo & in_render_pass_info,
        const WVkRenderPipelineInfo & in_pipeline_info,
        std::vector<WVkShaderStageInfo> in_shader_stages
	);

     ~WVkRenderPipeline();  // TODO Delete pipelines in the manager (can be copied)

    WVkRenderPipeline(WVkRenderPipeline && out_other);

    WVkRenderPipeline & operator=(WVkRenderPipeline && out_other);

    WVkRenderPipeline & operator=(const WVkRenderPipeline & in_render) = delete;
    WVkRenderPipeline(const WVkRenderPipeline & in_render) = delete;

    WNODISCARD WVkRenderPipelineInfo RenderPipelineInfo() const WNOEXCEPT;

    WNODISCARD WVkDescriptorSetLayoutInfo DescriptorSetLayoutInfo() const WNOEXCEPT;

    WNODISCARD WId WID() const;

    void WID(WId in_wid);

    

private:

    WId wid_;

    WVkRenderPipelineInfo render_pipeline_info_;
    WVkDescriptorSetLayoutInfo descriptor_set_layout_info;
    WVkDeviceInfo device_info_;

    std::vector<WVkShaderStageInfo> shader_stage_infos_;

    void Move(WVkRenderPipeline && out_other);

    bool operator<(const WVkRenderPipeline & in_other) {
        return wid_.GetId() < in_other.wid_.GetId();
    }

    bool operator==(const WVkRenderPipeline & in_other) {
        return wid_.GetId() == in_other.wid_.GetId();
    }
    
};

/**
 * @brief A render pipeline is a collection of shaders and render states.
*/
class WRENDER_API WVkRenderPipelinesManager
{

    using WStagePipelineMaps = std::unordered_map<EPipelineType, std::vector<WId>>;

public:

    WVkRenderPipelinesManager();

    ~WVkRenderPipelinesManager();

    WVkRenderPipelinesManager(
        WVkDeviceInfo device, 
        WVkRenderPassInfo render_pass_info
        );

    WVkRenderPipelinesManager(
        WVkRenderPipelinesManager && other
        );

    WVkRenderPipelinesManager & operator=(
        WVkRenderPipelinesManager && other
        );

    WVkDescriptorSetLayoutInfo & CreateDescriptorSetLayout();

    const WVkDescriptorSetInfo & CreateDescriptorSet(
        const WVkDescriptorSetLayoutInfo & in_descriptor_set_layout
        );

    WVkRenderPipeline & CreateRenderPipeline(
        WVkRenderPipelineInfo in_render_pipeline_info,
        std::vector<WVkShaderStageInfo> in_shader_stages,
        const WVkDescriptorSetLayoutInfo & in_descriptor_set_layout_info
        );

    void AddBinding(
        WId in_pipeline_id,
        const WVkDescriptorSetInfo & in_descriptor_set_info,
        const WVkMeshInfo & in_mesh_info
        );

    WNODISCARD std::vector<WId> & StagePipelines(EPipelineType in_type) WNOEXCEPT;

    WNODISCARD const std::vector<WVkPipelineBindingInfo> & PipelineBindings(WId in_pipeline_id) const;

    void Destroy();

private:

    void Move(WVkRenderPipelinesManager && out_other);

    TOjectDataBase<WVkRenderPipeline>

    std::unordered_map<WId, WVkRenderPipeline> render_pipelines_{};
    std::unordered_map<WId, WVkDescriptorSetLayoutInfo> descriptor_set_layouts_{};
    std::unordered_map<WId, WVkDescriptorSetInfo> descriptor_sets_{};

    // std::vector<WVkDescriptorSetLayoutInfo> descriptor_set_layouts_ {};
    // std::vector<WVkDescriptorSetInfo> descriptor_sets_ {};

    WStagePipelineMaps stage_pipelines_ {};

    std::unordered_map<WId, std::vector<WVkPipelineBindingInfo>> pipeline_bindings_ {};

    WIdPool pipelines_id_pool_{};
    WIdPool descriptor_set_layouts_id_pool_{};
    // WIdPool 

    WVkDeviceInfo device_info_ {};
    WVkRenderPassInfo render_pass_info_ {};
    WVkDescriptorPoolInfo descriptor_pool_info_ {};


};

