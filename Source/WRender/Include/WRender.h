// WEng Render Header File

#pragma once

#include "WCore/CoreMacros.h"
#include "WCore/WCore.h"
#include "WRenderConfig.h"
#include "WRenderCore.h"
#include "WRenderPipeline.h"
#include "WRenderCommandPool.h"
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

    WNODISCARD WINLINE WId Wid()
    {
        return wid_;
    }

    void Draw();

    WNODISCARD WINLINE WRenderPipelinesManager & RenderPipelinesManager()
    {
        return render_pipelines_manager_;
    }

    void DeviceWaitIdle() const;

    void RecreateSwapChain();

    static void FrameBufferSizeCallback(GLFWwindow*, int, int);

    WNODISCARD inline const WWindowInfo & WindowInfo() const noexcept
    { return window_info_; }

    WNODISCARD inline const WDeviceInfo & DeviceInfo() const noexcept
    { return device_info_; }

    WNODISCARD inline const WSwapChainInfo & SwapChainInfo() const noexcept
    { return swap_chain_info_; }

    WNODISCARD inline const size_t FramesInFlight() const noexcept
    { return MAX_FRAMES_IN_FLIGHT; }

private:

    WId wid_;

    WInstanceInfo instance_info_;
    WWindowInfo window_info_;
    WSurfaceInfo surface_info_;
    WDeviceInfo device_info_;
    WRenderDebugInfo debug_info_;

    WSwapChainInfo swap_chain_info_;
    WRenderPassInfo render_pass_info_;

    WCommandPoolInfo render_command_pool_info_;

    WRenderCommandPool render_command_pool_;
    WCommandBufferInfo render_command_buffer_;
    
    WRenderPipelinesManager render_pipelines_manager_;

    WSemaphoreInfo image_available_semaphore_;
    WSemaphoreInfo render_finished_semaphore_;
    WFenceInfo in_flight_fence_;

    uint32_t current_frame {0};

    bool frame_buffer_resized {false};

};

