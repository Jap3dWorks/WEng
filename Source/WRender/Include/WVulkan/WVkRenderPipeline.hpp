// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore/CoreMacros.hpp"
#include "WCore/WCore.h"
#include "WCore/WIdPool.h"
#include "WVulkan/WVkRenderCore.hpp"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include "WCore/TObjectDataBase.hpp"

/**
 * @brief A render pipeline is a collection of shaders and render states.
*/
class WRENDER_API WVkRenderPipelinesManager
{

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

    const WVkRenderPipelineInfo & RenderPipelineInfo(WId in_id);
    const WVkDescriptorSetLayoutInfo & DescriptorSetLayout(WId in_id);
    const WVkDescriptorSetInfo & DescriptorSet(WId in_id);
    const WVkPipelineBindingInfo & Binding(WId in_id);

    void ForEachPipeline(EPipelineType in_type, TFunction<void(WId)> in_predicate);
    void ForEachPipeline(EPipelineType in_type, TFunction<void(WVkRenderPipelineInfo&)> in_predicate);
    void ForEachBinding(WId in_pipeline_id, TFunction<void(WId)> in_predicate);
    void ForEachBinding(WId in_pipeline_id, TFunction<void(WVkPipelineBindingInfo)> in_predicate);

    void Clear();

private:

    void Initialize();

    void Move(WVkRenderPipelinesManager && out_other);

    TObjectDataBase<WVkRenderPipelineInfo> render_pipelines_{};
    TObjectDataBase<WVkDescriptorSetLayoutInfo> descriptor_set_layouts_{};
    TObjectDataBase<WVkDescriptorSetInfo> descriptor_sets_{};

    TObjectDataBase<WVkPipelineBindingInfo> bindings_{};

    /** Relation between each pipeline and its bindings*/
    std::unordered_map<WId, std::vector<WId>> pipeline_bindings_{};

    /** Pipelines by grouped by PipelineType*/
    std::unordered_map<EPipelineType, std::vector<WId>> stage_pipelines_{};

    WVkDescriptorPoolInfo descriptor_pool_info_ {};

    WVkDeviceInfo device_info_ {};
    WVkRenderPassInfo render_pass_info_ {};
    

};

