// WEng Render Header File

#pragma once

#include "WCore.h"
#include "WRenderCore.h"
#include "WRenderPipeline.h"
#include <optional>

class GLFWwindow;

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
    void* user_pointer = nullptr;
    GLFWwindow* window = nullptr;

};

struct WRenderDebugInfo
{
    WId wid;
    bool enable_validation_layers = false;
    std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    PFN_vkDebugUtilsMessengerCallbackEXT debug_callback = nullptr;
    VkDebugUtilsMessengerEXT debug_messenger = nullptr;
};

struct WSwapChainInfo
{
    WId wid;
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;
    
    std::vector<VkImage> swap_chain_images;
    std::vector<VkImageView> swap_chain_image_views;
    
    VkImage color_image;
    VkDeviceMemory color_image_memory;
    VkImageView color_image_view;    

    VkImage depth_image;
    VkDeviceMemory depth_image_memory;
    VkImageView depth_image_view;

    std::vector<VkFramebuffer> swap_chain_framebuffers;

    VkSwapchainKHR swap_chain = nullptr;
};

struct WRenderPassInfo
{
    WId wid;
    VkRenderPass render_pass = nullptr;
};

struct WCommandPoolInfo
{
    WId wid;
    VkCommandPool vk_command_pool = nullptr;
};

class WRENDER_API WRender
{
public:
    WId wid;
    WRender();
    ~WRender();

    void DrawFrame();

private:
    WInstanceInfo instance_info_;
    WWindowInfo window_info_;
    WSurfaceInfo surface_info_;
    WDeviceInfo device_info_;
    WRenderPipelines render_pipelines_;
    WRenderDebugInfo debug_info_;

    WSwapChainInfo swap_chain_info_;
    WRenderPassInfo render_pass_info_;

    WCommandPoolInfo command_pool_info_;

    void initialize();
};
