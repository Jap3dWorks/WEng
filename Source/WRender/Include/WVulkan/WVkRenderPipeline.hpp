// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"
#include "WVulkan/WVkRenderCore.hpp"
#include "WStructs/WRenderStructs.hpp"
#include <vector>
#include <unordered_map>
#include "WCore/TObjectDataBase.hpp"

/**
 * @brief A render pipeline is a collection of shaders and render states.
*/
class WRENDER_API WVkRenderPipelinesManager
{

public:

    WVkRenderPipelinesManager() noexcept {}

    virtual ~WVkRenderPipelinesManager();

    WVkRenderPipelinesManager(
        WVkDeviceInfo device, 
        WVkRenderPassInfo render_pass_info,
        uint32_t in_width,
        uint32_t in_height
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

    void CreateRenderPipeline(
        const WId & in_id,
        const WRenderPipelineStruct & pstruct
        );

    WId CreateBinding(
        const WId & component_id,
        const WId & in_pipeline_id,
        const WId & in_mesh_asset_id,
        std::vector<WVkTextureInfo> in_textures,
        std::vector<uint16_t> in_textures_bindings
        ) noexcept;

    WNODISCARD const WVkRenderPipelineInfo & RenderPipelineInfo(WId in_id) const {
        assert(pipelines_.Contains(in_id));
        return pipelines_.Get(in_id);
    }
    
    WNODISCARD const WVkDescriptorSetLayoutInfo & DescriptorSetLayout(WId in_id) const {
        assert(descriptor_set_layouts_.Contains(in_id));
        return descriptor_set_layouts_.Get(in_id);
    }
    
    WNODISCARD const WVkDescriptorSetInfo & DescriptorSet(WId in_id) const {
        assert(descriptor_sets_.Contains(in_id));
        return descriptor_sets_.Get(in_id);
    }
    
    WNODISCARD const WVkPipelineBindingInfo & Binding(WId in_id) const {
        assert(bindings_.Contains(in_id));
        return bindings_.Get(in_id);
    }

    WNODISCARD constexpr uint32_t Width() const noexcept { return width_; }

    void constexpr Width(uint32_t in_width) noexcept { width_ = in_width; }

    WNODISCARD constexpr uint32_t Heigth() const noexcept { return height_; }

    void constexpr Height(uint32_t in_height) noexcept { height_ = in_height; }

    void ForEachPipeline(EPipelineType in_type, TFunction<void(WId)> in_predicate);
    void ForEachPipeline(EPipelineType in_type, TFunction<void(WVkRenderPipelineInfo&)> in_predicate);
    void ForEachBinding(WId in_pipeline_id, TFunction<void(WId)> in_predicate);
    void ForEachBinding(WId in_pipeline_id, TFunction<void(WVkPipelineBindingInfo)> in_predicate);

    TIteratorPtr<WId> IteratePipelines(EPipelineType in_pipeline_type) {
        return {
            &(*stage_pipelines_[in_pipeline_type].begin()),
            &(*stage_pipelines_[in_pipeline_type].end())
        };
    }

    TIteratorPtr<WId> IterateBindings(WId in_pipeline_id) {
        return {
            &(*pipeline_bindings_[in_pipeline_id].begin()),
            &(*pipeline_bindings_[in_pipeline_id].end())
        };
    }

    /**
     * @brief Remove current active pipelines and bindings, resulting instance can be used.
     */
    void Clear();

    /**
     * @brief destroy the obejct, resulting instance is useless.
     */
    void Destroy();

private:

    // WId CreateDescriptorSetLayout();

    void CreateDescriptorSetLayout(const WId & in_id);

    WId CreateDescriptorSet(WId in_descriptor_set_layout_id);

    void InitializeClearLambdas();

    TObjectDataBase<WVkRenderPipelineInfo, void> pipelines_{};
    TObjectDataBase<WVkDescriptorSetLayoutInfo, void> descriptor_set_layouts_{};
    TObjectDataBase<WVkDescriptorSetInfo, void> descriptor_sets_{};
    TObjectDataBase<WVkPipelineBindingInfo, void> bindings_{};

    /** Relation between each pipeline and its bindings*/
    std::unordered_map<WId, std::vector<WId>> pipeline_bindings_{};

    /** Pipelines by grouped by PipelineType*/
    std::unordered_map<EPipelineType, std::vector<WId>> stage_pipelines_{};

    uint32_t width_{0};
    uint32_t height_{0};

    WVkDescriptorPoolInfo descriptor_pool_info_ {};

    WVkDeviceInfo device_info_ {};
    WVkRenderPassInfo render_pass_info_ {};

};

