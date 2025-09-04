#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

#include "WVulkan/WVkRender.hpp"
#include "WRenderUtils.hpp"
#include "WCore/WCore.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVkRenderCommandPool.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVkRenderStructs.hpp"
#include "WVulkan/WVkRenderPipeline.hpp"
#include "WVulkan/WVkRenderResources.hpp"
#include "WStructs/WComponentStructs.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVkRenderUtils.hpp"
#include "WLog.hpp"

#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include <array>

#ifdef NDEBUG
    #define _ENABLE_VALIDATON_LAYERS true
#else
    #define _ENABLE_VALIDATON_LAYERS true
#endif

// WVkRender
// -------

WVkRender::WVkRender() :
    instance_info_(),
    window_(),
    surface_info_(),
    device_info_(),
    debug_info_(),
    render_resources_(),
    swap_chain_info_(),
    offscreen_render_(),
    postprocess_render_(),
    render_command_pool_(),
    render_command_buffer_(),
    pipelines_manager_(),
    image_available_semaphore_(),
    render_finished_semaphore_(),
    flight_fence_(),
    frame_index_(0) // ,
    // frame_buffer_resized_(false)
{
}

WVkRender::WVkRender(GLFWwindow * in_window) : WVkRender() {
    Window(in_window);
}

WVkRender::~WVkRender()
{
    if (device_info_.vk_device) {
        Destroy();
    }
}

void WVkRender::WaitIdle() const
{
    vkDeviceWaitIdle(device_info_.vk_device);
}

void WVkRender::Window(GLFWwindow * in_window) {
    window_.window = in_window;
    std::int32_t width, height;
    
    glfwGetFramebufferSize(window_.window, &width, &height);

    window_.width = static_cast<std::uint32_t>(width);
    window_.height = static_cast<std::uint32_t>(height);
}

void WVkRender::Initialize()
{
    debug_info_.enable_validation_layers = _ENABLE_VALIDATON_LAYERS;

    // Create Vulkan Instance
    WVulkan::Create(
        instance_info_,
        debug_info_
        );

    // Create Vulkan Window Surface
    WVulkan::Create(
        surface_info_,
        instance_info_, 
        window_.window
        );

    // Create Vulkan Device
    WVulkan::Create(
        device_info_, 
        instance_info_, 
        surface_info_,
        debug_info_
        );

    // Create Vulkan Swap Chain
    WVulkan::Create(
        swap_chain_info_,
        device_info_,
        surface_info_,
        window_.width,
        window_.height,
        offscreen_render_,
        debug_info_
        );
    
    // Offscreen Render Pass
    // TODO: check frames in flight

    offscreen_render_.extent = {window_.width, window_.height};
    // WVulkan::CreateOffscreenRenderPass(
    //     offscreen_render_,
    //     swap_chain_info_.swap_chain_image_format,
    //     device_info_
    //     );

    offscreen_render_.color.extent = {window_.width, window_.height};
    WVulkan::CreateColorResource(
        offscreen_render_.color.image,
        offscreen_render_.color.memory,
        offscreen_render_.color.view,
        swap_chain_info_.swap_chain_image_format,
        device_info_,
        offscreen_render_.color.extent
        );

    offscreen_render_.depth.extent = {window_.width, window_.height};
    WVulkan::CreateDepthResource(
        offscreen_render_.depth.image,
        offscreen_render_.depth.memory,
        offscreen_render_.depth.view,
        device_info_,
        offscreen_render_.depth.extent
        );

    // WVulkan::CreateOffscreenFramebuffer(
    //     offscreen_render_,
    //     device_info_
    //     );

    // Postprocess Render Pass
    // TODO: frames in flight

    postprocess_render_.extent = {window_.width, window_.height};
    // WVulkan::CreatePostprocessRenderPass(
    //     postprocess_render_,
    //     swap_chain_info_.swap_chain_image_format,
    //     device_info_
    //     );

    postprocess_render_.color.extent = {window_.width, window_.height};
    WVulkan::CreateColorResource(
        postprocess_render_.color.image,
        postprocess_render_.color.memory,
        postprocess_render_.color.view,
        swap_chain_info_.swap_chain_image_format,
        device_info_,
        postprocess_render_.color.extent
        );

    WVulkan::CreatePostprocessFramebuffer(
        postprocess_render_,
        device_info_
        );

    // TODO no swap chain
    // WVulkan::CreateOffcreenRenderFrameBuffers(
    //     swap_chain_info_,
    //     offscreen_renderpass_info_,
    //     device_info_
    //     );

    // --

    pipelines_manager_ = WVkRenderPipelinesManager(
        device_info_,
        offscreen_render_,
        window_.width,
        window_.height
        );

    render_command_pool_ = WVkRenderCommandPool( 
        WVkCommandPoolInfo(),
        device_info_,
        surface_info_
        );

    // Offscreen Render Pass

    render_command_buffer_ =
        render_command_pool_.
        CreateCommandBuffer();

    WVulkan::Create(
        image_available_semaphore_,
        device_info_
        );

    WVulkan::Create(
        render_finished_semaphore_,
        device_info_
        );

    WVulkan::Create(
        flight_fence_,
        device_info_
        );

    render_resources_ = WVkRenderResources(
        device_info_,
        render_command_pool_.CommandPoolInfo()
        );

}

void WVkRender::Draw()
{

    vkWaitForFences(
        device_info_.vk_device,
        1,
        &flight_fence_.fences[frame_index_],
        VK_TRUE,
        UINT64_MAX
        );
    
    uint32_t image_index;

    VkResult result = vkAcquireNextImageKHR(
        device_info_.vk_device,
        swap_chain_info_.swap_chain,
        UINT64_MAX,
        image_available_semaphore_.semaphores[frame_index_],
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
        device_info_.vk_device,
        1,
        &flight_fence_.fences[frame_index_]
        );

    VkSemaphore signal_semaphores[] = {
        render_finished_semaphore_.semaphores[frame_index_]
    };

    // Begin command buffer

    WVkRenderUtils::BeginRenderCommandBuffer(
        render_command_buffer_.command_buffers[frame_index_]
        );

    RecordGraphicsRenderCommandBuffer(
        render_command_buffer_.command_buffers[frame_index_],
        frame_index_);

    RecordPostprocessRenderCommandBuffer(
        render_command_buffer_.command_buffers[frame_index_],
        frame_index_
        );

    // End Command buffer

    WVkRenderUtils::EndRenderCommandBuffer(
        render_command_buffer_.command_buffers[frame_index_]
        );

    VkSubmitInfo submit_info;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = nullptr;

    VkSemaphore wait_semaphores[] =
        { image_available_semaphore_.semaphores[frame_index_] };
    VkPipelineStageFlags wait_stages[] =
        { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &render_command_buffer_.command_buffers[frame_index_];

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if (vkQueueSubmit(
            device_info_.vk_graphics_queue,
            1,
            &submit_info,
            flight_fence_.fences[frame_index_]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command buffer");
    }
    // }

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    VkSwapchainKHR swap_chains[] = {swap_chain_info_.swap_chain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;

    result = vkQueuePresentKHR(device_info_.vk_present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    frame_index_ = (frame_index_ + 1) % WENG_MAX_FRAMES_IN_FLIGHT;
}

void WVkRender::CreateRenderPipeline(
    WRenderPipelineAsset * render_pipeline
    ) {
    pipelines_manager_.CreateRenderPipeline(
        render_pipeline->WID(),
        render_pipeline->RenderPipeline()
        );
}

void WVkRender::DeleteRenderPipeline(const WAssetId & in_id) {
    pipelines_manager_.DeleteRenderPipeline(
        in_id
        );
}


void WVkRender::CreatePipelineBinding(
    const WEntityComponentId & component_id,
    const WAssetId & pipeline_id,
    const WAssetIndexId & in_mesh_id,
    // const WAssetId & in_mesh_id,
    const WRenderPipelineParametersStruct & in_parameters
    )
{
    std::vector<WVkTextureInfo> tinfo{
        in_parameters.texture_assets_count
    };

    std::vector<uint16_t> tbinding{in_parameters.texture_assets_count};

    for (uint8_t i=0; i < in_parameters.texture_assets_count; i++) {
        tinfo[i] = render_resources_.TextureInfo(
            in_parameters.texture_assets[i].value
            );
        tbinding[i] = in_parameters.texture_assets[i].binding;
    }

    pipelines_manager_.CreateBinding(
        component_id,
        pipeline_id,
        in_mesh_id,
        tinfo,
        tbinding
        );
}

void WVkRender::DeletePipelineBinding(const WEntityComponentId & in_id) {
    pipelines_manager_.DeleteBinding(in_id);
}

void WVkRender::ClearPipelines() {
    pipelines_manager_.Clear();
}

void WVkRender::UnloadAllResources() {
    render_resources_.Clear();
}

void WVkRender::UpdateUboCamera(
    const WCameraStruct & camera_struct,
    const WTransformStruct & transform_struct
    ) {

    WUBOCameraStruct camera_ubo = WRenderUtils::ToUBOCameraStruct(
        camera_struct,
        transform_struct,
        (float) window_.width / (float) window_.height
        );

    pipelines_manager_.UpdateGlobalGraphicsDescriptorSet(
        camera_ubo,
        frame_index_
        );
}

void WVkRender::UpdateUboModelDynamic(
    const WEntityComponentId & in_component_id,
    const WTransformStruct & in_transform_struct
    ) {
    WUBOModelStruct ubo_model = WRenderUtils::ToUBOModelStruct(
        in_transform_struct
        );
    pipelines_manager_.UpdateModelDescriptorSet(
        ubo_model, in_component_id, frame_index_
        );
                             
}

void WVkRender::UpdateUboModelStatic(
    const WEntityComponentId & in_component_id,
    const WTransformStruct & in_transform_struct
    ) {
    WUBOModelStruct ubo_model = WRenderUtils::ToUBOModelStruct(
        in_transform_struct
        );
    pipelines_manager_.UpdateModelDescriptorSet(
        ubo_model, in_component_id
        );
}

void WVkRender::Destroy() {

    WFLOG("Destroy WVkRender...");

    WFLOG("Destroy Render Pipelines Manager");
    pipelines_manager_.Destroy();

    WFLOG("Destroy Fences and Semaphores");

    WVulkan::Destroy(image_available_semaphore_, device_info_);

    WVulkan::Destroy(render_finished_semaphore_, device_info_);

    WVulkan::Destroy(flight_fence_, device_info_);

    WFLOG("Destroy Render Pass Info");

    // Destroy Vulkan Render Pass
    WVulkan::Destroy(offscreen_render_, device_info_);

    WFLOG("Destroy Swap Chain Info");

    // Destroy Swap Chain and Image Views
    WVulkan::Destroy(swap_chain_info_, device_info_);

    WFLOG("Destroy Render Command Pool");

    render_command_pool_.Clear();
    render_command_pool_ = {};
    
    render_command_buffer_ = {};

    WFLOG("Destroy Render Resources");

    render_resources_.Clear();

    WFLOG("Destroy Vulkan Device");

    // Destroy Vulkan Device
    WVulkan::Destroy(device_info_);

    // Destroy Vulkan Surface
    WVulkan::Destroy(surface_info_, instance_info_);

    // Destroy Vulkan Instance
    WVulkan::Destroy(instance_info_);
}

void WVkRender::Rescale(const std::uint32_t & in_width, const std::uint32_t & in_height) {
    window_.width = in_width;
    window_.height = in_height;

    RecreateSwapChain();
    
    pipelines_manager_.Width(window_.width);
    pipelines_manager_.Height(window_.height);
}

void WVkRender::RecreateSwapChain() {
    WFLOG("RECREATE SWAP CHAIN!");
    
    WaitIdle();

    WVulkan::Destroy(
        swap_chain_info_,
        device_info_
        );

    WVulkan::Destroy(
        offscreen_render_,
        device_info_
        );

    WVulkan::Create(
        swap_chain_info_,
        device_info_,
        surface_info_,
        window_.width,
        window_.height,
        offscreen_render_,
        debug_info_
        );

    // TODO Recreate Render resources with the new size

    // WVulkan::CreateSwapChainImageViews(
    //     swap_chain_info_,
    //     device_info_
    //     );

    // WVulkan::CreateOffscreenRenderPass(
    //     offscreen_render_,
    //     swap_chain_info_,
    //     device_info_
    //     );
}


void WVkRender::RecordGraphicsRenderCommandBuffer(
    const VkCommandBuffer & in_command_buffer,
    const std::uint32_t & in_frame_index
    )
{
    // Image Layouts
    WVkRenderUtils::RndCmd_TransitionRenderImageLayout(
        in_command_buffer,
        offscreen_render_.color.image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        {},
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        );

    WVkRenderUtils::RndCmd_TransitionRenderImageLayout(
        in_command_buffer,
        offscreen_render_.depth.image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        {},
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
        );

    WVkRenderUtils::RndCmd_BeginOffscreenRendering(
        in_command_buffer,
        offscreen_render_.color.view,
        offscreen_render_.depth.view,
        offscreen_render_.extent
        );

    for(auto pipeline_id : pipelines_manager_.IteratePipelines(EPipelineType::Graphics)) {
        
        pipelines_manager_.ResetDescriptorPool(pipeline_id, frame_index_);

        const WVkRenderPipelineInfo & render_pipeline =
            pipelines_manager_.RenderPipelineInfo(pipeline_id);

        vkCmdBindPipeline(
            render_command_buffer_.command_buffers[in_frame_index],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            render_pipeline.pipeline
            );

        WVkRenderUtils::RndCmd_SetViewportAndScissor(
            in_command_buffer,
            offscreen_render_.extent
            );

        for (auto & bid : pipelines_manager_.IterateBindings(pipeline_id))
        {

            auto& binding = pipelines_manager_.Binding(bid);

            // Create descriptor
            VkDescriptorSet descriptorset = WVkRenderUtils::CreateGraphicsDescriptor(
                device_info_.vk_device,
                pipelines_manager_.DescriptorPoolInfo(pipeline_id, in_frame_index).descriptor_pool,
                pipelines_manager_.DescriptorSetLayout(pipeline_id).descriptor_set_layout,
                binding.ubo[in_frame_index],
                binding.textures
                );

            auto& mesh_info =
                render_resources_.StaticMeshInfo(
                    binding.mesh_asset_id
                    );

            VkBuffer vertex_buffers[] = {mesh_info.vertex_buffer};
            VkDeviceSize offsets[] = {0};

            vkCmdBindVertexBuffers(
                render_command_buffer_.command_buffers[in_frame_index],
                0,
                1,
                vertex_buffers,
                offsets
                );

            vkCmdBindIndexBuffer(
                render_command_buffer_.command_buffers[in_frame_index],
                mesh_info.index_buffer,
                0,
                VK_INDEX_TYPE_UINT32
                );

            std::array<VkDescriptorSet, 2> descsets =
                {
                    pipelines_manager_.GlobalGraphicsDescriptorSet(frame_index_).descriptor_set,
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

    // Transition to postprocess
    WVkRenderUtils::RndCmd_TransitionRenderImageLayout(
        in_command_buffer,
        offscreen_render_.color.image,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,  // for postprocess
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
        );


}

void WVkRender::RecordPostprocessRenderCommandBuffer(
    const VkCommandBuffer & in_command_buffer,
    const std::uint32_t & in_frame_index
    )
{
    // Transition to postprocess

    WVkRenderUtils::RndCmd_TransitionRenderImageLayout(
        in_command_buffer,
        postprocess_render_.color.image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        {},
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        );


    for(auto pipeline_id : pipelines_manager_.IteratePipelines(EPipelineType::Postprocess)) {

        // TODO Descriptor and prev texture binding
        
    }

    // TODO at the end the default postprocess (writepixels to swap chain image)

    
}
