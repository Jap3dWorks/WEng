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
    VkCommandPool command_pool = nullptr;
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

    // std::string name_;

    void initialize();
};

namespace WVulkan
{
    // Create functions
    // ----------------

    /**
     * Creates a Vulkan Instance.
    */
    void CreateInstance(WInstanceInfo &out_instance_info, const WRenderDebugInfo &debug_info);

    /**
     * Creates a Vulkan Surface.
    */
    void CreateSurface(WSurfaceInfo &surface_info, const WInstanceInfo &instance, const WWindowInfo &window);

    /**
     * Creates a Vulkan Device.
    */
    void CreateDevice(WDeviceInfo &Device, const WInstanceInfo &instance_info, const WSurfaceInfo &surface_info, const WRenderDebugInfo &debug_info);

    /**
     * Creates a Vulkan Swap Chain.
    */
    void CreateSwapChain(
        WSwapChainInfo &out_swap_chain, 
        const WDeviceInfo &device_info, 
        const WSurfaceInfo &surface_info, 
        const WWindowInfo &window_info, 
        const WRenderPassInfo &render_pass_info, 
        const WRenderDebugInfo &debug_info
    );

    /**
     * Creates Vulkan Image Views.
    */
    void CreateImageViews(WSwapChainInfo &swap_chain_info, const WDeviceInfo &device_info);

    /**
     * Creates a Vulkan Render Pass.
    */
    void CreateRenderPass(WRenderPassInfo& render_pass, const WSwapChainInfo &swap_chain_info, const WDeviceInfo&);

    /**
     * Creates a Vulkan Command Pool.
    */
    void CreateCommandPool(WCommandPoolInfo& command_pool_info, const WDeviceInfo& device_info, const WSurfaceInfo& surface_info);

    /**
     * Creates a GLFW Window.
    */
    void CreateWindow(WWindowInfo &info);

    // Destroy functions
    // -----------------

    void DestroyInstance(WInstanceInfo &instance_info);

    void DestroySurface(WSurfaceInfo &surface_info, const WInstanceInfo &instance_info);

    void DestroyDevice(WDeviceInfo &device_info);

    void DestroySwapChain(WSwapChainInfo &swap_chain_info, const WDeviceInfo &device_info);

    void DestroyImageViews(WSwapChainInfo &swap_chain_info, const WDeviceInfo &device_info);

    void DestroyRenderPass(WRenderPassInfo &render_pass_info, const WDeviceInfo &device_info);

    void DestroyWindow(WWindowInfo &window_info);

    // Helper Functions
    // ----------------

    /**
     * Checks if the requested validation layers are available.
    */
    bool CheckValidationLayerSupport(const WRenderDebugInfo &debug_info);

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool IsComplete()
        {
            return graphics_family.has_value() && present_family.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats);

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes);

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow* window);

    /**
     * Return a list of required vulkan instance extensions.
    */
    std::vector<const char*> GetRequiredExtensions(bool enable_validation_layers=false);

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT InMessageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT InMessageType,
        const VkDebugUtilsMessengerCallbackDataEXT* InCallbackData,
        void* InUserData
    );

    bool IsDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, const std::vector<const char*>& device_extensions);

    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device, const std::vector<const char*>& device_extensions);

    VkFormat FindSupportedFormat(const VkPhysicalDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkFormat FindDepthFormat(const VkPhysicalDevice& device);

    uint32_t FindMemoryType(const VkPhysicalDevice& device, uint32_t type_filter, VkMemoryPropertyFlags properties);

    void CreateImage(
        const VkDevice& device, 
        const VkPhysicalDevice& physical_device,
        const uint32_t& width, const uint32_t& height, 
        const uint32_t& mip_levels, 
        const VkSampleCountFlagBits& samples,
        const VkFormat& format, 
        const VkImageTiling& tiling, 
        const VkImageUsageFlags& usage, 
        const VkMemoryPropertyFlags& properties,
        VkImage& out_image,
        VkDeviceMemory& out_image_memory
    );

    VkImageView CreateImageView(
        const VkDevice& device, 
        const VkImage& image, 
        const VkFormat& format, 
        const VkImageAspectFlags& aspect_flags, 
        const uint32_t& mip_levels
    );

}