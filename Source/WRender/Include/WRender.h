// WEng Render Header File

#pragma once

#include "WCore.h"
#include "WRenderCore.h"
#include "WRenderPipeline.h"


class GLFWwindow;

class WRENDER_API WRenderPipeline
{
public:
    WRenderPipeline() {}
};

struct WInstanceInfo
{
    WId wid;
    VkInstance instance = nullptr;
};

struct WSurfaceInfo
{
    WId wid;
    VkSurfaceKHR surface = nullptr;
};

struct WWindowInfo
{
    WId wid;
    std::string title = "WEngine";
    uint32_t width = 800;
    uint32_t height = 600;

    GLFWframebuffersizefun framebuffer_size_callback = nullptr;
    GLFWwindow* window = nullptr;

};

class WRENDER_API WRender
{
public:
    WId wid;
    WRender();
    ~WRender();

private:
    WInstanceInfo instance_info_;
    WWindowInfo window_info_;
    WSurfaceInfo surface_info_;
    WDeviceInfo device_;
    WRenderPipelines render_pipelines_;

    std::string name_;

    void initialize();
};


namespace WVulkan
{
    /**
     * Checks if the requested validation layers are available.
    */
    bool CheckValidationLayerSupport();

    /**
     * Creates a Vulkan Instance.
    */
    void CreateInstance(WInstanceInfo &out_instance_info, bool enable_validation_layers=false);

    /**
     * Creates a Vulkan Surface.
    */
    void CreateSurface(WSurfaceInfo &surface_info, WInstanceInfo instance, WWindowInfo window);

    /**
     * Creates a GLFW Window.
    */
    void InitWindow(WWindowInfo &info);

    WDeviceInfo CreateDevice(WDeviceInfo Device, instance_info, WSurfaceInfo surface_info);

    /**
     * Return a list of required vulkan instance extensions.
    */
    std::vector<const char*> GetRequiredExtensions(bool enable_validation_layers=false);


}