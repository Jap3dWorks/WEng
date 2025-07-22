#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVkRenderCore.hpp"
#include "WVulkan/WVkRenderPipeline.hpp"
#include "WVulkan/WVkRenderCommandPool.hpp"
#include "WCore/TRef.hpp"
#include "WEngineInterfaces/IRender.hpp"

#include <cstddef>

// class IRenderResources;

/**
 * @brief Render related top class. Highest level abstraction.
*/
class WRENDER_API WRender : public IRender
{
public:

    WRender();

    ~WRender() override;

    void Initialize();

    void Draw() override ;

    WId CreateRenderPipeline(
        EPipelineType in_pipeline_type,
        const std::vector<std::string> & in_shader_files,
        const std::vector<EShaderType> & in_shader_types
        ) override;

    void AddPipelineBinding(
        WId pipeline_id,
        WId in_mesh_id,
        const std::vector<WId> & in_textures,
        const std::vector<uint32_t> & in_textures_bindings
        ) override ;

    void Clear() override;


    WNODISCARD WVkRenderPipelinesManager & RenderPipelinesManager()
    {
        return pipelines_manager_;
    }

    void WaitIdle() const override;

    WNODISCARD TRef<IRenderResources> RenderResources() override;

    WNODISCARD GLFWwindow * Window() const noexcept override {
        return window_info_.window;
    }

    static void FrameBufferSizeCallback(GLFWwindow*, int, int);

    WNODISCARD const WVkWindowInfo & WindowInfo() const noexcept
    { return window_info_; }

    WNODISCARD const WVkDeviceInfo & DeviceInfo() const noexcept
    { return device_info_; }

    WNODISCARD const WVkSwapChainInfo & SwapChainInfo() const noexcept
    { return swap_chain_info_; }

    WNODISCARD const size_t FramesInFlight() const noexcept
    { return WENG_MAX_FRAMES_IN_FLIGHT; }

    WNODISCARD const WVkRenderCommandPool & RenderCommandPool() const noexcept
    { return render_command_pool_; }

private:

    void RecreateSwapChain();

    void RecordRenderCommandBuffer(WId in_pipeline_id, uint32_t in_frame_index, uint32_t in_image_index);

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
    
    WVkRenderPipelinesManager pipelines_manager_;

    WVkSemaphoreInfo image_available_semaphore_;
    WVkSemaphoreInfo render_finished_semaphore_;
    WVkFenceInfo flight_fence_;

    uint32_t frame_index {0};

    bool frame_buffer_resized {false};

};

