#include "WRender.h"
#include "WCore/WCore.h"
#include "WRenderCommandPool.h"
#include "WRenderConfig.h"
#include "WRenderCore.h"
#include "WRenderPipeline.h"
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstring>
#include <set>
#include <string>
#include <array>

#ifdef NDEBUG
    #define _ENABLE_VALIDATON_LAYERS false
#else
    #define _ENABLE_VALIDATON_LAYERS true
#endif

#include "WVulkan.h"

// WRender
// -------

WRender::WRender() 
{
    instance_info_ = {};
    instance_info_.wid = {}; // WId::GenerateId();

    window_info_ = {};
    window_info_.wid = {}; // WId::GenerateId();
    window_info_.user_pointer = this;

    surface_info_ = {};
    surface_info_.wid = {}; // WId::GenerateId();

    device_info_ = {};
    device_info_.wid = {}; // WId::GenerateId();

    debug_info_ = {};
    debug_info_.wid = {}; // WId::GenerateId();
    debug_info_.enable_validation_layers = _ENABLE_VALIDATON_LAYERS;

    render_command_pool_info_ = {};
    render_command_pool_info_.wid = {};

    // Create Vulkan Instance
    WVulkan::Create(
        instance_info_,
        debug_info_
    );

    WVulkan::Create(
        window_info_
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
    WVulkan::CreateImageView(
        swap_chain_info_,
        device_info_
    );

    // Create Vulkan Render Pass
    WVulkan::Create(
        render_pass_info_,
        swap_chain_info_,
        device_info_
    );

    render_pipelines_manager_ = WRenderPipelinesManager(
        device_info_,
        render_pass_info_
	);

    render_command_pool_ = WRenderCommandPool(
        WCommandPoolInfo(),
        device_info_,
        surface_info_
	);

    render_command_buffer_ = render_command_pool_.CreateCommandBuffer();

    WVulkan::Create(
        image_available_semaphore_,
        device_info_
        );

    WVulkan::Create(
        render_finished_semaphore_,
        device_info_
        );

    WVulkan::Create(
        in_flight_fence_,
        device_info_
        );
}

WRender::~WRender()
{

    WVulkan::Destroy(image_available_semaphore_, device_info_);

    WVulkan::Destroy(render_finished_semaphore_, device_info_);

    WVulkan::Destroy(in_flight_fence_, device_info_);
    
    // Destroy Vulkan Render Pass
    WVulkan::Destroy(render_pass_info_, device_info_);

    // Destroy Vulkan Image Views
    WVulkan::Destroy(swap_chain_info_, device_info_);

    // Destroy Vulkan Swap Chain
    WVulkan::Destroy(swap_chain_info_, device_info_);
    
    // Destroy Vulkan Device
    WVulkan::Destroy(device_info_);

    // Destroy Vulkan Surface
    WVulkan::Destroy(surface_info_, instance_info_);

    // Destroy Vulkan Instance
    WVulkan::Destroy(instance_info_);

    // Destroy Window
    WVulkan::Destroy(window_info_);
}

void WRender::DeviceWaitIdle() const
{
    vkDeviceWaitIdle(device_info_.vk_device);
}

void WRender::DrawFrame()
{
    vkWaitForFences(
        device_info_.vk_device,
        1,
        &in_flight_fence_.fences[current_frame],
        VK_TRUE,
        UINT64_MAX
        );
    
    vkResetFences(
        device_info_.vk_device,
        1,
        &in_flight_fence_.fences[current_frame]
        );

    uint32_t image_index;

    vkAcquireNextImageKHR(
        device_info_.vk_device,
        swap_chain_info_.swap_chain,
        UINT64_MAX,
        image_available_semaphore_.semaphores[current_frame],
        VK_NULL_HANDLE,
        &image_index
        );

    VkSemaphore signal_semaphores[] = {render_finished_semaphore_.semaphores[current_frame]};

    for(WRenderPipeline& render_pipeline : render_pipelines_manager_.RenderPipelines()[WPipelineType::Graphics])
    {
        vkResetCommandBuffer(render_command_buffer_.command_buffers[current_frame], 0);

        WVulkan::RecordRenderCommandBuffer(
            render_command_buffer_,
            render_pass_info_,
            swap_chain_info_,
            render_pipeline.RenderPipelineInfo(),
            current_frame
            );

        VkSubmitInfo submit_info;
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore wait_semaphores[] = {image_available_semaphore_.semaphores[current_frame]};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;

        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &render_command_buffer_.command_buffers[current_frame];

        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;
        
        if (vkQueueSubmit(
                device_info_.vk_graphics_queue,
                1,
                &submit_info,
                in_flight_fence_.fences[current_frame]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit draw command buffer");
        }
    }

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swap_chains[] = {swap_chain_info_.swap_chain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;

    vkQueuePresentKHR(device_info_.vk_present_queue, &present_info);

    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

}

