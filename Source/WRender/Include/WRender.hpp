#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVkRenderCore.hpp"
#include "WVulkan/WVkRenderPipeline.hpp"
#include "WVulkan/WVkRenderCommandPool.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WVulkan/WVkRenderResources.hpp"

#include <cstddef>

// class IRenderResources;

/**
 * @brief Default Rendere class
*/
class WRENDER_API WRender : public IRender
{
public:

    WRender();

    ~WRender() override;

    void Initialize();

    void Draw() override ;

    void Destroy() override;

    void CreateRenderPipeline(
        WRenderPipelineAsset * in_pipeline_asset
        ) override;

    void DeleteRenderPipeline(const WAssetId & in_id) override;

    void CreatePipelineBinding(
        const WEntityComponentId & component_id,
        const WAssetId & pipeline_id,
        const WAssetId & in_mesh_id,
        const WRenderPipelineParametersStruct & in_parameters
        ) override ;

    void DeletePipelineBinding(const WEntityComponentId & in_id) override;

    WNODISCARD WVkRenderPipelinesManager & RenderPipelinesManager()
    {
        return pipelines_manager_;
    }

    void WaitIdle() const override;

    WNODISCARD GLFWwindow * Window() const noexcept override {
        return window_info_.window;
    }

    static void FrameBufferSizeCallback(GLFWwindow*, int, int);

    WNODISCARD const WVkWindowInfo & WindowInfo() const noexcept
    { return window_info_; }

    WNODISCARD const WVkDeviceInfo & DeviceInfo() const noexcept
    { return device_info_; }

    WNODISCARD const WVkSwapChainInfo & SwapChainInfo() const noexcept
    { return swap_chain_info_; }

    WNODISCARD const size_t FramesInFlight() const noexcept
    { return WENG_MAX_FRAMES_IN_FLIGHT; }

    WNODISCARD const WVkRenderCommandPool & RenderCommandPool() const noexcept
    { return render_command_pool_; }

    void ClearPipelines() override;

    void UnloadAllResources() override;

    void RegisterTexture(WTextureAsset & in_texture_asset) override {
        render_resources_.RegisterTexture(in_texture_asset);
    }

    void UnregisterTexture(const WId & in_id) override {
        render_resources_.UnregisterTexture(in_id);
    }

    void LoadTexture(const WId & in_id) override {
        render_resources_.LoadTexture(in_id);
    }

    void UnloadTexture(const WId & in_id) override {
        render_resources_.UnloadTexture(in_id);
    }

    void RegisterStaticMesh(WStaticMeshAsset & in_static_mesh_asset) override {
        render_resources_.RegisterStaticMesh(in_static_mesh_asset);
    }

    void UnregisterStaticMesh(const WId & in_id) override {
        render_resources_.UnregisterStaticMesh(in_id);
    }

    void LoadStaticMesh(const WId & in_id) override {
        render_resources_.LoadStaticMesh(in_id);
    }

    void UnloadStaticMesh(const WId & in_id) override {
        render_resources_.UnloadStaticMesh(in_id);
    }

private:

    void RecreateSwapChain();

    void RecordRenderCommandBuffer(WId in_pipeline_id,
                                   uint32_t in_frame_index,
                                   uint32_t in_image_index);

    WVkRenderResources render_resources_{};

    WVkInstanceInfo instance_info_;
    WVkWindowInfo window_info_;
    WVkSurfaceInfo surface_info_;
    WVkDeviceInfo device_info_;
    WVkRenderDebugInfo debug_info_;

    WVkSwapChainInfo swap_chain_info_;
    WVkRenderPassInfo render_pass_info_;

    WVkRenderCommandPool render_command_pool_;
    WVkCommandBufferInfo render_command_buffer_;
    
    WVkRenderPipelinesManager pipelines_manager_;

    WVkSemaphoreInfo image_available_semaphore_;
    WVkSemaphoreInfo render_finished_semaphore_;
    WVkFenceInfo flight_fence_;

    uint32_t frame_index {0};

    bool frame_buffer_resized {false};

};

