// WEng Render Header File

#pragma once

#include "WCore/CoreMacros.h"
#include "WCore/WCore.h"
#include "WVulkan/WVkRenderConfig.h"
#include "WVulkan/WVkRenderCore.h"
#include "WVulkan/WVkRenderPipeline.h"
#include "WVulkan/WVkRenderCommandPool.h"
#include <cstddef>
#include <optional>

/**
 * @brief Render related top class. Highest level abstraction.
*/
class WRENDER_API WRender
{
public:

    WRender();

    ~WRender();

    void Draw();

    WNODISCARD WINLINE WVkRenderPipelinesManager & RenderPipelinesManager()
    {
        return render_pipelines_manager_;
    }

    void DeviceWaitIdle() const;

    void RecreateSwapChain();

    static void FrameBufferSizeCallback(GLFWwindow*, int, int);

    WNODISCARD inline const WVkWindowInfo & WindowInfo() const noexcept
    { return window_info_; }

    WNODISCARD inline const WVkDeviceInfo & DeviceInfo() const noexcept
    { return device_info_; }

    WNODISCARD inline const WVkSwapChainInfo & SwapChainInfo() const noexcept
    { return swap_chain_info_; }

    WNODISCARD inline const size_t FramesInFlight() const noexcept
    { return MAX_FRAMES_IN_FLIGHT; }

    WNODISCARD inline const WVkRenderCommandPool & RenderCommandPool() const noexcept
    { return render_command_pool_; }

private:

    WVkInstanceInfo instance_info_;
    WVkWindowInfo window_info_;
    WVkSurfaceInfo surface_info_;
    WVkDeviceInfo device_info_;
    WVkRenderDebugInfo debug_info_;

    WVkSwapChainInfo swap_chain_info_;
    WVkRenderPassInfo render_pass_info_;

    WVkRenderCommandPool render_command_pool_;
    WVkCommandBufferInfo render_command_buffer_;
    
    WVkRenderPipelinesManager render_pipelines_manager_;

    WVkSemaphoreInfo image_available_semaphore_;
    WVkSemaphoreInfo render_finished_semaphore_;
    WVkFenceInfo in_flight_fence_;

    uint32_t current_frame {0};

    bool frame_buffer_resized {false};

};

