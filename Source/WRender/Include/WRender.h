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
    VkSurfaceKHR surface = nullptr;
    GLFWwindow* window = nullptr;
};

class WRENDER_API WRender
{

public:
    WRender() {}
    ~WRender();

private:
    WInstanceInfo instance_info_;
    WDevice device_;
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
    VkInstance CreateInstance(bool enable_validation_layers=false);

    /**
     * Return a list of required vulkan instance extensions.
    */
    std::vector<const char*> GetRequiredExtensions(bool enable_validation_layers=false);
}