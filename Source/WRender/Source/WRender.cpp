#include "WRender.h"
#include "WRenderCommandPool.h"
#include "WRenderCore.h"
#include "WRenderPipeline.h"

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
}

WRender::~WRender()
{
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

void WRender::DrawFrame()
{
    // TODO
}

