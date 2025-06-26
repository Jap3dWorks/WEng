#pragma once
#include "WCore/WCore.h"
#include "WVulkan/WVkRenderCore.h"
#include "WVulkan/WVkRenderPipeline.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>
#include <optional>

struct WTextureStruct;
struct WMeshStruct;

namespace WVulkan
{
    // Create functions
    // ----------------

    /**
     * Creates a Vulkan Instance.
    */
    void Create(WVkInstanceInfo &out_instance_info, const WVkRenderDebugInfo &debug_info);

    /**
     * Creates a Vulkan Surface.
    */
    void Create(WVkSurfaceInfo & surface_info, WVkInstanceInfo & instance, WVkWindowInfo & window);

    /**
     * Creates a Vulkan Device.
    */
    void Create(WVkDeviceInfo &Device, const WVkInstanceInfo &instance_info, const WVkSurfaceInfo &surface_info, const WVkRenderDebugInfo &debug_info);

    /**
     * Creates a Vulkan Swap Chain.
    */
    void Create(
        WVkSwapChainInfo & out_swap_chain, 
        const WVkDeviceInfo &device_info, 
        const WVkSurfaceInfo &surface_info, 
        const WVkWindowInfo &window_info, 
        const WVkRenderPassInfo &render_pass_info, 
        const WVkRenderDebugInfo &debug_info
        );

    /**
     * @brief Creates a Vulkan Render Pass.
    */
    void Create(WVkRenderPassInfo & render_pass, const WVkSwapChainInfo &swap_chain_info, const WVkDeviceInfo&);

    /**
     * @brief Creates a Vulkan Command Pool.
    */
    void Create(
        WVkCommandPoolInfo & command_pool_info,
        const WVkDeviceInfo & device_info,
        const WVkSurfaceInfo & surface_info
	);

    /**
     * @brief: Creates a GLFW Window.
    */
    void Create(WVkWindowInfo &info);

    VkShaderModule CreateShaderModule(
        const WVkShaderStageInfo & in_shader_info,
        const WVkDeviceInfo & in_device
    );

    void Create(
        WVkTextureInfo& out_texture_info, 
        const WTextureStruct& texture_struct,
        const WVkDeviceInfo& device_info,
        const WVkCommandPoolInfo& command_pool_info
    );

    void Create(
        WVkRenderPipelineInfo &out_pipeline_info,
        const WVkDeviceInfo &device,
        const WVkDescriptorSetLayoutInfo &descriptor_set_layout_info,
        const WVkRenderPassInfo &render_pass_info,
        const std::vector<WVkShaderStageInfo> &in_shader_stage_infos // ,
        // const std::vector<WShaderModule> &in_shader_modules
        );

    void Create(
        WVkDescriptorSetLayoutInfo& out_descriptor_set_layout_info,
        const WVkDeviceInfo &device
        );

    /**
     * @brief Create a vulkan  mesh
     */
    void Create(
        WVkMeshInfo & out_mesh_info,
        const WMeshStruct & mesh_struct,
        const WVkDeviceInfo & device,
        const WVkCommandPoolInfo & command_pool_info
    );

    void Create(
        WVkUniformBufferObjectInfo & out_uniform_buffer_info,
        const WVkDeviceInfo & device
    );

    void Create(
        WVkDescriptorPoolInfo & out_descriptor_pool_info,
        const WVkDeviceInfo & device
    );

    void Create(
        WVkDescriptorSetInfo& out_descriptor_set_info,
        const WVkDeviceInfo &device,
        const WVkDescriptorSetLayoutInfo& descriptor_set_layout_info,
        const WVkDescriptorPoolInfo& descriptor_pool_info // ,
        // const std::vector<VkWriteDescriptorSet>& write_descriptor_sets
    );
 
    // ----------------
 
    struct WVkWriteDescriptorSetUBOStruct
    {
        uint32_t binding;
        WVkUniformBufferObjectInfo uniform_buffer_info;
        VkDescriptorSet descriptor_set;

        VkDescriptorBufferInfo buffer_info{};
    };

    struct WVkWriteDescriptorSetTextureStruct
    {
        uint32_t binding;
        WVkTextureInfo texture_info;
        VkDescriptorSet descriptor_set;

        VkDescriptorImageInfo image_info{};
    };


    /**
     * @brief Create a WCommandBufferInfo.
    */
    void Create(
        WVkCommandBufferInfo & out_command_buffer_info,
        const WVkDeviceInfo & device,
        const WVkCommandPoolInfo & command_pool_info
    );

    void Create(
        VkImage& out_image,
        VkDeviceMemory& out_image_memory,
        const VkDevice& device, 
        const VkPhysicalDevice& physical_device,
        const uint32_t& width,
        const uint32_t& height, 
        const uint32_t& mip_levels, 
        const VkSampleCountFlagBits& samples,
        const VkFormat& format, 
        const VkImageTiling& tiling, 
        const VkImageUsageFlags& usage, 
        const VkMemoryPropertyFlags& properties
    );

    VkImageView CreateImageView(
        const VkImage& image, 
        const VkFormat& format, 
        const VkImageAspectFlags& aspect_flags, 
        const uint32_t& mip_levels,
        const VkDevice& device 
    );

    VkSampler CreateTextureSampler(
        const VkDevice& device, 
        const VkPhysicalDevice& physical_device,
        const uint32_t& mip_levels
    );

    void CreateVkBuffer(
        VkBuffer & out_buffer, 
        VkDeviceMemory & out_buffer_memory,
        const VkDevice& device,
        const VkPhysicalDevice& physical_device,
        VkDeviceSize size, 
        VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties
    );

    void Create(
        WVkSemaphoreInfo & out_semaphore_info,
        const WVkDeviceInfo & in_device_info
        );

    void Create(
        WVkFenceInfo & out_fence_info,
        const WVkDeviceInfo & in_device_info
        );

    // Destroy functions
    // -----------------

    void Destroy(WVkInstanceInfo & instance_info);

    void Destroy(WVkSurfaceInfo & surface_info, const WVkInstanceInfo & instance_info);

    void Destroy(WVkDeviceInfo & device_info);

    void Destroy(WVkSwapChainInfo & swap_chain_info, const WVkDeviceInfo & device_info);

    void DestroyImageView(WVkSwapChainInfo & swap_chain_info, const WVkDeviceInfo & device_info);

    void Destroy(WVkRenderPassInfo & render_pass_info, const WVkDeviceInfo & device_info);

    void Destroy(WVkWindowInfo &window_info);

    void Destroy(WVkRenderPipelineInfo &pipeline_info, const WVkDeviceInfo &device);

    void Destroy(
        WVkDescriptorSetLayoutInfo & descriptor_set_layout_info,
        const WVkDeviceInfo & device
        );

    void Destroy(
        WVkDescriptorPoolInfo & out_descriptor_pool_info,
        const WVkDeviceInfo & in_device
        );

    void Destroy(
        WVkCommandPoolInfo & out_command_pool,
        const WVkDeviceInfo & in_device_info
        );

    void Destroy(
        WVkSemaphoreInfo & out_semaphore_info,
        const WVkDeviceInfo & in_device_info
        );

    void Destroy(
        WVkFenceInfo & out_fence_info,
        const WVkDeviceInfo & in_device_info
        );

    // Record Commands
    // ---------------

    void RecordRenderCommandBuffer(
        VkCommandBuffer in_commandbuffer,
        const WVkRenderPassInfo & in_render_pass_info,
        const WVkSwapChainInfo & in_swap_chain_info,
        const WVkRenderPipelineInfo & in_render_pipeline_info,
        const std::vector<WVkPipelineBindingInfo> & in_bindings,
        uint32_t in_image_index = 0,
        uint32_t in_framebuffer_index = 0
        );

    // Draw
    // ----

    // Swap Chain
    // ----------

    /**
     * @brief Creates Vulkan Image Views.
    */
    void CreateSCImageViews(
        WVkSwapChainInfo & out_swap_chain_info,
        const WVkDeviceInfo &in_device_info
        );

    void CreateSCFramebuffers(
        WVkSwapChainInfo & out_swap_chain_info,
        const WVkRenderPassInfo & out_render_pass_info,
        const WVkDeviceInfo & in_device_info
        );

    void CreateSCColorResources(
        WVkSwapChainInfo & out_swap_chain_info,
        const WVkDeviceInfo & in_device_info
        );

    void CreateSCDepthResources(
        WVkSwapChainInfo & out_swap_chain_info,
        const WVkDeviceInfo & in_device_info
        );

    // Descriptor Set Layout
    // ---------------------

    void AddDSLDefaultBindings(WVkDescriptorSetLayoutInfo & out_descriptor_set_layout);

    /**
     * @brief Update VkWriteDescriptorSet Stop condition
    */
    void UpdateWriteDescriptorSet();

    /**
     * @brief Update a VkWriteDescriptorSet with a UBO struct.
    */
    template<typename ...Args>
    void UpdateWriteDescriptorSet(
        VkWriteDescriptorSet & out_write_descriptor_set,
        WVkWriteDescriptorSetUBOStruct & ubo_struct,
        Args && ... args
    )
    {
        ubo_struct.buffer_info.buffer = ubo_struct.uniform_buffer_info.uniform_buffer;
        ubo_struct.buffer_info.offset = 0;
        ubo_struct.buffer_info.range = sizeof(WVkUBOStruct);
        
        out_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        out_write_descriptor_set.dstSet = ubo_struct.descriptor_set;
        out_write_descriptor_set.dstBinding = ubo_struct.binding;
        out_write_descriptor_set.dstArrayElement = 0;
        out_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        out_write_descriptor_set.descriptorCount = 1;
        out_write_descriptor_set.pBufferInfo = &ubo_struct.buffer_info;

        UpdateWriteDescriptorSet(
            std::forward<Args>(args) ...
        );
    }

    /**
     * @brief Update a VkWriteDescriptorSet with a texture struct.
    */
    template<typename ...Args>
    void UpdateWriteDescriptorSet(
        VkWriteDescriptorSet & out_write_descriptor_set,
        WVkWriteDescriptorSetTextureStruct& texture_struct,
        Args&&... args
    )
    {
        texture_struct.image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        texture_struct.image_info.imageView = texture_struct.texture_info.image_view;
        texture_struct.image_info.sampler = texture_struct.texture_info.sampler;

        out_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        out_write_descriptor_set.dstSet = texture_struct.descriptor_set;
        out_write_descriptor_set.dstBinding = texture_struct.binding;
        out_write_descriptor_set.dstArrayElement = 0;
        out_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        out_write_descriptor_set.descriptorCount = 1;
        out_write_descriptor_set.pImageInfo = &texture_struct.image_info;

        UpdateWriteDescriptorSet(
            std::forward<Args>(args)...
        );
    }

    void UpdateDescriptorSets(
        std::vector<VkWriteDescriptorSet> in_write_descriptor_sets,
        const WVkDeviceInfo & in_device_info
        );

    // void DrawFrame()

    // Helper Functions
    // ----------------

    /**
     * @brief Checks if the requested validation layers are available.
    */
    bool CheckValidationLayerSupport(const WVkRenderDebugInfo &debug_info);

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

    void TransitionImageLayout(
        const VkDevice & device, 
        const VkCommandPool & command_pool, 
        const VkQueue & graphics_queue, 
        const VkImage & image, 
        const VkFormat & format, 
        const VkImageLayout & old_layout, 
        const VkImageLayout & new_layout, 
        const uint32_t & mip_levels
    );

    void CopyBufferToImage(
        VkBuffer in_buffer,
        VkImage in_image,
        uint32_t in_width,
        uint32_t in_height,
        const VkDevice & in_device,
        const VkCommandPool & in_command_pool,
        const VkQueue & in_graphics_queue
        );

    void CopyVkBuffer(
        const VkDevice & device, 
        const VkCommandPool & command_pool, 
        const VkQueue & graphics_queue, 
        const VkBuffer & src_buffer, 
        const VkBuffer & dst_buffer, 
        const VkDeviceSize & size
    );
    
    /**
     * @brief Enum representing the shader stage flags for Vulkan.
     */
    VkShaderStageFlagBits ToShaderStageFlagBits(const EShaderType & type);

    VkCommandBuffer BeginSingleTimeCommands(
        const VkDevice & device, 
        const VkCommandPool & command_pool
    );

    void EndSingleTimeCommands(
        const VkDevice& device, 
        const VkCommandPool& command_pool, 
        const VkQueue& graphics_queue, 
        const VkCommandBuffer& command_buffer
    );

    /**
     * @brief Creates a shader stage from a spir-v shader file in disk.
     */
    WVkShaderStageInfo CreateShaderStageInfo(
	const char* in_shader_file_path,
	const char* int_entry_point,
	EShaderType in_shader_type
	);

    VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice in_physical_device);

    void GenerateMipmaps(
        VkImage in_image,
        VkFormat in_image_format,
        int32_t in_tex_width,
        int32_t in_tex_height,
        int32_t in_mip_levels,
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device,
        const VkCommandPool & in_command_pool,
        const VkQueue & in_graphic_queue
        );


    VkFormat GetImageFormat(uint8_t in_texture_channels );

    WTextureStruct AddRGBAPadding(const WTextureStruct & in_texture_struct);

}
