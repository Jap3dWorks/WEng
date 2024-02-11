#include "WRender.h"

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

WRender::WRender() : render_pipelines_(device_info_)
{
    instance_info_ = {};
    instance_info_.wid = 0; // WId::GenerateId();

    window_info_ = {};
    window_info_.wid = 0; // WId::GenerateId();
    window_info_.user_pointer = this;

    surface_info_ = {};
    surface_info_.wid = 0; // WId::GenerateId();

    device_info_ = {};
    device_info_.wid = 0; // WId::GenerateId();

    debug_info_ = {};
    debug_info_.wid = 0; // WId::GenerateId();
    debug_info_.enable_validation_layers = _ENABLE_VALIDATON_LAYERS;
}

void WRender::initialize()
{
    // Create Vulkan Instance
    WVulkan::CreateInstance(
        instance_info_,
        debug_info_
    );

    WVulkan::CreateWindow(
        window_info_
    );

    WVulkan::CreateSurface(
        surface_info_,
        instance_info_, 
        window_info_
    );

    // Create Vulkan Device
    WVulkan::CreateDevice(
        device_info_, 
        instance_info_, 
        surface_info_,
        debug_info_
    );

    // Create Vulkan Swap Chain
    WVulkan::CreateSwapChain(
        swap_chain_info_,
        device_info_,
        surface_info_,
        window_info_,
        render_pass_info_,
        debug_info_
    );

    // Create Vulkan Image Views
    WVulkan::CreateImageViews(
        swap_chain_info_,
        device_info_
    );

    // Create Vulkan Render Pass
    WVulkan::CreateRenderPass(
        render_pass_info_,
        swap_chain_info_,
        device_info_
    );
}

WRender::~WRender()
{
    // Destroy Vulkan Render Pass
    WVulkan::DestroyRenderPass(render_pass_info_, device_info_);

    // Destroy Vulkan Image Views
    WVulkan::DestroyImageViews(swap_chain_info_, device_info_);

    // Destroy Vulkan Swap Chain
    WVulkan::DestroySwapChain(swap_chain_info_, device_info_);
    
    // Destroy Vulkan Device
    WVulkan::DestroyDevice(device_info_);

    // Destroy Vulkan Surface
    WVulkan::DestroySurface(surface_info_, instance_info_);

    // Destroy Vulkan Instance
    WVulkan::DestroyInstance(instance_info_);

    // Destroy Window
    WVulkan::DestroyWindow(window_info_);
}

void WRender::DrawFrame()
{
    
}

