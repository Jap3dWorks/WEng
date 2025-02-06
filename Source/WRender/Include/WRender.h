// WEng Render Header File

#pragma once

#include "WCore/WCore.h"
#include "WRenderCore.h"
#include "WRenderPipeline.h"
#include <optional>


/**
 * @brief Render related top class. Highest level abstraction.
*/
class WRENDER_API WRender
{
public:

    WId wid;
    WRender();
    ~WRender();

    void DrawFrame();

    inline WRenderPipelinesManager& RenderPipelinesManager()
    {
	return render_pipelines_manager_;
    }

private:

    WInstanceInfo instance_info_;
    WWindowInfo window_info_;
    WSurfaceInfo surface_info_;
    WDeviceInfo device_info_;
    WRenderPipelinesManager render_pipelines_manager_;
    WRenderDebugInfo debug_info_;

    WSwapChainInfo swap_chain_info_;
    WRenderPassInfo render_pass_info_;

    WCommandPoolInfo command_pool_info_;

    void initialize();
};
