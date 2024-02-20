#pragma once
#include "WCore/WCore.h"
#include "WRenderCore.h"
#include "WRender.h"
#include "WShader.h"
#include "WRenderPipeline.h"


struct WTextureStruct;
struct WMeshStruct;


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

    WShaderModuleManager CreateShaderModule(const WDeviceInfo& device, const WShaderStageInfo& out_shader_info);

    void CreateVkTexture(
        WTextureInfo& out_texture_info, 
        const WTextureStruct& texture_struct,
        const WDeviceInfo& device_info,
        const WCommandPoolInfo& command_pool_info
    );

    void CreateVkRenderPipeline(
        const WDeviceInfo &device, 
        const WDescriptorSetLayoutInfo& descriptor_set_layout_info,
        const WRenderPassInfo& render_pass_info, 
        WRenderPipelineInfo& out_pipeline_info
    );

    void CreateVkDescriptorSetLayout(const WDeviceInfo &device, WDescriptorSetLayoutInfo& out_descriptor_set_layout_info);

    void CreateVkMesh(
        WMeshInfo& out_mesh_info,
        const WMeshStruct& mesh_struct,
        const WDeviceInfo &device,
        const WCommandPoolInfo &command_pool_info
    );

    void CreateVkUniformBuffer(
        WUniformBufferObjectInfo& out_uniform_buffer_info,
        const WDeviceInfo &device
    );

    // Destroy functions
    // -----------------

    void DestroyInstance(WInstanceInfo &instance_info);

    void DestroySurface(WSurfaceInfo &surface_info, const WInstanceInfo &instance_info);

    void DestroyDevice(WDeviceInfo &device_info);

    void DestroySwapChain(WSwapChainInfo &swap_chain_info, const WDeviceInfo &device_info);

    void DestroyImageViews(WSwapChainInfo &swap_chain_info, const WDeviceInfo &device_info);

    void DestroyRenderPass(WRenderPassInfo &render_pass_info, const WDeviceInfo &device_info);

    void DestroyWindow(WWindowInfo &window_info);

    void DestroyVkRenderPipeline(const WDeviceInfo &device, const WRenderPipelineInfo& pipeline_info);
    
    void DestroyDescriptorSetLayout(const WDeviceInfo &device, const WDescriptorSetLayoutInfo& descriptor_set_layout_info); 


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

    void CreateVkImage(
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

    VkImageView CreateVkImageView(
        const VkDevice& device, 
        const VkImage& image, 
        const VkFormat& format, 
        const VkImageAspectFlags& aspect_flags, 
        const uint32_t& mip_levels
    );

    VkSampler CreateVkTextureSampler(
        const VkDevice& device, 
        const VkPhysicalDevice& physical_device,
        const uint32_t& mip_levels
    );

    void CreateVkBuffer(
        const VkDevice& device,
        const VkPhysicalDevice& physical_device,
        VkDeviceSize size, 
        VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties, 
        VkBuffer& out_buffer, 
        VkDeviceMemory& out_buffer_memory
    );

    void TransitionImageLayout(
        const VkDevice& device, 
        const VkCommandPool& command_pool, 
        const VkQueue& graphics_queue, 
        const VkImage& image, 
        const VkFormat& format, 
        const VkImageLayout& old_layout, 
        const VkImageLayout& new_layout, 
        const uint32_t& mip_levels
    );

    void CopyVkBuffer(
        const VkDevice& device, 
        const VkCommandPool& command_pool, 
        const VkQueue& graphics_queue, 
        const VkBuffer& src_buffer, 
        const VkBuffer& dst_buffer, 
        const VkDeviceSize& size
    );
    
    /**
     * @brief Enum representing the shader stage flags for Vulkan.
     */
    VkShaderStageFlagBits ToShaderStageFlagBits(const WShaderType& type);

    VkCommandBuffer BeginSingleTimeCommands(
        const VkDevice& device, 
        const VkCommandPool& command_pool
    );

    void EndSingleTimeCommands(
        const VkDevice& device, 
        const VkCommandPool& command_pool, 
        const VkQueue& graphics_queue, 
        const VkCommandBuffer& command_buffer
    );

}