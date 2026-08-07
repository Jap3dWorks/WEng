#pragma once

#include "WAssets/RenderPipeline.hpp"
#include "WAssets/RenderPipelineParams.hpp"
#include "WCore/WCore.hpp"

#include "WVulkan/WVkConfig.hpp"

#include "WCore/WCoreMacros.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/WVkAttachmentsLightingRAII.hpp"
#include "WVulkan/RAII/WVkAttachmentsPostprocessRAII.hpp"
#include "WVulkan/RAII/WVkAttachmentsTonemappingRAII.hpp"
#include "WVulkan/RAII/WVkAttachmentsGBuffersRAII.hpp"
#include "WVulkan/RAII/ShadowMapAttachments.hpp"
#include "WVulkan/RAII/ShadowMapPipeline.hpp"
#include "WVulkan/RAII/WVkSwapchainRAII.hpp"
#include "WVulkan/RAII/WVkRenderPlaneRAII.hpp"
#include "WVulkan/RAII/WVkGlobalDescriptorsRAII.hpp"
#include "WVulkan/RAII/WVkPostprocessGlobalDescriptorRAII.hpp"
#include "WVulkan/RAII/Pipelines/GBuffer.hpp"
#include "WVulkan/RAII/Pipelines/Postprocess.hpp"

#include "WVulkan/RAII/Pipelines/_new_Postprocess.hpp"

#include "WVulkan/RAII/WVkLightingPipelineRAII.hpp"
#include "WVulkan/RAII/WVkTonemappingPipelineRAII.hpp"
#include "WVulkan/RAII/WVkCommandPoolRAII.hpp"
#include "WInterfaces/IRender.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WVulkan/RAII/UBOManager/DynamicUBOManager.hpp"
#include "WVulkan/RAII/WVkSwapchainPipelineRAII.hpp"
#include "WVulkan/RAII/WVkRenderSyncRAII.hpp"

#include "WRender/WDenseLightingUBO.hpp"

#include "WVulkan/RAII/WVkDeviceRAII.hpp"
#include "WVulkan/RAII/WVkInstanceRAII.hpp"
#include "WVulkan/RAII/WVkSurfaceRAII.hpp"

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
    { return WVK_MAX_FRAMES_IN_FLIGHT; }

    WVkRender() noexcept=default;

    ~WVkRender() override=default;

    WVkRender(const WVkRender &)=delete;
    WVkRender& operator=(const WVkRender&)=delete;

    WVkRender(WVkRender && other) noexcept = default;

    WVkRender & operator=(WVkRender && other) noexcept=default;

    void Draw() override;

    void WaitIdle() const override;

    void CreateRenderPipeline(
        was::RenderPipeline const & pipeline_asset
        ) override;

    void DeleteRenderPipeline(
        was::RenderPipeline const & render_pipeline
        ) override;

    void CreatePipelineBindingSet(
        std::size_t binding_set_collection,
        wcr::wid::WEngId binding_set_id,
        wcr::wid::WTypeAssetIndexId renderable_asset_id,
        was::RenderPipeline const & pipeline_id,
        was::RenderPipelineParams const & in_param_asset
        ) override;

    void DeletePipelineBindingSet(
        std::size_t binding_set_collection,
        wcr::wid::WEngId binding_set_id,
        was::RenderPipeline const & pipeline
        ) override;

    void RefreshPipelines() override;

    void LoadTexture(const wcr::wid::WAssetId & in_id,
                     const was::Texture & in_texture) override
    {
        asset_render_data_.LoadTexture(in_id, in_texture);
    }

    void UnloadTexture(const wcr::wid::WAssetId & in_id) override {
        asset_render_data_.UnloadTexture(in_id);
    }

    void LoadStaticMesh(const wcr::wid::WTypeAssetIndexId & in_id,
                        const wct::geometry::WMesh & in_mesh) override {
        asset_render_data_.LoadStaticMesh(in_id, in_mesh);
    }

    void UnloadStaticMesh(const wcr::wid::WTypeAssetIndexId & in_id) override {
        asset_render_data_.UnloadStaticMesh(in_id);
    }

    /**
     * @brief Updates only for current frame in flight
     */
    void UpdatePipelineBindingSetParameter_Dynamic(
        std::size_t binding_set_collection,
        wcr::wid::WEngId binding_set_id,
        was::RenderPipeline const & pipeline,
        wcr::wid::WSubIdxId param_descriptor_id,
        wct::render::RPipeParamUbo const & ubo_pipe_param
        ) override;

    /**
     * @brief Updates for all frames in flight
     */
    void UpdatePipelineBindingSetParameter_Static(
        std::size_t binding_set_collection,
        wcr::wid::WEngId binding_set_id,
        was::RenderPipeline const & pipeline,
        wcr::wid::WSubIdxId param_descriptor_id,
        wct::render::RPipeParamUbo const & ubo_pipe_param
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

    wrd::light::WDenseLightingUBO lighting_UBO_{};

    WVkInstanceRAII instance_{};
    WVkSurfaceRAII surface_{};
    WVkDeviceRAII device_{};
    WVkSwapchainRAII swap_chain_{};

    wdw::WWindow * window_{nullptr};

    wvk::raii::AssetRenderData asset_render_data_{};
    wvk::raii::ubo_manager::DynamicUBOManager<WVK_MAX_FRAMES_IN_FLIGHT> dynamic_ubo_manager_{};
    
    WVkRenderPlaneRAII render_plane_{};

    WVkAttachmentsGBuffersRAII<WVK_MAX_FRAMES_IN_FLIGHT> gbuffers_attachments_{};
    wvk::raii::ShadowMapAttachments<WVK_MAX_FRAMES_IN_FLIGHT> shadow_map_attachments_{};
    WVkAttachmentsLightingRAII<WVK_MAX_FRAMES_IN_FLIGHT> lighting_attachments_{};
    WVkAttachmentsPostprocessRAII<WVK_MAX_FRAMES_IN_FLIGHT> postprocess_attachments_{};
    WVkAttachmentsTonemappingRAII<WVK_MAX_FRAMES_IN_FLIGHT> tonemapping_attachments_{};

    WVkSwapchainPipelineRAII<WVK_MAX_FRAMES_IN_FLIGHT> swap_chain_pipeline_{};
    VkImageView swap_chain_input_imgview_{VK_NULL_HANDLE};

    WVkCommandPoolRAII command_pool_{};
    WVkCommandPoolRAII::CommandBuffers<WVK_MAX_FRAMES_IN_FLIGHT> render_command_buffers_{};

    WVkGlobalDescriptorsRAII<WVK_MAX_FRAMES_IN_FLIGHT> global_descriptors_{};
    WVkPostprocessGlobalDescriptorRAII<WVK_MAX_FRAMES_IN_FLIGHT> ppcess_global_descriptors_{};

    wvk::raii::pipelines::GBuffer<WVK_MAX_FRAMES_IN_FLIGHT> gbuffers_pipelines_{};
    wvk::raii::ShadowMapPipeline<WVK_MAX_FRAMES_IN_FLIGHT> shadow_map_pipelines_{};
    WVkLightingPipelineRAII<WVK_MAX_FRAMES_IN_FLIGHT> lighting_pipeline_{};

    wvk::raii::pipelines::_new_Postprocess<WVK_MAX_FRAMES_IN_FLIGHT> ppcess_pipelines_{};
    // wvk::raii::pipelines::Postprocess ppcess_pipelines_{};

    WVkTonemappingPipelineRAII<WVK_MAX_FRAMES_IN_FLIGHT> tonemapping_pipeline_{};

    WVkRenderSyncRAII<WVK_MAX_FRAMES_IN_FLIGHT> render_sync_{};
    std::size_t semaphore_index_{0};

    std::uint32_t frame_index_{0};
    
    wct::render::RenderSize render_size_{
        800, 600
    };

};

