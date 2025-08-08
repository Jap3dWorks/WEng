// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
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

private:

    struct WVkDescriptorSetUBOBinding {
        uint32_t binding{0}; // TODO binding to PipelineBindingInfo
        WVkUBOInfo ubo_info{};
        VkDescriptorBufferInfo buffer_info{};
    };

    struct WVkDescriptorSetTextureBinding {
        uint32_t binding{0};
        VkDescriptorImageInfo image_info{};
    };

    /**
     * @brief Render Pipeline Bindings data
     */
    struct WVkPipelineBindingInfo
    {
        WId wid{0};  // WEntityId

        WId render_pipeline_id{0};
        WId descriptor_set_id {0};
        WId mesh_asset_id{0};

        std::vector<WVkDescriptorSetTextureBinding> textures{};
        std::array<WVkDescriptorSetUBOBinding, WENG_MAX_FRAMES_IN_FLIGHT> ubo{};
    };

    /**
     * @Brief Graphics pipelines global descriptors sets for camera and lights.
     */
    struct GlobalGraphicsDescriptors {
        WVkDescriptorPoolInfo descpool_info{};
        WVkDescriptorSetLayoutInfo descset_layout_info{};
        WVkDescriptorSetInfo descset_info{};
        std::array<WVkUBOInfo, WENG_MAX_FRAMES_IN_FLIGHT> camera_ubo{};
    };

public:

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

    void UpdateGlobalGraphicsDescriptorSet(
        const WUBOCameraStruct & in_camera_struct,
        uint32_t in_frame_index
        ) ;

    const WVkDescriptorSetInfo & GlobalGraphicDescriptorSet() const {
        return global_graphics_descsets_.descset_info;
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

    void CreateDescriptorSet(
        const WAssetId & in_descriptor_set_layout_id,
        const WEntityComponentId & entity_component_id
        );
    
    void Initialize_ClearLambdas();

    void Initialize_GlobalGraphicDescriptors();

    void Destroy_GlobalGraphics();
    
    WVkRenderPipelineDb pipelines_{};
    WVkDescSetLayoutDb descriptor_set_layouts_{};
    WVkDesSetDb descriptor_sets_{};
    WVkPipelineBindingDb bindings_{};

    /** Relation between each pipeline and its bindings */
    std::unordered_map<WAssetId, TSparseSet<WEntityComponentId>> pipeline_pbindings_{};

    /** Pipelines grouped by PipelineType*/
    std::unordered_map<EPipelineType, TSparseSet<WAssetId>> ptype_pipelines_{};

    uint32_t width_{0};
    uint32_t height_{0};

    WVkDescriptorPoolInfo descriptor_pool_info_ {};

    GlobalGraphicsDescriptors global_graphics_descsets_{};

    WVkDeviceInfo device_info_ {};
    WVkRenderPassInfo render_pass_info_ {};

};

