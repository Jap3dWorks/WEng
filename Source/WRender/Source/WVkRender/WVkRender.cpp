#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include "WCore/WId.hpp"
#endif

#include "WVulkan/WVkRender.hpp"

#include <GLFW/glfw3.h>

#include "WAssets/RenderPipeline.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "RenderUtils.hpp"
#include "WWindow/WWindow.hpp"
#include "WCore/TVisitor.hpp"
#include "PipelineBindings.hpp"
#include "RecordDrawCommands.hpp"

#include "WLog.hpp"

#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include <array>

// WVkRender
// -------

void WVkRender::WaitIdle() const
{
    vkDeviceWaitIdle(device_.Device());
}

void WVkRender::SetWindow(wdw::WWindow * in_window) {
    window_ = in_window;

    auto size = window_->GetFramebufferSize();
    
    render_size_={.width=size.width,
                  .height=size.height};

    Initialize();
}

void WVkRender::Initialize()
{
    WVkRenderDebugInfo render_debug_info =
        // wvk::render::CreateWVkRenderDebugInfo(WENG_VK_ENABLE_VALIDATION_LAYERS);
        wvk::render::CreateWVkRenderDebugInfo(true);

    std::array<std::uint32_t,2> dimensions = {
        render_size_.width,
        render_size_.height
    };

    // Create Vulkan Instance
    instance_ = WVkInstanceRAII(
        {},
        render_debug_info.enable_validation_layers,
        render_debug_info.validation_layers,
        render_debug_info.debug_callback,
        render_debug_info.debug_messenger
        );

    // Create Vulkan Window Surface
    surface_ = WVkSurfaceRAII(
        {*instance_},
        window_
        );

    // Create Vulkan Device
    device_ = WVkDeviceRAII(
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME
        },
        instance_.Value(),
        surface_.Value(),
        render_debug_info.enable_validation_layers,
        render_debug_info.validation_layers
        );

    swap_chain_ = WVkSwapchainRAII(
        device_.Device(),
        device_.PhysicalDevice(),
        surface_.Value(),
        dimensions[0],
        dimensions[1]
        );

    // GBuffers Attachments

    gbuffers_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        WVK_GBUFFER_RENDER_COLOR_FORMAT,
        WVK_GBUFFER_RENDER_EMISSION_FORMAT,
        WVK_GBUFFER_RENDER_NORMAL_FORMAT,
        WVK_GBUFFER_RENDER_ORM_FORMAT,
        WVK_GBUFFER_RENDER_DEPTH_FORMAT,
        WVK_GBUFFER_RENDER_EXTRA01_FORMAT,
    };

    // Lighting Attachments

    lighting_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        WVK_LIGHTING_RENDER_COLOR_FORMAT        
    };

    // Postprocess Attachments

    postprocess_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        WVK_POSTPROCESS_RENDER_COLOR_FORMAT        
    };

    // tonemapping Attachments

    tonemapping_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        swap_chain_.Format()
    };
    
    // Create Render Command Pool

    command_pool_ = WVkCommandPoolRAII( 
        device_.Device(),
        device_.PhysicalDevice(),
        surface_.Value()
        );

    render_plane_ = WVkRenderPlaneRAII(
        device_.Device(),
        device_.PhysicalDevice(),
        device_.GraphicsQueue(),
        command_pool_.Value()
        );

    WFLOG("[DEBUG] Initialize Global Descriptor Set.");

    global_descriptors_ = {
        device_.Device(),
        device_.PhysicalDevice()
    };

    WFLOG("[DEBUG] Initialize Postprocess Global Descriptor Set.");

    ppcess_global_descriptors_ = {
        device_.Device()
    };

    WFLOG("[DEBUG] Initialize GBuffer Pipelines.");

    gbuffers_pipelines_ = {
        device_.Device(),
        device_.PhysicalDevice()
    };

    WFLOG("[DEBUG] Initialize Lighting Pipeline.");

    lighting_pipeline_ = {
        device_.Device(),
        global_descriptors_.DescriptorSetLayout()
    };

    WFLOG("[DEBUG] Initialize Postprocess Pipelines.");

    ppcss_pipelines_ = {
        device_.Device(),
        device_.PhysicalDevice()
    };

    WFLOG("[DEBUG] Initialize tonemapping pipeline");

    tonemapping_pipeline_ = {
        device_.Device(),
        swap_chain_.Format()
    };
    
    WFLOG("[DEBUG] Initialize swap chain pipeline");

    swap_chain_pipeline_ = {
        device_.Device(),
        swap_chain_.Format()
    };

    render_command_buffers_ =
        command_pool_.
        CreateCommandBuffers();

    render_sync_ = {device_.Device(),
                   swap_chain_.Images().size()};
    
    asset_render_data_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        device_.GraphicsQueue(),
        command_pool_.Value()
    };

    wvk::render::UpdatePPcessGlobalDescriptorSet(
        ppcess_global_descriptors_,
        gbuffers_attachments_,
        lighting_attachments_,
        render_plane_.Sampler()
        );
}

void WVkRender::Draw()
{
    vkWaitForFences(
        device_.Device(),
        1,
        &render_sync_.Fence(frame_index_),
        VK_TRUE,
        UINT64_MAX
        );
    
    uint32_t image_index;

    VkResult result = vkAcquireNextImageKHR(
        device_.Device(),
        swap_chain_.Swapchain(),
        UINT64_MAX,
        render_sync_.ImageAvailableSemaphore(semaphore_index_),
        VK_NULL_HANDLE,
        &image_index
        );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquiera swap chain image!");
    }

    vkResetFences(
        device_.Device(),
        1,
        &render_sync_.Fence(frame_index_)
        );

    // Begin command buffer

    wvk::render::BeginRenderCommandBuffer(
        render_command_buffers_[frame_index_]
        );

    // RecordGBuffersRenderCommandBuffer(
    //     render_command_buffers_[frame_index_],
    //     frame_index_);

    wvk::render::rec_cmd_bffr::GBuffers(
        device_.Device(),
        render_command_buffers_[frame_index_],
        frame_index_,
        gbuffers_attachments_,
        gbuffers_pipelines_,
        asset_render_data_,
        global_descriptors_
        );

    wvk::render::rec_cmd_bffr::Lighting(
        device_.Device(),
        render_command_buffers_[frame_index_],
        frame_index_,
        lighting_attachments_,
        lighting_pipeline_,
        gbuffers_attachments_,
        global_descriptors_,
        render_plane_.RenderPlane(),
        render_plane_.Sampler()
        );

    swap_chain_input_imgview_ = wvk::render::rec_cmd_bffr::Postprocess(
        device_.Device(),
        render_command_buffers_[frame_index_],
        frame_index_,
        postprocess_attachments_,
        ppcss_pipelines_,
        lighting_attachments_,
        gbuffers_attachments_,
        ppcess_global_descriptors_,
        global_descriptors_,
        render_plane_.RenderPlane(),
        render_plane_.Sampler()
        );

    swap_chain_input_imgview_ = wvk::render::rec_cmd_bffr::Tonemapping(
        device_.Device(),
        render_command_buffers_[frame_index_],
        frame_index_,
        tonemapping_attachments_,
        tonemapping_pipeline_,
        swap_chain_input_imgview_,
        render_plane_.RenderPlane(),
        render_plane_.Sampler()
        );

    wvk::render::rec_cmd_bffr::SwapChain(
        device_.Device(),
        render_command_buffers_[frame_index_],
        frame_index_,
        image_index,
        swap_chain_,
        swap_chain_pipeline_,
        swap_chain_input_imgview_,
        render_plane_.RenderPlane(),
        render_plane_.Sampler()
        );

    // End Command buffer

    wvk::render::EndRenderCommandBuffer(
        render_command_buffers_[frame_index_]
        );

    VkSubmitInfo submit_info = wvk::types::VkSubmitInfo();

    VkPipelineStageFlags wait_stages[] =
        { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores =
        &render_sync_.ImageAvailableSemaphore(semaphore_index_);
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers =
        &render_command_buffers_[frame_index_];

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores =
        &render_sync_.RenderFinishedSempahore(image_index);

    wvk::vulkan::ExecVkProcChecked(
        vkQueueSubmit,
        "Failed to submit draw command buffer",
        device_.GraphicsQueue(),
        1,
        &submit_info,
        render_sync_.Fence(frame_index_)
        );

    VkPresentInfoKHR present_info = wvk::types::VkPresentInfoKHR();
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_sync_.RenderFinishedSempahore(image_index);

    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swap_chain_.Swapchain();
    present_info.pImageIndices = &image_index;
    present_info.pResults = VK_NULL_HANDLE;

    result = vkQueuePresentKHR(
        device_.PresentQueue(),
        &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    semaphore_index_ = (semaphore_index_ + 1) % swap_chain_.Images().size();
    frame_index_ = (frame_index_ + 1) % WVK_MAX_FRAMES_IN_FLIGHT;
}

// Pipelines
// ---------

void WVkRender::CreateRenderPipeline(
    was::RenderPipeline * render_pipeline
    ) {

    wct::render::pipeline_type_dispatcher<
        wct::render::ERPipeType::Graphics,
        wct::render::ERPipeType::GBuffer,
        wct::render::ERPipeType::Postprocess>
        (
            render_pipeline->Get_pipeline_type(),
            [&,this](){
                gbuffers_pipelines_.CreatePipeline(
                    render_pipeline->Get_asset_id(),
                    *render_pipeline,
                    global_descriptors_.DescriptorSetLayout()
                    );
            },
            [&,this](){
                gbuffers_pipelines_.CreatePipeline(
                    render_pipeline->Get_asset_id(),
                    *render_pipeline,
                    global_descriptors_.DescriptorSetLayout()
                    );
            },
            [&,this](){
                ppcss_pipelines_.CreatePipeline(
                    render_pipeline->Get_asset_id(),
                    *render_pipeline,
                    global_descriptors_.DescriptorSetLayout(),
                    ppcess_global_descriptors_.DescriptorSetLayout()
                    );
            }
            );

    pipeline_track_.pipeline_pipetype[render_pipeline->Get_asset_id()] =
        render_pipeline->Get_pipeline_type();

}

void WVkRender::DeleteRenderPipeline(const wcr::wid::WAssetId & in_id) {

    auto clearbindingfn = [this](const wcr::wid::WEntityComponentId & binding) {
        pipeline_track_.binding_pipetype.erase(binding);
    };

    wct::render::pipeline_type_dispatcher<
        wct::render::ERPipeType::Graphics,
        wct::render::ERPipeType::GBuffer,
        wct::render::ERPipeType::Postprocess
        >
        (pipeline_track_.pipeline_pipetype[in_id],
         [&,this]() {
             gbuffers_pipelines_.ForEachBinding(
                 in_id, clearbindingfn
                 );
             gbuffers_pipelines_.DeletePipeline(
                 in_id
                 );
         },
         [&,this]() {
             gbuffers_pipelines_.ForEachBinding(
                 in_id, clearbindingfn
                 );
             gbuffers_pipelines_.DeletePipeline(
                 in_id
                 );
         },
         [&,this]() {
             ppcss_pipelines_.ForEachBinding(
                 in_id, clearbindingfn
                 );
             gbuffers_pipelines_.DeletePipeline(
                 in_id
                 );
         }
            );

    pipeline_track_.pipeline_pipetype.erase(in_id);

}

void WVkRender::CreatePipelineBinding(
    const wcr::wid::WEntityComponentId & component_id,
    const wcr::wid::WTypeAssetIndexId & renderable_asset_id, // like a submesh
    const was::RenderPipeline & pipeline,
    const was::RenderPipelineParams & parameters
    )
{
    assert(pipeline_track_.pipeline_pipetype.contains(pipeline.Get_asset_id()));

    auto textures_bindings = wvk::render::pipe_bindings::CollectTextureBindings(
        parameters,
        asset_render_data_
        );

    auto ubo_bindings = wvk::render::pipe_bindings::CollectUBOBindings<FramesInFlight()>(
        component_id,
        pipeline,
        parameters,
        asset_render_data_
        );

    wvk::render::pipe_bindings::CreateBindingSet(
        pipeline_track_.pipeline_pipetype[pipeline.Get_asset_id()],
        component_id,
        renderable_asset_id,
        pipeline.Get_asset_id(),
        gbuffers_pipelines_,
        ppcss_pipelines_,
        std::move(ubo_bindings),
        std::move(textures_bindings)
        );

    pipeline_track_.binding_pipetype[component_id] =
        pipeline_track_.pipeline_pipetype[pipeline.Get_asset_id()];
    
}

void WVkRender::DeletePipelineBinding(const wcr::wid::WEntityComponentId & in_id) {

    wct::render::pipeline_type_dispatcher<
        wct::render::ERPipeType::Graphics,
        wct::render::ERPipeType::GBuffer,
        wct::render::ERPipeType::Postprocess>
        (
            pipeline_track_.binding_pipetype[in_id],
            [&,this](){ gbuffers_pipelines_.DeleteBinding(in_id); },
            [&,this](){ gbuffers_pipelines_.DeleteBinding(in_id); },
            [&,this](){ ppcss_pipelines_.DeleteBinding(in_id); }
            );
    
    pipeline_track_.binding_pipetype.erase(in_id);
}

void WVkRender::RefreshPipelines() {
    ppcss_pipelines_.ComputeBindingOrder();
}

void WVkRender::ClearPipelines() {
    
    // Do not clear global pipelines and global descriptros.
    
    gbuffers_pipelines_.ClearPipelinesDb();
    ppcss_pipelines_.ClearPipelinesDb();
    ppcss_pipelines_.ComputeBindingOrder();
}

// Resources
// ---------

void WVkRender::UnloadAllResources() {
    asset_render_data_.Clear();
}

void WVkRender::UpdateUboCamera(
    wct::render::CameraUBO const & camera_ubo
    ) {
    global_descriptors_.UpdateCameraUBO(
        frame_index_,
        camera_ubo
        );
}

void WVkRender::UpdateParameterDynamic(
    const wcr::wid::WEntityComponentId & component_id,
    const wct::render::RPipeParamUbo & ubo_pipe_param
    ) {

    WVkDescSetUBOWrite ubowrt = wvk::render::pipe_bindings::GetUboWrite(ubo_pipe_param);

    WVkDescSetUBOBinding<FramesInFlight()> ubo_binding =
        wvk::render::pipe_bindings::GetUboBinding(
            component_id,
            ubo_pipe_param.binding,
            pipeline_track_.binding_pipetype[component_id],
            gbuffers_pipelines_,
            ppcss_pipelines_
            );

    wvk::render::pipe_bindings::UpdateParamDynamic(
        ubo_binding,
        ubowrt,
        asset_render_data_,
        device_.Device(),
        frame_index_
        );
}

void WVkRender::UpdateParameterStatic(
    const wcr::wid::WEntityComponentId & component_id,
    const wct::render::RPipeParamUbo & ubo_pipe_param
    ) {

    WVkDescSetUBOWrite ubowrt = wvk::render::pipe_bindings::GetUboWrite(ubo_pipe_param);

    WVkDescSetUBOBinding<FramesInFlight()> ubo_binding =
        wvk::render::pipe_bindings::GetUboBinding(
            component_id,
            ubo_pipe_param.binding,
            pipeline_track_.binding_pipetype[component_id],
            gbuffers_pipelines_,
            ppcss_pipelines_
            );
    
    wvk::render::pipe_bindings::UpdateParamStatic(
        ubo_binding,
        ubowrt,
        asset_render_data_,
        device_.Device()
        );
}

void WVkRender::Rescale(const std::uint32_t & in_width, const std::uint32_t & in_height) {

    render_size_.width = in_width;
    render_size_.height = in_height;

    RecreateSwapChain();
}

void WVkRender::RecreateSwapChain() {

    std::array<std::uint32_t,2> dimensions = {
        render_size_.width,
        render_size_.height
    };

    WaitIdle();

    // Recreate swap chain and other render targets

    swap_chain_ = {};

    swap_chain_ = WVkSwapchainRAII(
        device_.Device(),
        device_.PhysicalDevice(),
        surface_.Value(),
        dimensions[0],
        dimensions[1]
        );

    // Recreate Attachments

    gbuffers_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        WVK_GBUFFER_RENDER_COLOR_FORMAT,
        WVK_GBUFFER_RENDER_EMISSION_FORMAT,
        WVK_GBUFFER_RENDER_NORMAL_FORMAT,
        WVK_GBUFFER_RENDER_ORM_FORMAT,
        WVK_GBUFFER_RENDER_DEPTH_FORMAT,
        WVK_GBUFFER_RENDER_EXTRA01_FORMAT
    };

    lighting_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        WVK_LIGHTING_RENDER_COLOR_FORMAT        
    };

    postprocess_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        WVK_POSTPROCESS_RENDER_COLOR_FORMAT        
    };

    tonemapping_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        swap_chain_.Format()
    };

    // update postprocess global descriptors

    wvk::render::UpdatePPcessGlobalDescriptorSet(
        ppcess_global_descriptors_,
        gbuffers_attachments_,
        lighting_attachments_,
        render_plane_.Sampler()
        );
}

// Lights
// ------

void WVkRender::InitializeLights(
    std::span<wcr::wid::WEntityComponentId> in_pl_ids,
    std::span<wct::render::PointLight> in_point_lights,
    std::span<wcr::wid::WEntityComponentId> in_dl_ids,
    std::span<wct::render::DirectionalLight> in_directional_lights,
    const wct::render::AmbientLight & in_ambient_light
    ) {
    lighting_UBO_.Clear();

    lighting_UBO_.PointLightDenseController()
        .Update(in_pl_ids, in_point_lights);

    lighting_UBO_.DirectionalLightDenseController()
        .Update(in_dl_ids, in_directional_lights);

    lighting_UBO_.UpdateAmbientLight(in_ambient_light);

    global_descriptors_.StaticUpdateLightingUBO(
        lighting_UBO_.LightingUbo()
        );
}

void WVkRender::ClearLights() {
    lighting_UBO_.Clear();

    global_descriptors_.StaticUpdateLightingUBO(
        lighting_UBO_.LightingUbo()
        );
}


namespace {
    /**
     * Helper function to avoid too much duplicated code
     * TODO move to a better object.
     */
    template<typename LightType,
             std::uint8_t FramesInFlight,
             typename DenseController>
    inline void UpdateLightUBO(
        WVkGlobalDescriptorsRAII<FramesInFlight> & global_descriptor,
        std::uint8_t frame_index,
        DenseController & dense_controller,
        std::span<wcr::wid::WEntityComponentId> in_ids,
        std::span<LightType> in_lights
        ) {
    
        dense_controller.Update(
            in_ids, in_lights
            );

        std::uint32_t light_count = dense_controller.Count();
        std::uint32_t first=0;
        std::uint32_t last = light_count - 1;

        // if more than a half of the lights are being updated
        // update all lights.
        if (in_ids.size() < dense_controller.Count() / 2 ) {
            auto first_last = dense_controller
                .FirstLastDensePosition(in_ids);

            first = std::get<0>(first_last);
            last = std::get<1>(first_last);
        }

        std::uint8_t * ptr = reinterpret_cast<std::uint8_t*>(
            const_cast<LightType*>(dense_controller.DenseData()))
            + (sizeof(LightType) * first);

        global_descriptor.UpdateLightingUBO(
            frame_index,
            ptr,
            sizeof(wct::render::PointLight) * (last + 1 - first)
            );
    }
}

void WVkRender::UpdatePointLights(
    std::span<wcr::wid::WEntityComponentId> in_ids,
    std::span<wct::render::PointLight> in_point_lights
    ) {
    if (in_ids.empty()) return;

    auto dense_controller = lighting_UBO_.PointLightDenseController();
    
    UpdateLightUBO<wct::render::PointLight>(
        global_descriptors_,
        frame_index_,
        dense_controller,
        in_ids, in_point_lights);
}

void WVkRender::UpdateDirectionalLights(
    std::span<wcr::wid::WEntityComponentId> in_ids,
    std::span<wct::render::DirectionalLight> in_directional_lights
    ) {
    if (in_ids.empty()) return;

    auto dense_controller = lighting_UBO_.DirectionalLightDenseController();

    UpdateLightUBO<wct::render::DirectionalLight>(
        global_descriptors_,
        frame_index_,
        dense_controller,
        in_ids, in_directional_lights);
}

void WVkRender::UpdateAmbientLight(
    const wct::render::AmbientLight & in_ambient_light
    ) {
    lighting_UBO_.UpdateAmbientLight(in_ambient_light);

    const wct::render::LightingUBO & ubo = lighting_UBO_.LightingUbo();

    std::uint8_t * ptr = reinterpret_cast<std::uint8_t*>(
        const_cast<wct::render::LightingUBO*>(&ubo)
        ) + offsetof(wct::render::LightingUBO, ambient_light);

    global_descriptors_.UpdateLightingUBO(
        frame_index_,
        ptr,
        sizeof(wct::render::LightingUBO::ambient_light)
        );
}

