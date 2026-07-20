#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include "WCore/WId.hpp"
#endif

#include "WVulkan/WVkRender.hpp"

#include <GLFW/glfw3.h>

#include "WAssets/WRenderPipelineAsset.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/WVk/WVkRender.hpp"
#include "WWindow/WWindow.hpp"
#include "WCore/TVisitor.hpp"
#include "PipelineBindings.hpp"

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

    swapchain_ = WVkSwapchainRAII(
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
        WENG_VK_GBUFFER_RENDER_COLOR_FORMAT,
        WENG_VK_GBUFFER_RENDER_EMISSION_FORMAT,
        WENG_VK_GBUFFER_RENDER_NORMAL_FORMAT,
        WENG_VK_GBUFFER_RENDER_WSPOSITION_FORMAT,
        WENG_VK_GBUFFER_RENDER_MRAO_FORMAT,
        WENG_VK_GBUFFER_RENDER_DEPTH_FORMAT,
        WENG_VK_GBUFFER_RENDER_EXTRA01_FORMAT,
    };

    // Offscreen Attachments

    offscreen_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        WENG_VK_OFFSCREEN_RENDER_COLOR_FORMAT        
    };

    // Postprocess Attachments

    postprocess_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        WENG_VK_POSTPROCESS_RENDER_COLOR_FORMAT        
    };

    // tonemapping Attachments

    tonemapping_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        swapchain_.Format()
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

    WFLOG("[DEBUG] Initialize Offscreen Pipeline.");

    offscreen_pipeline_ = {
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
        swapchain_.Format()
    };
    
    WFLOG("[DEBUG] Initialize swap chain pipeline");

    swap_chain_pipeline_ = {
        device_.Device(),
        swapchain_.Format()
    };

    render_command_buffers_ =
        command_pool_.
        CreateCommandBuffers();

    render_sync_ = {device_.Device(),
                   swapchain_.Images().size()};
    
    asset_render_data_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        device_.GraphicsQueue(),
        command_pool_.Value()
    };

    wvk::render::UpdatePPcessGlobalDescriptorSet(
        ppcess_global_descriptors_,
        gbuffers_attachments_,
        offscreen_attachments_,
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
        swapchain_.Swapchain(),
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

    RecordGBuffersRenderCommandBuffer(
        render_command_buffers_[frame_index_],
        frame_index_);

    RecordOffscreenRenderCommandBuffer(
        render_command_buffers_[frame_index_],
        frame_index_);

    RecordPostprocessRenderCommandBuffer(
        render_command_buffers_[frame_index_],
        frame_index_,
        image_index
        );

    RecordTonemappingRenderCommandBuffer(
        render_command_buffers_[frame_index_],
        frame_index_,
        image_index
        );

    RecordSwapChainRenderCommandBuffer(
        render_command_buffers_[frame_index_],
        frame_index_,
        image_index
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
    present_info.pSwapchains = &swapchain_.Swapchain();
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

    semaphore_index_ = (semaphore_index_ + 1) % swapchain_.Images().size();
    frame_index_ = (frame_index_ + 1) % WENG_MAX_FRAMES_IN_FLIGHT;
}

// Pipelines
// ---------

void WVkRender::CreateRenderPipeline(
    WRenderPipelineAsset * render_pipeline
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
    const WRenderPipelineAsset & pipeline,
    const WRenderPipelineParametersAsset & parameters
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

    swapchain_ = {};

    swapchain_ = WVkSwapchainRAII(
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
        WENG_VK_GBUFFER_RENDER_COLOR_FORMAT,
        WENG_VK_GBUFFER_RENDER_EMISSION_FORMAT,
        WENG_VK_GBUFFER_RENDER_NORMAL_FORMAT,
        WENG_VK_GBUFFER_RENDER_WSPOSITION_FORMAT,
        WENG_VK_GBUFFER_RENDER_MRAO_FORMAT,
        WENG_VK_GBUFFER_RENDER_DEPTH_FORMAT,
        WENG_VK_GBUFFER_RENDER_EXTRA01_FORMAT
    };

    offscreen_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        WENG_VK_OFFSCREEN_RENDER_COLOR_FORMAT        
    };

    postprocess_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        WENG_VK_POSTPROCESS_RENDER_COLOR_FORMAT        
    };

    tonemapping_attachments_ = {
        device_.Device(),
        device_.PhysicalDevice(),
        {dimensions[0], dimensions[1]},
        swapchain_.Format()
    };

    // update postprocess global descriptors

    wvk::render::UpdatePPcessGlobalDescriptorSet(
        ppcess_global_descriptors_,
        gbuffers_attachments_,
        offscreen_attachments_,
        render_plane_.Sampler()
        );
}

void WVkRender::RecordGBuffersRenderCommandBuffer(
    const VkCommandBuffer & in_command_buffer,
    const std::uint32_t & in_frame_index
    )
{

    wvk::render::RndCmd_TransitionGBufferWriteLayout(
        in_command_buffer,
        gbuffers_attachments_.Albedo(in_frame_index).Image(),
        gbuffers_attachments_.Emission(in_frame_index).Image(),
        gbuffers_attachments_.Normal(in_frame_index).Image(),
        gbuffers_attachments_.WsPosition(in_frame_index).Image(),
        gbuffers_attachments_.MrAO(in_frame_index).Image(),
        gbuffers_attachments_.Depth(in_frame_index).Image(),
        gbuffers_attachments_.Extra01(in_frame_index).Image()
        );

    wvk::render::RndCmd_BeginGBuffersRendering(
        in_command_buffer,
        gbuffers_attachments_.Albedo(in_frame_index).View(),
        gbuffers_attachments_.Emission(in_frame_index).View(),
        gbuffers_attachments_.Normal(in_frame_index).View(),
        gbuffers_attachments_.WsPosition(in_frame_index).View(),
        gbuffers_attachments_.MrAO(in_frame_index).View(),
        gbuffers_attachments_.Depth(in_frame_index).View(),
        gbuffers_attachments_.Extra01(in_frame_index).View(),
        gbuffers_attachments_.Extent()
        );

    for(auto pipeline_id : gbuffers_pipelines_.IterPipelines()) {
        
        gbuffers_pipelines_.ResetDescriptorPool(pipeline_id, frame_index_);

        const WVkRenderPipeline & render_pipeline =
            gbuffers_pipelines_.Pipeline(pipeline_id);

        vkCmdBindPipeline(render_command_buffers_[in_frame_index],
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          render_pipeline.pipeline);

        wvk::render::RndCmd_SetViewportAndScissor(
            in_command_buffer,
            gbuffers_attachments_.Extent()
            );

        for (auto & bid : gbuffers_pipelines_.IterBindings(pipeline_id)) {

            auto& binding = gbuffers_pipelines_.GetBinding(bid);

            // Create descriptor
            VkDescriptorSet descriptorset =
                wvk::render::CreateDescriptorSet(
                    device_.Device(),
                    gbuffers_pipelines_.DescriptorPool(pipeline_id, in_frame_index),
                    gbuffers_pipelines_.DescriptorSetLayout(pipeline_id).descset_layout,
                    frame_index_,
                    binding.ubos,
                    binding.textures
                    );

            auto& mesh_info =
                asset_render_data_.StaticMeshInfo(
                    binding.mesh_asset_id
                    );

            VkBuffer vertex_buffers[] = {mesh_info.vertex_buffer};
            VkDeviceSize offsets[] = {0};

            vkCmdBindVertexBuffers(
                in_command_buffer,
                0,
                1,
                vertex_buffers,
                offsets
                );

            vkCmdBindIndexBuffer(
                in_command_buffer,
                mesh_info.index_buffer,
                0,
                VK_INDEX_TYPE_UINT32
                );

            std::array<VkDescriptorSet, 2> descsets =
                {
                    global_descriptors_.DescriptorSet(frame_index_),
                    descriptorset
                };

            vkCmdBindDescriptorSets(in_command_buffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    render_pipeline.pipeline_layout,
                                    0,
                                    static_cast<std::uint32_t>(descsets.size()),
                                    descsets.data(),
                                    0,
                                    nullptr);

            vkCmdDrawIndexed(in_command_buffer,
                             mesh_info.index_count,
                             1,
                             0,
                             0,
                             0);
        }
    }
    
    vkCmdEndRendering(in_command_buffer);

    wvk::render::RndCmd_TransitionGBufferReadLayout(
        in_command_buffer,
        gbuffers_attachments_.Albedo(in_frame_index).Image(),
        gbuffers_attachments_.Emission(in_frame_index).Image(),
        gbuffers_attachments_.Normal(in_frame_index).Image(),
        gbuffers_attachments_.WsPosition(in_frame_index).Image(),
        gbuffers_attachments_.MrAO(in_frame_index).Image(),
        gbuffers_attachments_.Depth(in_frame_index).Image(),
        gbuffers_attachments_.Extra01(in_frame_index).Image()
        );
}

void WVkRender::RecordOffscreenRenderCommandBuffer(
    const VkCommandBuffer & in_command_buffer,
    const std::uint32_t & in_frame_index
    ) {
    
    wvk::render::RndCmd_TransitionOffscreenWriteLayout(
        in_command_buffer,
        offscreen_attachments_.Color(in_frame_index).Image()
        );

    wvk::render::RndCmd_BeginOffscreenRendering(
        in_command_buffer,
        offscreen_attachments_.Color(in_frame_index).View(),
        offscreen_attachments_.Extent()
        );

    offscreen_pipeline_.ResetDescriptorPool(in_frame_index);

    VkPipeline pipeline = offscreen_pipeline_.Pipeline();

    // Bind Pipeline
    vkCmdBindPipeline(
        in_command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline
        );

    wvk::render::RndCmd_SetViewportAndScissor(
        in_command_buffer,
        offscreen_attachments_.Extent()
        );

    // DescriptorSet
    // TODO do not recreate each frame, create descriptorsSets only once.
    VkDescriptorSet descriptorset = wvk::render::CreateOffscreenRenderDescriptor(
        device_.Device(),
        offscreen_pipeline_.DescriptorPool(in_frame_index),
        offscreen_pipeline_.DescriptorSetLayout(),
        render_plane_.Sampler(),
        gbuffers_attachments_.Albedo(in_frame_index).View(),
        gbuffers_attachments_.Emission(in_frame_index).View(),
        gbuffers_attachments_.Normal(in_frame_index).View(),
        gbuffers_attachments_.WsPosition(in_frame_index).View(),
        gbuffers_attachments_.MrAO(in_frame_index).View(),
        gbuffers_attachments_.Depth(in_frame_index).View(),
        gbuffers_attachments_.Extra01(in_frame_index).View()
        );

    // Draw Commands
    const WVkMesh & rplane = render_plane_.RenderPlane();
    
    VkBuffer vertex_buffers[] = {rplane.vertex_buffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(
        in_command_buffer,
        0,
        1,
        vertex_buffers,
        offsets
        );

    vkCmdBindIndexBuffer(
        in_command_buffer,
        rplane.index_buffer,
        0,
        VK_INDEX_TYPE_UINT32
        );

    std::array<VkDescriptorSet,2> descsets = {
        global_descriptors_.DescriptorSet(in_frame_index),
        descriptorset
    };

    vkCmdBindDescriptorSets(in_command_buffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            offscreen_pipeline_.PipelineLayout(),
                            0,
                            static_cast<std::uint32_t>(descsets.size()),
                            descsets.data(),
                            0,
                            nullptr);

    vkCmdDrawIndexed(in_command_buffer,
                     rplane.index_count,
                     1,
                     0,
                     0,
                     0);

    vkCmdEndRendering(in_command_buffer);
    
    wvk::render::RndCmd_TransitionOffscreenReadLayout(
        in_command_buffer,
        offscreen_attachments_.Color(in_frame_index).Image()
        );
    
}

void WVkRender::RecordPostprocessRenderCommandBuffer(
    const VkCommandBuffer & in_command_buffer,
    const std::uint32_t & in_frame_index,
    const std::uint32_t & in_image_index
    )
{

    VkImageView input_view = offscreen_attachments_.Color(in_frame_index).View();
    VkImage input_img = offscreen_attachments_.Color(in_frame_index).Image();
    
    VkImageView dst_view = postprocess_attachments_.Color(in_frame_index).View();
    VkImage dst_img = postprocess_attachments_.Color(in_frame_index).Image();
    
    std::array<VkImageView, 2> pp_views = {input_view, dst_view};
    std::array<VkImage, 2> pp_images = {input_img, dst_img};

    // TODO Descriptors are being recreated each frame.
    //  is it required? can I preserve the descriptors between frames?
    ppcss_pipelines_.ResetDescriptorPools(in_frame_index);

    // Render each postprocess shader
    std::uint32_t idx=0;
    for(auto pbindingid : ppcss_pipelines_.BindingOrderIterator()) {

        // DELETE_WVkPipelineBindingInfo ppcess_binding =
        //     ppcss_pipelines_.Binding(pbindingid);

        auto ppcess_binding = ppcss_pipelines_.GetBinding(pbindingid);

        WVkRenderPipeline ppcess_pipeline =
            ppcss_pipelines_.Pipeline(ppcess_binding.pipeline_id);
        WVkDescriptorSetLayoutInfo ppcess_dsetlay =
            ppcss_pipelines_.DescriptorSetLayout(ppcess_binding.pipeline_id);
        VkDescriptorPool ppcess_dpool =
            ppcss_pipelines_.DescriptorPool(ppcess_binding.pipeline_id, in_frame_index);

        // render into layout
        wvk::render::RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            dst_img,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        wvk::render::RndCmd_BeginPostprocessRendering(
            in_command_buffer,
            dst_view,
            postprocess_attachments_.Extent()
            );

        vkCmdBindPipeline(
            in_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            ppcess_pipeline.pipeline
            );

        wvk::render::RndCmd_SetViewportAndScissor(
            in_command_buffer,
            postprocess_attachments_.Extent()
            );

        ppcess_global_descriptors_.UpdateDescriptorBinding(
            ppcess_global_descriptors_.PREV_BINDING,
            frame_index_,
            {
                .sampler=render_plane_.Sampler(),
                .imageView=input_view,
                .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            }
            );

        // TODO is it required to be recreated each frame?
        VkDescriptorSet pp_descriptor = wvk::render::CreateDescriptorSet(
            device_.Device(),
            ppcess_dpool,
            ppcess_dsetlay.descset_layout,
            frame_index_,
            ppcess_binding.ubos,
            ppcess_binding.textures
            );

        const WVkMesh & render_plane = render_plane_.RenderPlane();

        wvk::render::RndCmd_PostprocessDrawCommands(
            device_.Device(), in_command_buffer,
            render_plane.vertex_buffer, render_plane.index_buffer,
            render_plane.index_count,
            ppcess_pipeline.pipeline_layout,
            ppcess_pipeline.pipeline,
            std::array<VkDescriptorSet,3>{
                global_descriptors_.DescriptorSet(frame_index_),
                pp_descriptor,
                ppcess_global_descriptors_.DescriptorSet(frame_index_)
            }
            );

        vkCmdEndRendering(in_command_buffer);

        idx++;

        input_view = pp_views[idx % 2];
        input_img = pp_images[idx % 2];
        dst_view = pp_views[(idx + 1) % 2];
        dst_img = pp_images[(idx + 1) % 2];

        // render from layout
        wvk::render::RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            input_img,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );
    }

    // Image view that will be used by the swap chain pipeline pass
    swap_chain_input_imgview_ref = input_view;
}

void WVkRender::RecordTonemappingRenderCommandBuffer(
    const VkCommandBuffer & in_command_buffer,
    const std::uint32_t & in_frame_index,
    const std::uint32_t & in_image_index
    ) {
    
    wvk::render::RndCmd_TransitionTonemappingWriteLayout(
        in_command_buffer,
        tonemapping_attachments_.Color(in_frame_index).Image()
        // tonemapping_rtargets_[in_frame_index].color.image
        );

    wvk::render::RndCmd_BeginTonemappingRendering(
        in_command_buffer,
        tonemapping_attachments_.Color(in_frame_index).View(),
        tonemapping_attachments_.Extent()
        );

    tonemapping_pipeline_.ResetDescriptorPool(in_frame_index);

    VkPipeline pipeline = tonemapping_pipeline_.Pipeline();

    vkCmdBindPipeline(
        in_command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline
        );

    wvk::render::RndCmd_SetViewportAndScissor(
        in_command_buffer,
        tonemapping_attachments_.Extent()
        );

    VkDescriptorSet descriptorset =
        wvk::render::CreateTonemappingDescriptor(
            device_.Device(),
            tonemapping_pipeline_.DescriptorPool(in_frame_index),
            tonemapping_pipeline_.DescriptorSetLayout(),
            render_plane_.Sampler(),
            swap_chain_input_imgview_ref
            );

    const WVkMesh & rplane = render_plane_. RenderPlane();
    VkDeviceSize offsets = 0;

    wvk::render::TonemappingBindings(
        in_command_buffer,
        rplane.vertex_buffer,
        rplane.index_buffer,
        offsets,
        descriptorset,
        tonemapping_pipeline_.PipelineLayout()
        );

    vkCmdDrawIndexed(in_command_buffer,
                     rplane.index_count,
                     1,
                     0,
                     0,
                     0);

    vkCmdEndRendering(in_command_buffer);

    wvk::render::RndCmd_TransitionTonemappingReadLayout(
        in_command_buffer,
        tonemapping_attachments_.Color(in_frame_index).Image()
        );

    swap_chain_input_imgview_ref =
        tonemapping_attachments_.Color(in_frame_index).View();
}

void WVkRender::RecordSwapChainRenderCommandBuffer(
    const VkCommandBuffer & in_command_buffer,
    const std::uint32_t & in_frame_index,
    const std::uint32_t & in_image_index
    ) {
    VkImage swapchain_image = swapchain_.Images()[in_image_index];
    VkImageView swapchain_imageview = swapchain_.Views()[in_image_index];

    // swap chain image layout to render into it
    wvk::render::RndCmd_TransitionRenderImageLayout(
        in_command_buffer,
        swapchain_image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        {},
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        );

    wvk::render::RndCmd_BeginSwapchainRendering(
        in_command_buffer,
        swapchain_imageview,
        swapchain_imageview,
        swapchain_.Extent()
        );

    swap_chain_pipeline_.ResetDescriptorPool(in_frame_index);

    VkPipeline pipeline = swap_chain_pipeline_.Pipeline();
    VkDescriptorSetLayout dslay = swap_chain_pipeline_.DescriptorSetLayout();

    vkCmdBindPipeline(
        in_command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline
        );

    wvk::render::RndCmd_SetViewportAndScissor(
        in_command_buffer,
        swapchain_.Extent()
        );

    VkDescriptorSet descriptor = wvk::render::CreateInputRenderDescriptor(
        device_.Device(),
        swap_chain_pipeline_.DescriptorPool(in_frame_index),
        dslay,
        swap_chain_input_imgview_ref,
        render_plane_.Sampler()
        );

    auto & render_plane = render_plane_.RenderPlane();
    VkDeviceSize offsets=0;

    vkCmdBindVertexBuffers(in_command_buffer,
                           0,
                           1,
                           &render_plane.vertex_buffer,
                           &offsets);

    vkCmdBindIndexBuffer(in_command_buffer,
                         render_plane.index_buffer,
                         0,
                         VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(in_command_buffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            swap_chain_pipeline_.PipelineLayout(),
                            0,
                            1,
                            &descriptor,
                            0,
                            nullptr);

    vkCmdDrawIndexed(in_command_buffer,
                     render_plane.index_count,
                     1,
                     0,0,0);

    vkCmdEndRendering(in_command_buffer);

    // Prepare swapchain images for present
    wvk::render::RndCmd_TransitionRenderImageLayout(
        in_command_buffer,
        swapchain_image,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        {},
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
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

