// a render pipeline is a collection of shaders and render states.
// It consist on a shader for each stage of the pipeline

#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WStructs/WRenderStructs.hpp"
#include <vector>
#include "WVulkan/WVkPipelinesBase.hpp"

/**
 * @brief A render pipeline is a collection of shaders and render states.
 */
class WVkGraphicsPipelines : public WVkPipelinesBase<WAssetId, WEntityComponentId, WENG_MAX_FRAMES_IN_FLIGHT>
{

private:

    /**
     * @Brief Graphics pipelines global descriptors sets for camera and lights.
     */
    struct GlobalGraphicsResources {
        WVkDescriptorPoolInfo descpool_info{};
        WVkDescriptorSetLayoutInfo descset_layout_info{};

        std::array<WVkDescriptorSetInfo, frames_in_flight> descset_info{};
        std::array<WVkUBOInfo, frames_in_flight> camera_ubo{};
    };

public:

    using Super = WVkPipelinesBase<WAssetId, WEntityComponentId, frames_in_flight>;

    WVkGraphicsPipelines() noexcept = default;

    virtual ~WVkGraphicsPipelines() override;

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
        const WRenderPipelineStruct & in_pipeline_struct
        );

public:

    void Initialize(const WVkDeviceInfo & in_device) override {
        Super::Initialize(in_device);
        Initialize_GlobalResources();
    }

    WEntityComponentId CreateBinding(
        const WEntityComponentId & in_binding_id,
        const WAssetId & in_pipeline_id,
        const WAssetIndexId & in_mesh_asset_id,
        const std::vector<WVkDescriptorSetUBOWriteStruct> & in_ubos,
        const std::vector<WVkDescriptorSetTextureWriteStruct> & in_texture
        );

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

    /**
     * @brief destroy the obejct, resulting instance is useless.
     */
    void Destroy();

private:

    void Initialize_GlobalResources();

    void Destroy_GlobalResources();

    /** Camera, lights, ... */
    GlobalGraphicsResources global_graphics_descsets_{};

};

