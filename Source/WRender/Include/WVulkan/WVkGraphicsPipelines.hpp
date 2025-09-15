// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WStructs/WRenderStructs.hpp"
#include <vector>
#include <unordered_map>
#include "WCore/TObjectDataBase.hpp"
#include "WVulkan/WVkPipelinesDb.hpp"

/**
 * @brief A render pipeline is a collection of shaders and render states.
 */
class WRENDER_API WVkGraphicsPipelines
{

private:

    /**
     * @Brief Graphics pipelines global descriptors sets for camera and lights.
     */
    struct GlobalGraphicsResources {
        WVkDescriptorPoolInfo descpool_info{};
        WVkDescriptorSetLayoutInfo descset_layout_info{};

        std::array<WVkDescriptorSetInfo, WENG_MAX_FRAMES_IN_FLIGHT> descset_info{};
        std::array<WVkUBOInfo, WENG_MAX_FRAMES_IN_FLIGHT> camera_ubo{};
    };

public:

    using WVkPipelineBindingDb = TObjectDataBase<WVkPipelineBindingInfo, void, WEntityComponentId>;

    WVkGraphicsPipelines() noexcept = default;

    virtual ~WVkGraphicsPipelines();

    WVkGraphicsPipelines(
        WVkDeviceInfo device 
        );

    WVkGraphicsPipelines(const WVkGraphicsPipelines & other)=delete;

    WVkGraphicsPipelines(
        WVkGraphicsPipelines && other
        ) noexcept;

    WVkGraphicsPipelines & operator=(
        const WVkGraphicsPipelines & other
        )=delete;

    WVkGraphicsPipelines & operator=(
        WVkGraphicsPipelines && other
        ) noexcept;

    // Create Graphics Render Pipeline
    void CreatePipeline(
        const WAssetId & in_id,
        const WRenderPipelineStruct & pstruct
        );

    void DeletePipeline(
        const WAssetId & in_id
        );

    void ResetDescriptorPool(const WAssetId & in_id, const std::uint32_t & in_frameindex);

    WEntityComponentId CreateBinding(
        const WEntityComponentId & component_id,
        const WAssetId & in_pipeline_id,
        const WAssetIndexId & in_mesh_asset_id,
        std::vector<WVkTextureInfo> in_textures,
        std::vector<uint16_t> in_textures_bindings
        ) noexcept;

    void DeleteBinding(const WEntityComponentId & in_id);

    WNODISCARD const WVkRenderPipelineInfo & Pipeline(const WAssetId & in_id) const {
        assert(pipelines_db_.pipelines.Contains(in_id));
        return pipelines_db_.pipelines.Get(in_id);
    }
    
    WNODISCARD const WVkDescriptorSetLayoutInfo & DescriptorSetLayout(const WAssetId & in_id) const {
        assert(pipelines_db_.descriptor_set_layouts.Contains(in_id));
        return pipelines_db_.descriptor_set_layouts.Get(in_id);
    }

    WNODISCARD const WVkDescriptorPoolInfo & DescriptorPool(const WAssetId & in_id,
                                                                const std::uint32_t & in_frameindex) const {
        assert(pipelines_db_.descriptor_pools.at(in_frameindex).Contains(in_id));
        return pipelines_db_.descriptor_pools.at(in_frameindex).Get(in_id);
    }

    WNODISCARD const WVkPipelineBindingInfo & Binding(const WEntityComponentId & in_id) const {
        assert(bindings_.Contains(in_id));
        return bindings_.Get(in_id);
    }

    template<CCallable<void, const WAssetId&, WVkRenderPipelineInfo&> TFn>
    void ForEachPipeline(TFn && in_fn) const {
        pipelines_db_.pipelines.ForEachIdValue(std::forward<TFn>(in_fn));
    }

    template<CCallable<void, const WEntityComponentId &> TFn>
    void ForEachBinding(const WAssetId & in_pipeline_id, TFn && in_predicate) const {
        for (const auto & wid : pipeline_pbindings_.at(in_pipeline_id)) {
            std::forward<TFn>(in_predicate)(wid);
        }
    }

    template<CCallable<void, const WVkPipelineBindingInfo &> TFn>
    void ForEachBinding(const WAssetId & in_pipeline_id, TFn && in_fn) const {
        for (const auto & wid : pipeline_pbindings_.at(in_pipeline_id)) {
            std::forward<TFn>(in_fn)(bindings_.Get(wid));
        }
    }

    auto IterPipelines() const {
        return pipelines_db_.pipelines.IterIndexes();
    }

    auto IterBindings(const WAssetId & in_pipeline_id) const {
        return WIteratorUtils::DefaultIteratorPtr<const WEntityComponentId>(
            &(*pipeline_pbindings_.at(in_pipeline_id).begin()),
            &(*pipeline_pbindings_.at(in_pipeline_id).end())
            );
    }

    void UpdateGlobalGraphicsDescriptorSet(
        const WUBOCameraStruct & in_camera_struct,
        uint32_t in_frame_index
        );

    const WVkDescriptorSetInfo & GlobalGraphicsDescriptorSet(
        const std::uint32_t & in_frame_index
        ) const
    {
        return global_graphics_descsets_.descset_info[in_frame_index];
    }

    void UpdateModelDescriptorSet(
        const WUBOModelStruct & in_ubo_model_struct,
        const WEntityComponentId & in_desc_set,
        uint32_t in_frame_index
        );

    void UpdateModelDescriptorSet(
        const WUBOModelStruct & in_ubo_model_struct,
        const WEntityComponentId & in_desc_set
        );

    /**
     * @brief Remove current active pipelines and bindings, resulting instance can be used.
     */
    void Clear();

    /**
     * @brief destroy the obejct, resulting instance is useless.
     */
    void Destroy();

private:

    void Initialize_ClearLambdas();

    void Initialize_GlobalGraphicDescriptors();

    void Destroy_GlobalGraphics();

    WVkPipelinesDb<WAssetId, WENG_MAX_FRAMES_IN_FLIGHT> pipelines_db_{};
    
    WVkPipelineBindingDb bindings_{};

    /** Relation between each pipeline and its bindings */
    std::unordered_map<WAssetId, TSparseSet<WEntityComponentId>> pipeline_pbindings_{};

    /** Camera, lights, ... */
    GlobalGraphicsResources global_graphics_descsets_{};

    WVkDeviceInfo device_info_{};

};

