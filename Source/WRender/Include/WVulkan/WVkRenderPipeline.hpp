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

/**
 * @brief A render pipeline is a collection of shaders and render states.
*/
class WRENDER_API WVkRenderPipelinesManager
{

    using WStagePipelineMaps = std::unordered_map<EPipelineType, std::vector<WId>>;

public:

    WVkRenderPipelinesManager();

    virtual ~WVkRenderPipelinesManager();

    WVkRenderPipelinesManager(
        WVkDeviceInfo device, 
        WVkRenderPassInfo render_pass_info
        );

    WVkRenderPipelinesManager(const WVkRenderPipelinesManager & other)=delete;

    WVkRenderPipelinesManager(
        WVkRenderPipelinesManager && other
        ) noexcept;

    WVkRenderPipelinesManager & operator=(
        const WVkRenderPipelinesManager & other
        )=delete;

    WVkRenderPipelinesManager & operator=(
        WVkRenderPipelinesManager && other
        ) noexcept;

    WId CreateDescriptorSetLayout();

    WId CreateDescriptorSet(WId in_descriptor_set_layout_id);

    WId CreateRenderPipeline(
        WVkRenderPipelineInfo in_render_pipeline_info,
        std::vector<WVkShaderStageInfo> in_shader_stages,
        WId in_descriptor_set_layout_id
        );

    WId AddBinding(
        WId in_pipeline_id,
        WId in_descriptor_set_id,
        const WVkMeshInfo & in_mesh_info
        );

    WNODISCARD std::vector<WId> & StagePipelines(EPipelineType in_type) WNOEXCEPT;

    WNODISCARD const std::vector<WVkPipelineBindingInfo> & PipelineBindings(WId in_pipeline_id) const;

    void Clear();

private:

    void Initialize();

    void Move(WVkRenderPipelinesManager && out_other);

    TObjectDataBase<WVkRenderPipelineInfo> render_pipelines_{};
    TObjectDataBase<WVkDescriptorSetLayoutInfo> descriptor_set_layouts_{};
    TObjectDataBase<WVkDescriptorSetInfo> descriptor_sets_{};

    // TObjectDataBase<WVkPipelineBindingInfo> pipeline_bindings_{};

    std::unordered_map<WId, std::vector<WVkPipelineBindingInfo>> pb_{};

    WStagePipelineMaps stage_pipelines_ {};

    WVkDescriptorPoolInfo descriptor_pool_info_ {};

    WVkDeviceInfo device_info_ {};
    WVkRenderPassInfo render_pass_info_ {};
    

};

