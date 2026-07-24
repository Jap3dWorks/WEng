#pragma once

#include "WAssets/RenderPipelineParams.hpp"
#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
// #include "WCoreTypes/WTexture.hpp"
#include "WVulkan/WVkRAII/WVkAttachmentsLightingRAII.hpp"
#include "WVulkan/WVkRAII/WVkAttachmentsPostprocessRAII.hpp"
#include "WVulkan/WVkRAII/WVkAttachmentsTonemappingRAII.hpp"
#include "WVulkan/WVkRAII/WVkSwapchainRAII.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
// #include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVkRAII/WVkRenderPlaneRAII.hpp"
#include "WVulkan/WVkRAII/WVkGlobalDescriptorsRAII.hpp"
#include "WVulkan/WVkRAII/WVkPostprocessGlobalDescriptorRAII.hpp"
#include "WVulkan/WVkRAII/WVkGBufferPipelinesRAII.hpp"
#include "WVulkan/WVkRAII/WVkPostprocessPipelinesRAII.hpp"
#include "WVulkan/WVkRAII/WVkLightingPipelineRAII.hpp"
#include "WVulkan/WVkRAII/WVkTonemappingPipelineRAII.hpp"
#include "WVulkan/WVkRAII/WVkCommandPoolRAII.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WVulkan/WVkRAII/WVkAssetRenderDataRAII.hpp"
#include "WVulkan/WVkRAII/WVkSwapchainPipelineRAII.hpp"
#include "WVulkan/WVkRAII/WVkAttachmentsGBuffersRAII.hpp"
#include "WVulkan/WVkRAII/WVkRenderSyncRAII.hpp"

#include "WRender/WDenseLightingUBO.hpp"

#include "WVulkan/WVkRAII/WVkDeviceRAII.hpp"
#include "WVulkan/WVkRAII/WVkInstanceRAII.hpp"
#include "WVulkan/WVkRAII/WVkSurfaceRAII.hpp"

#include <cstddef>
#include <vulkan/vulkan_core.h>

// struct GLFWwindow;
namespace wdw { class WWindow; }

/**
 * @brief Default Render class
*/
class WRENDER_API WVkRender : public IRender
{
public:

    WNODISCARD static inline constexpr size_t FramesInFlight() noexcept
    { return WENG_MAX_FRAMES_IN_FLIGHT; }

    WVkRender() noexcept=default;

    ~WVkRender() override=default;

    WVkRender(const WVkRender &)=delete;
    WVkRender& operator=(const WVkRender&)=delete;

    WVkRender(WVkRender && other) noexcept = default;

    WVkRender & operator=(WVkRender && other) noexcept=default;

    void Draw() override;

    void WaitIdle() const override;

    void CreateRenderPipeline(
        was::RenderPipeline * in_pipeline_asset
        ) override;

    void CreatePipelineBinding(
        const wcr::wid::WEntityComponentId & component_id,
        const wcr::wid::WTypeAssetIndexId & in_mesh_id,
        const was::RenderPipeline & pipeline_id,
        const was::RenderPipelineParams & in_param_asset
        ) override;

    void DeleteRenderPipeline(const wcr::wid::WAssetId & in_id) override;

    void DeletePipelineBinding(const wcr::wid::WEntityComponentId & in_id) override;

    void RefreshPipelines() override;

    void LoadTexture(const wcr::wid::WAssetId & in_id,
                     const was::Texture & in_texture) override
    {
        asset_render_data_.LoadTexture(in_id, in_texture);
    }

    void UnloadTexture(const wcr::wid::WAssetId & in_id) override {
        asset_render_data_.UnloadTexture(in_id);
    }

    void LoadStaticMesh(const wcr::wid::WTypeAssetIndexId & in_id, const wct::geometry::WMesh & in_mesh) override {
        asset_render_data_.LoadStaticMesh(in_id, in_mesh);
    }

    void UnloadStaticMesh(const wcr::wid::WTypeAssetIndexId & in_id) override {
        asset_render_data_.UnloadStaticMesh(in_id);
    }

    /**
     * @brief Updates only for current frame in flight
     */
    void UpdateParameterDynamic(
        const wcr::wid::WEntityComponentId & in_component_id,
        const wct::render::RPipeParamUbo & ubo_write
        ) override;

    /**
     * @brief Updates for all frames in flight
     */
    void UpdateParameterStatic(
        const wcr::wid::WEntityComponentId & in_component_id,
        const wct::render::RPipeParamUbo & ubo_write
        ) override;

    void UnloadAllResources() override;

    void SetWindow(wdw::WWindow * in_window) override;

    void Rescale(const std::uint32_t & in_width,
                 const std::uint32_t & in_height) override;

    wct::render::RenderSize RenderSize() const override { return render_size_; }

    WNODISCARD VkDevice Device() const noexcept
    { return device_.Device(); }

    WNODISCARD const WVkCommandPoolRAII & RenderCommandPool() const noexcept
    { return command_pool_; }

    void ClearPipelines() override;

    // Camera
    // ------

    void UpdateUboCamera(
        const wct::render::CameraUBO & in_ubo
        ) override;

    // Lights
    // ------

    void InitializeLights(
        std::span<wcr::wid::WEntityComponentId> in_pl_ids,
        std::span<wct::render::PointLight> in_point_lights,
        std::span<wcr::wid::WEntityComponentId> in_dl_ids,
        std::span<wct::render::DirectionalLight> in_directional_lights,
        const wct::render::AmbientLight & in_ambient_light
        ) override;

    void ClearLights() override;

    void UpdatePointLights(
        std::span<wcr::wid::WEntityComponentId> in_ids,
        std::span<wct::render::PointLight> in_point_lights_structs
        ) override;

    void UpdateDirectionalLights(
        std::span<wcr::wid::WEntityComponentId> in_ids,
        std::span<wct::render::DirectionalLight> in_directional_light_structs
        ) override;

    void UpdateAmbientLight(
        const wct::render::AmbientLight & in_ambient_light
        ) override;

private:

    void Initialize();

    void RecreateSwapChain();

    // TODO move Record commands to an inline library

    void RecordGBuffersRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index
        );

    void RecordLightingRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index
        );

    void RecordPostprocessRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index,
        const std::uint32_t & in_image_index
        );

    void RecordTonemappingRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index,
        const std::uint32_t & in_image_index
        );

    void RecordSwapChainRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index,
        const std::uint32_t & in_image_index
        );

    wrd::light::WDenseLightingUBO lighting_UBO_{};

    WVkInstanceRAII instance_{};
    WVkSurfaceRAII surface_{};
    WVkDeviceRAII device_{};
    WVkSwapchainRAII swapchain_{};

    wdw::WWindow * window_{nullptr};

    WVkAssetRenderDataRAII asset_render_data_{};
    WVkRenderPlaneRAII render_plane_{};

    WVkAttachmentsGBuffersRAII<WENG_MAX_FRAMES_IN_FLIGHT> gbuffers_attachments_{};
    WVkAttachmentsLightingRAII<WENG_MAX_FRAMES_IN_FLIGHT> offscreen_attachments_{};
    WVkAttachmentsPostprocessRAII<WENG_MAX_FRAMES_IN_FLIGHT> postprocess_attachments_{};
    WVkAttachmentsTonemappingRAII<WENG_MAX_FRAMES_IN_FLIGHT> tonemapping_attachments_{};

    WVkSwapchainPipelineRAII<> swap_chain_pipeline_{};
    VkImageView swap_chain_input_imgview_ref{VK_NULL_HANDLE};

    WVkCommandPoolRAII command_pool_{};
    WVkCommandPoolRAII::CommandBuffers<WENG_MAX_FRAMES_IN_FLIGHT> render_command_buffers_{};

    WVkGlobalDescriptorsRAII<WENG_MAX_FRAMES_IN_FLIGHT> global_descriptors_{};
    WVkPostprocessGlobalDescriptorRAII<WENG_MAX_FRAMES_IN_FLIGHT> ppcess_global_descriptors_{};

    WVkGBufferPipelinesRAII<WENG_MAX_FRAMES_IN_FLIGHT> gbuffers_pipelines_{};
    WVkPostprocessPipelinesRAII ppcss_pipelines_{};
    WVkLightingPipelineRAII<> offscreen_pipeline_{};
    WVkTonemappingPipelineRAII<> tonemapping_pipeline_{};

    WVkRenderSyncRAII<WENG_MAX_FRAMES_IN_FLIGHT> render_sync_{};
    std::size_t semaphore_index_{0};

    struct PipelinesTrack {
        std::unordered_map<wcr::wid::WAssetId, wct::render::ERPipeType> pipeline_pipetype{};
        std::unordered_map<wcr::wid::WEntityComponentId, wct::render::ERPipeType> binding_pipetype{};
    } pipeline_track_{};


    uint32_t frame_index_{0};
    
    wct::render::RenderSize render_size_{
        800, 600
    };

};

