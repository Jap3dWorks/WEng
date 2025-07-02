// WEng Render Header File

#pragma once

#include "WCore/WCore.hpp"
#include "WCore/CoreMacros.hpp"
#include "WVulkan/WVkRenderConfig.h"
#include "WVulkan/WVkRenderCore.hpp"
#include "WVulkan/WVkRenderPipeline.hpp"
#include "WVulkan/WVkRenderCommandPool.h"
#include "WVulkan/WVkAssetCollection.h"

#include <cstddef>

class IRenderResources;

/**
 * @brief Render related top class. Highest level abstraction.
*/
class WRENDER_API WRender
{
public:

    WRender();

    ~WRender();

    void Draw();

    WNODISCARD WVkRenderPipelinesManager & RenderPipelinesManager()
    {
        return render_pipelines_manager_;
    }

    void DeviceWaitIdle() const;

    void RecreateSwapChain();

    TRef<IRenderResources> RenderResources();

    static void FrameBufferSizeCallback(GLFWwindow*, int, int);

    WNODISCARD const WVkWindowInfo & WindowInfo() const noexcept
    { return window_info_; }

    WNODISCARD const WVkDeviceInfo & DeviceInfo() const noexcept
    { return device_info_; }

    WNODISCARD const WVkSwapChainInfo & SwapChainInfo() const noexcept
    { return swap_chain_info_; }

    WNODISCARD const size_t FramesInFlight() const noexcept
    { return MAX_FRAMES_IN_FLIGHT; }

    WNODISCARD const WVkRenderCommandPool & RenderCommandPool() const noexcept
    { return render_command_pool_; }

    void AddPipelineBinding(
        WId pipeline_id,
        WId descriptor_set_id,
        WId in_mesh_id
        );

private:

    std::unique_ptr<IRenderResources> render_resources_{nullptr};

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

