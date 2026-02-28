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
 * @brief Graphics Pipelines outputs the GBuffers.
 */
class WVkGBuffersPipelines : public WVkPipelinesBase<WAssetId, WEntityComponentId, WENG_MAX_FRAMES_IN_FLIGHT>
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

    WVkGBuffersPipelines() noexcept = default;

    virtual ~WVkGBuffersPipelines() override;

    WVkGBuffersPipelines(const WVkGBuffersPipelines & other)=delete;

    WVkGBuffersPipelines(
        WVkGBuffersPipelines && other
        ) noexcept;

    WVkGBuffersPipelines & operator=(
        const WVkGBuffersPipelines & other
        ) = delete;

    WVkGBuffersPipelines & operator=(
        WVkGBuffersPipelines && other
        ) noexcept;

    // Create Graphics Render Pipeline
    void CreatePipeline(
        const WAssetId & in_id,
        const WRenderPipelineStruct & in_pipeline_struct
        );

public:

    void Initialize(const VkDevice & in_device,
                    const VkPhysicalDevice & in_physical_device) override {
        Super::Initialize(in_device,
                          in_physical_device);
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

