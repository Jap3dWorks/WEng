// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"
#include "WVulkan/WVkRenderStructs.hpp"
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

    // using WIdType = WEntityComponentId;

    using WVkRenderPipelineDb = TObjectDataBase<WVkRenderPipelineInfo, void, WAssetId>;
    using WVkDescSetLayoutDb = TObjectDataBase<WVkDescriptorSetLayoutInfo, void, WAssetId>;
    using WVkDesSetDb = TObjectDataBase<WVkDescriptorSetInfo, void, WEntityComponentId>;
    using WVkPipelineBindingDb = TObjectDataBase<WVkPipelineBindingInfo, void, WEntityComponentId>;

    WVkRenderPipelinesManager() noexcept = default;

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
        const WAssetId & in_id,
        const WRenderPipelineStruct & pstruct  // TODO Camera UBO
        );

    void DeleteRenderPipeline(
        const WAssetId & in_id
        );

    WEntityComponentId CreateBinding(
        const WEntityComponentId & component_id,
        const WAssetId & in_pipeline_id,
        const WAssetId & in_mesh_asset_id,
        std::vector<WVkTextureInfo> in_textures,
        std::vector<uint16_t> in_textures_bindings
        ) noexcept;

    void DeleteBinding(const WEntityComponentId & in_id);

    WNODISCARD const WVkRenderPipelineInfo & RenderPipelineInfo(const WAssetId & in_id) const {
        assert(pipelines_.Contains(in_id));
        return pipelines_.Get(in_id);
    }
    
    WNODISCARD const WVkDescriptorSetLayoutInfo & DescriptorSetLayout(const WAssetId & in_id) const {
        assert(descriptor_set_layouts_.Contains(in_id));
        return descriptor_set_layouts_.Get(in_id);
    }
    
    WNODISCARD const WVkDescriptorSetInfo & DescriptorSet(const WEntityComponentId & in_id) const {
        assert(descriptor_sets_.Contains(in_id));
        return descriptor_sets_.Get(in_id);
    }
    
    WNODISCARD const WVkPipelineBindingInfo & Binding(const WEntityComponentId & in_id) const {
        assert(bindings_.Contains(in_id));
        return bindings_.Get(in_id);
    }

    WNODISCARD constexpr uint32_t Width() const noexcept { return width_; }

    void constexpr Width(uint32_t in_width) noexcept { width_ = in_width; }

    WNODISCARD constexpr uint32_t Heigth() const noexcept { return height_; }

    void constexpr Height(uint32_t in_height) noexcept { height_ = in_height; }

    void ForEachPipeline(EPipelineType in_type,
                         TFunction<void(const WAssetId &)> in_predicate);
    
    void ForEachPipeline(EPipelineType in_type,
                         TFunction<void(WVkRenderPipelineInfo&)> in_predicate);
    
    void ForEachBinding(const WAssetId & in_pipeline_id,
                        TFunction<void(const WEntityComponentId &)> in_predicate);
    
    void ForEachBinding(const WAssetId & in_pipeline_id,
                        TFunction<void(WVkPipelineBindingInfo)> in_predicate);

    TIteratorPtr<WAssetId> IteratePipelines(EPipelineType in_pipeline_type) {
        return {
            &(*ptype_pipelines_[in_pipeline_type].begin()),
            &(*ptype_pipelines_[in_pipeline_type].end())
        };
    }

    TIteratorPtr<WEntityComponentId> IterateBindings(const WAssetId & in_pipeline_id) {
        return {
            &(*pipeline_pbindings_[in_pipeline_id].begin()),
            &(*pipeline_pbindings_[in_pipeline_id].end())
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

    void CreateGraphicDescriptorSetLayout(const WAssetId & in_id);

    WEntityComponentId CreateDescriptorSet(const WAssetId & in_descriptor_set_layout_id);

    void InitializeClearLambdas();
    
    WVkRenderPipelineDb pipelines_{};
    WVkDescSetLayoutDb descriptor_set_layouts_{};
    WVkDesSetDb descriptor_sets_{};
    WVkPipelineBindingDb bindings_{};

    /** Relation between each pipeline and its bindings*/
    std::unordered_map<WAssetId, TSparseSet<WEntityComponentId>> pipeline_pbindings_{};

    /** Pipelines grouped by PipelineType*/
    std::unordered_map<EPipelineType, TSparseSet<WAssetId>> ptype_pipelines_{};

    uint32_t width_{0};
    uint32_t height_{0};

    WVkDescriptorPoolInfo descriptor_pool_info_ {};

    WVkDeviceInfo device_info_ {};
    WVkRenderPassInfo render_pass_info_ {};

};

