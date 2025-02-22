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

void WRender::FrameBufferSizeCallback(GLFWwindow* in_window, int, int)
{
    auto app = reinterpret_cast<WRender*>(glfwGetWindowUserPointer(in_window));
    app->frame_buffer_resized = true;
}

WRender::WRender() 
{
    instance_info_ = {};
    instance_info_.wid = {}; // WId::GenerateId();

    window_info_ = {};
    window_info_.wid = {}; // WId::GenerateId();
    window_info_.user_pointer = this;
    window_info_.framebuffer_size_callback = FrameBufferSizeCallback;

    surface_info_ = {};
    surface_info_.wid = {}; // WId::GenerateId();

    device_info_ = {};
    device_info_.wid = {}; // WId::GenerateId();

    debug_info_ = {};
    debug_info_.wid = {}; // WId::GenerateId();
    debug_info_.enable_validation_layers = _ENABLE_VALIDATON_LAYERS;

    render_command_pool_info_ = {};
    render_command_pool_info_.wid = {};

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

    render_pipelines_manager_ = WRenderPipelinesManager(
        device_info_,
        render_pass_info_
	);

    render_command_pool_ = WRenderCommandPool(  // TODO fix Memory error Here!
        WCommandPoolInfo(),
        device_info_,
        surface_info_
	);

    // TODO CHECK Create Depth and color Resources
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
    
    uint32_t image_index;

    VkResult result = vkAcquireNextImageKHR(
        device_info_.vk_device,
        swap_chain_info_.swap_chain,
        UINT64_MAX,
        image_available_semaphore_.semaphores[current_frame],
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
        &in_flight_fence_.fences[current_frame]
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

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frame_buffer_resized) {
        frame_buffer_resized = false;
        RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

}

void WRender::RecreateSwapChain()
{
    int width=0, height=0;
    glfwGetFramebufferSize(window_info_.window, &width, &height);

    while(width ==0 || height == 0) {
        glfwGetFramebufferSize(window_info_.window, &width, &height);
        glfwWaitEvents();
    }

    DeviceWaitIdle();

    WVulkan::Destroy(swap_chain_info_, device_info_);

    WVulkan::Create(
        swap_chain_info_,
        device_info_,
        surface_info_,
        window_info_,
        render_pass_info_,
        debug_info_
        );
}
