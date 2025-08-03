#include "WRender.hpp"
#include "WCore/WCore.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVkRenderCommandPool.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVkRenderCore.hpp"
#include "WVulkan/WVkRenderPipeline.hpp"
#include "WVulkan/WVkRenderResources.hpp"
#include "WLog.hpp"

#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>

#ifdef NDEBUG
    #define _ENABLE_VALIDATON_LAYERS false
#else
    #define _ENABLE_VALIDATON_LAYERS true
#endif

// WRender
// -------

void WRender::FrameBufferSizeCallback(GLFWwindow* in_window, int, int)
{
    auto app = reinterpret_cast<WRender*>(glfwGetWindowUserPointer(in_window));
    app->frame_buffer_resized = true;
}

WRender::WRender() :
    instance_info_(),
    window_info_(),
    surface_info_(),
    device_info_(),
    debug_info_(),
    swap_chain_info_(),
    render_pass_info_(),
    render_command_pool_(),
    render_command_buffer_(),
    pipelines_manager_(),
    image_available_semaphore_(),
    render_finished_semaphore_(),
    flight_fence_()
{
    Initialize();
}

WRender::~WRender()
{
    WFLOG("Destroy WRender");
    Destroy();
}

void WRender::WaitIdle() const
{
    vkDeviceWaitIdle(device_info_.vk_device);
}

void WRender::Initialize()
{
    window_info_.user_pointer = this;
    window_info_.framebuffer_size_callback = FrameBufferSizeCallback;
    debug_info_.enable_validation_layers = _ENABLE_VALIDATON_LAYERS;

    WVulkan::Create(
        window_info_
        );

    // Create Vulkan Instance
    WVulkan::Create(
        instance_info_,
        debug_info_
        );

    WVulkan::Create(
        surface_info_,
        instance_info_, 
        window_info_
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
        window_info_,
        render_pass_info_,
        debug_info_
        );

    // Create Vulkan Image Views
    WVulkan::CreateSCImageViews(
        swap_chain_info_,
        device_info_
        );

    // Create Vulkan Render Pass
    WVulkan::Create(
        render_pass_info_,
        swap_chain_info_,
        device_info_
        );

    pipelines_manager_ = WVkRenderPipelinesManager(
        device_info_,
        render_pass_info_,
        window_info_.width,
        window_info_.height
        );

    render_command_pool_ = WVkRenderCommandPool( 
        WVkCommandPoolInfo(),
        device_info_,
        surface_info_
        );

    WVulkan::CreateSCColorResources(
        swap_chain_info_,
        device_info_
        );

    WVulkan::CreateSCDepthResources(
        swap_chain_info_,
        device_info_
        );

    WVulkan::CreateSCFramebuffers(
        swap_chain_info_,
        render_pass_info_,
        device_info_
        );

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

void WRender::Draw()
{

    WFLOG("Drawing...");

    vkWaitForFences(
        device_info_.vk_device,
        1,
        &flight_fence_.fences[frame_index],
        VK_TRUE,
        UINT64_MAX
        );
    
    uint32_t image_index;

    VkResult result = vkAcquireNextImageKHR(
        device_info_.vk_device,
        swap_chain_info_.swap_chain,
        UINT64_MAX,
        image_available_semaphore_.semaphores[frame_index],
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
        &flight_fence_.fences[frame_index]
        );

    VkSemaphore signal_semaphores[] = {
        render_finished_semaphore_.semaphores[frame_index]
    };

    for(auto pit : pipelines_manager_.IteratePipelines(EPipelineType::Graphics)) {

        vkResetCommandBuffer(render_command_buffer_.command_buffers[frame_index], 0);

        RecordRenderCommandBuffer(
            pit,
            frame_index,
            image_index
            );

        VkSubmitInfo submit_info;
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = nullptr;

        VkSemaphore wait_semaphores[] =
            { image_available_semaphore_.semaphores[frame_index] };
        VkPipelineStageFlags wait_stages[] =
            { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;

        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &render_command_buffer_.command_buffers[frame_index];

        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        if (vkQueueSubmit(
                device_info_.vk_graphics_queue,
                1,
                &submit_info,
                flight_fence_.fences[frame_index]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit draw command buffer");
        }
    }

    WFLOG("Signal Semaphores: {:d}", (size_t)signal_semaphores[0]);

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
    WFLOG("QueuePresentKHR Result: {:d}", (size_t)result);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frame_buffer_resized) {
        frame_buffer_resized = false;
        RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    frame_index = (frame_index + 1) % WENG_MAX_FRAMES_IN_FLIGHT;
}

void WRender::CreateRenderPipeline(
    WRenderPipelineAsset * render_pipeline
    ) {
    pipelines_manager_.CreateRenderPipeline(
        render_pipeline->WID(),
        render_pipeline->RenderPipeline()
        );
}

void WRender::DeleteRenderPipeline(const WAssetId & in_id) {
    pipelines_manager_.DeleteRenderPipeline(
        in_id
        );
}

void WRender::RecreateSwapChain() {
    WFLOG("RECREATE SWAP CHAIN!");
    
    int width=0, height=0;
    glfwGetFramebufferSize(window_info_.window, &width, &height);

    while(width ==0 || height == 0) {
        glfwGetFramebufferSize(window_info_.window, &width, &height);
        glfwWaitEvents();
    }

    window_info_.width = width;
    window_info_.height = height;

    WaitIdle();

    WVulkan::Destroy(
        swap_chain_info_,
        device_info_
        );

    WVulkan::Destroy(
        render_pass_info_,
        device_info_
        );

    WVulkan::Create(
        swap_chain_info_,
        device_info_,
        surface_info_,
        window_info_,
        render_pass_info_,
        debug_info_
        );

    WVulkan::CreateSCImageViews(
        swap_chain_info_,
        device_info_
        );

    WVulkan::Create(
        render_pass_info_,
        swap_chain_info_,
        device_info_
        );

    pipelines_manager_.Width(width);
    pipelines_manager_.Height(height);
}

void WRender::CreatePipelineBinding(
    const WEntityComponentId & component_id,
    const WAssetId & pipeline_id,
    const WAssetId & in_mesh_id,
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

void WRender::DeletePipelineBinding(const WEntityComponentId & in_id) {
    pipelines_manager_.DeleteBinding(in_id);
}

void WRender::RecordRenderCommandBuffer(WId in_pipeline_id, uint32_t in_frame_index, uint32_t in_image_index)
{
    const WVkRenderPipelineInfo & render_pipeline =
        pipelines_manager_.RenderPipelineInfo(in_pipeline_id);

    // Update pipeline descriptor data

    VkCommandBufferBeginInfo begin_info{};
    
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(
            render_command_buffer_.command_buffers[in_frame_index],
            &begin_info
            ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass =  render_pass_info_.render_pass;
    render_pass_info.framebuffer = swap_chain_info_.swap_chain_framebuffers[in_image_index];
    render_pass_info.renderArea.offset = {0,0};
    render_pass_info.renderArea.extent = swap_chain_info_.swap_chain_extent;

    std::array<VkClearValue, 2> clear_colors;
    
    float r = 0.5;
    float g = 0.5;
    float b = 0.5;

    clear_colors[0].color = {{
            r,
            g,
            b,
            1.f}};

    clear_colors[1].depthStencil = {1.f, 0};

    render_pass_info.clearValueCount = clear_colors.size();
    render_pass_info.pClearValues = clear_colors.data();

    vkCmdBeginRenderPass(
        // in_commandbuffer,
        render_command_buffer_.command_buffers[in_frame_index],
        &render_pass_info,
        VK_SUBPASS_CONTENTS_INLINE
        );

    vkCmdBindPipeline(
        // in_commandbuffer,
        render_command_buffer_.command_buffers[in_frame_index],
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        render_pipeline.pipeline
        );

    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = static_cast<float>(swap_chain_info_.swap_chain_extent.width);
    viewport.height = static_cast<float>(swap_chain_info_.swap_chain_extent.height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    vkCmdSetViewport(
        // in_commandbuffer,
        render_command_buffer_.command_buffers[in_frame_index],
        0, 1,
        &viewport
        );

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swap_chain_info_.swap_chain_extent;
    vkCmdSetScissor(
        // in_commandbuffer,
        render_command_buffer_.command_buffers[in_frame_index],
        0, 1,
        &scissor
        );

    for (auto & bid : pipelines_manager_.IterateBindings(in_pipeline_id))
    {

        // TODO Update Descriptor Actor UBO data

        auto& binding = pipelines_manager_.Binding(bid);

        const WVkDescriptorSetInfo & descriptor =
            pipelines_manager_.DescriptorSet(binding.descriptor_set_id);

        auto& mesh_info =
            render_resources_.StaticMeshInfo(
                binding.mesh_asset_id
                );

        VkBuffer vertex_buffers[] = {mesh_info.vertex_buffer};
        VkDeviceSize offsets[] = {0};
        
        vkCmdBindVertexBuffers(
            // in_commandbuffer,
            render_command_buffer_.command_buffers[in_frame_index],
            0,
            1,
            vertex_buffers,
            offsets
            );

        vkCmdBindIndexBuffer(
            // in_commandbuffer,
            render_command_buffer_.command_buffers[in_frame_index],
            mesh_info.index_buffer,
            0,
            VK_INDEX_TYPE_UINT32
            );

        vkCmdBindDescriptorSets(
            render_command_buffer_.command_buffers[in_frame_index],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            render_pipeline.pipeline_layout,
            0,
            1,
            &descriptor.descriptor_sets[in_frame_index],
            0,
            nullptr
            );

        vkCmdDrawIndexed(
            render_command_buffer_.command_buffers[in_frame_index],
            mesh_info.index_count,
            1,
            0,
            0,
            0
            );
    }

    vkCmdEndRenderPass(render_command_buffer_.command_buffers[in_frame_index]);

    if(vkEndCommandBuffer(render_command_buffer_.command_buffers[in_frame_index]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

void WRender::ClearPipelines() {
    pipelines_manager_.Clear();
}

void WRender::UnloadAllResources() {
    render_resources_.Clear();
}

void WRender::Destroy() {
    pipelines_manager_.Destroy();

    WVulkan::Destroy(image_available_semaphore_, device_info_);

    WVulkan::Destroy(render_finished_semaphore_, device_info_);

    WVulkan::Destroy(flight_fence_, device_info_);
    
    // Destroy Vulkan Render Pass
    WVulkan::Destroy(render_pass_info_, device_info_);

    // Destroy Swap Chain and Image Views
    WVulkan::Destroy(swap_chain_info_, device_info_);

    render_command_pool_.Clear();
    render_command_pool_ = {};
    
    render_command_buffer_ = {};

    render_resources_.Clear();

    // Destroy Vulkan Device
    WVulkan::Destroy(device_info_);

    // Destroy Vulkan Surface
    WVulkan::Destroy(surface_info_, instance_info_);

    // Destroy Vulkan Instance
    WVulkan::Destroy(instance_info_);

    // Destroy Window
    WVulkan::Destroy(window_info_);    
}

