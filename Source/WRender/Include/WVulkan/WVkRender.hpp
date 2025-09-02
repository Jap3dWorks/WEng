#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WStructs/WGeometryStructs.hpp"
#include "WStructs/WTextureStructs.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVkRenderStructs.hpp"
#include "WVulkan/WVkRenderPipeline.hpp"
#include "WVulkan/WVkRenderCommandPool.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WVulkan/WVkRenderResources.hpp"

#include <cstddef>
#include <vulkan/vulkan_core.h>

struct GLFWwindow;

/**
 * @brief Default Rendere class
*/
class WRENDER_API WVkRender : public IRender
{
public:

    WVkRender();

    WVkRender(GLFWwindow * in_window);

    ~WVkRender() override;

    void Initialize() override;

    void Draw() override;

    void Destroy() override;

    void WaitIdle() const override;

    // TODO pipeline struct and WAssetId
    void CreateRenderPipeline(
        WRenderPipelineAsset * in_pipeline_asset
        ) override;

    void CreatePipelineBinding(
        const WEntityComponentId & component_id,
        const WAssetId & pipeline_id,
        const WAssetIndexId & in_mesh_id,
        const WRenderPipelineParametersStruct & in_parameters
        ) override;

    void DeleteRenderPipeline(const WAssetId & in_id) override;

    void DeletePipelineBinding(const WEntityComponentId & in_id) override;

    void LoadTexture(const WAssetId & in_id, const WTextureStruct & in_texture) override {
        render_resources_.LoadTexture(in_id, in_texture);
    }

    void UnloadTexture(const WAssetId & in_id) override {
        render_resources_.UnloadTexture(in_id);
    }

    void LoadStaticMesh(const WAssetIndexId & in_id, const WMeshStruct & in_mesh) override {
        render_resources_.LoadStaticMesh(in_id, in_mesh);
    }

    void UnloadStaticMesh(const WAssetIndexId & in_id) override {
        render_resources_.UnloadStaticMesh(in_id);
    }

    void UpdateUboCamera(
        const WCameraStruct & camera_struct,
        const WTransformStruct & transform_struct
        ) override;

    /** only current frame index */
    void UpdateUboModelDynamic(
        const WEntityComponentId & in_component_id,
        const WTransformStruct & in_transform_struct
        ) override;

    /** both frame index */
    void UpdateUboModelStatic(
        const WEntityComponentId & in_component_id,
        const WTransformStruct & in_transform_struct
        ) override;

    void UnloadAllResources() override;

    void Window(GLFWwindow * in_window) override;

    void Rescale(const std::uint32_t & in_width,
                 const std::uint32_t & in_height) override;

    WNODISCARD WVkRenderPipelinesManager & RenderPipelinesManager()
    {
        return pipelines_manager_;
    }

    WNODISCARD const WVkDeviceInfo & DeviceInfo() const noexcept
    { return device_info_; }

    WNODISCARD const WVkSwapChainInfo & SwapChainInfo() const noexcept
    { return swap_chain_info_; }

    WNODISCARD const size_t FramesInFlight() const noexcept
    { return WENG_MAX_FRAMES_IN_FLIGHT; }

    WNODISCARD const WVkRenderCommandPool & RenderCommandPool() const noexcept
    { return render_command_pool_; }

    void ClearPipelines() override;

private:

    void RecreateSwapChain();

    void RecordRenderCommandBuffer(const WAssetId & in_pipeline_id,
                                   const std::uint32_t & in_frame_index,
                                   const std::uint32_t & in_image_index);

    VkDescriptorSet BindingDescriptor(const WEntityComponentId & in_binding_id,
                                      const std::uint32_t & in_frame_index);

    WVkInstanceInfo instance_info_;
    
    struct WVkRenderWindow {
        GLFWwindow * window{nullptr};
        std::uint32_t width{800};  // TODO size struct
        std::uint32_t height{600};
    } window_;

    WVkSurfaceInfo surface_info_;
    WVkDeviceInfo device_info_;
    WVkRenderDebugInfo debug_info_;

    WVkRenderResources render_resources_;

    WVkRenderPassInfo offscreen_rpass_info_;
    WVkRenderPassInfo postprocess_rpass_info_;

    WVkSwapChainInfo swap_chain_info_;

    WVkRenderCommandPool render_command_pool_;
    WVkCommandBufferInfo render_command_buffer_;
    
    WVkRenderPipelinesManager pipelines_manager_;

    WVkSemaphoreInfo image_available_semaphore_;
    WVkSemaphoreInfo render_finished_semaphore_;
    WVkFenceInfo flight_fence_;

    uint32_t frame_index_;
    // bool frame_buffer_resized_;

};

