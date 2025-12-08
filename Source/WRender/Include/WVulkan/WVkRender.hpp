#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WStructs/WGeometryStructs.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WStructs/WTextureStructs.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVkGraphicsPipelines.hpp"
#include "WVulkan/WVkPostprocessPipelines.hpp"
#include "WVulkan/WVkOffscreenPipeline.hpp"
#include "WVulkan/WVkRenderCommandPool.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WVulkan/WVkAssetResources.hpp"
#include "WVkSwapChainPipeline.hpp"

#include <cstddef>
#include <vulkan/vulkan_core.h>

struct GLFWwindow;

/**
 * @brief Default Rendere class
*/
class WRENDER_API WVkRender : public IRender
{
public:

    WVkRender() noexcept;

    WVkRender(WVkRender && other) noexcept;

    WVkRender & operator=(WVkRender && other) noexcept;

    WVkRender(GLFWwindow * in_window);

    ~WVkRender() override;

    void Initialize() override;

    void Draw() override;

    void Destroy() override;

    void WaitIdle() const override;

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

    void RefreshPipelines() override;

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
        const WUBOCameraStruct & in_ubo
        ) override;

    void UpdateParameterDynamic(
        const WEntityComponentId & in_component_id,
        const WRPParamUboStruct & ubo_write
        ) override;

    void UpdateParameterStatic(
        const WEntityComponentId & in_component_id,
        const WRPParamUboStruct & ubo_write
        ) override;

    void UnloadAllResources() override;

    void Window(GLFWwindow * in_window) override;

    void Rescale(const std::uint32_t & in_width,
                 const std::uint32_t & in_height) override;

    WRenderSize RenderSize() const override { return render_size_; }

    void RenderSize(const WRenderSize & in_render_size) override { render_size_ = in_render_size; }

    WNODISCARD WVkGraphicsPipelines & RenderPipelinesManager()
    {
        return graphics_pipelines_;
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

    void RecordGBuffersRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index
        );

    void RecordOffscreenRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index
        );

    void RecordPostprocessRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index,
        const std::uint32_t & in_image_index
        );

    WVkInstanceInfo instance_info_{};

    struct WVkRenderWindow {
        GLFWwindow * window{nullptr};
    } window_{};

    WRenderSize render_size_{
        800, 600
    };

    WVkSurfaceInfo surface_info_{};
    WVkDeviceInfo device_info_{};
    WVkRenderDebugInfo debug_info_{};

    WVkAssetResources render_resources_{};

    std::array<WVkGBuffersRenderStruct, WENG_MAX_FRAMES_IN_FLIGHT> gbuffers_rtargets_{};
    std::array<WVkOffscreenRenderStruct, WENG_MAX_FRAMES_IN_FLIGHT> offscreen_rtargets_{};
    std::array<WVkPostprocessRenderStruct, WENG_MAX_FRAMES_IN_FLIGHT> postprocess_rtargets_{};
    std::array<WVkTonemappingRenderStruct, WENG_MAX_FRAMES_IN_FLIGHT> tonemapping_rtargets_{};

    WVkSwapChainInfo swap_chain_info_{};
    WVkSwapChainPipeline<> swap_chain_pipeline_{};

    WVkRenderCommandPool render_command_pool_{};
    WVkCommandBufferInfo render_command_buffer_{};

    // Pipelines, Maybe I should create a container class/struct.
    WVkGraphicsPipelines graphics_pipelines_{};
    WVkOffscreenPipeline<> offscreen_pipeline_{};
    WVkPostprocessPipelines ppcss_pipelines_{};
    struct PipelinesTrack {
        std::unordered_map<WAssetId, EPipelineType> pipeline_ptype{};
        std::unordered_map<WEntityComponentId, EPipelineType> binding_ptype{};
    } pipeline_track_;
    // --

    struct SyncSemaphores {
        VkSemaphore image_available{VK_NULL_HANDLE};
        VkSemaphore render_finished{VK_NULL_HANDLE};
    };
    std::vector<SyncSemaphores> sync_semaphores_{};
    std::size_t semaphore_index_{0};
    
    std::array<VkFence, WENG_MAX_FRAMES_IN_FLIGHT> sync_fences_{};

    uint32_t frame_index_{0};

};

