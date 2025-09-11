#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVkRenderPipeline.hpp"
#include "WLog.hpp"

#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <vulkan/vulkan_core.h>
#include <optional>

struct WTextureStruct;
struct WMeshStruct;
struct GLFWwindow;

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
    void Create(WVkSurfaceInfo & surface_info, WVkInstanceInfo & instance, GLFWwindow * window);

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
        const std::uint32_t & in_width,
        const std::uint32_t & in_height,
        const WVkRenderDebugInfo &debug_info
        );

    /**
     * @brief Creates offscreen Render Pass.
    */
    void CreateOffscreenRenderPass(WVkOffscreenRenderStruct & out_render_pass,
                                   const VkFormat & in_swap_chain_image_format,
                                   const WVkDeviceInfo& in_device_info);

    /** @brief Create Postprocess render pass. */
    void CreatePostprocessRenderPass(WVkPostprocessRenderStruct & out_render_pass,
                                     const VkFormat & in_swap_chain_image_format,
                                     const WVkDeviceInfo & in_device_info);

    /**
     * @brief Creates a Vulkan Command Pool.
    */
    void Create(
        WVkCommandPoolInfo & command_pool_info,
        const WVkDeviceInfo & device_info,
        const WVkSurfaceInfo & surface_info
	);

    void CreateTexture(
        WVkTextureInfo& out_texture_info, 
        const WTextureStruct& texture_struct,
        const WVkDeviceInfo& device_info,
        const WVkCommandPoolInfo& command_pool_info
    );

    void Create(
        WVkRenderPipelineInfo & out_pipeline_info,
        const WVkDeviceInfo & device,
        const std::vector<WVkDescriptorSetLayoutInfo> & descriptor_set_layout_infos,
        const std::vector<WVkShaderStageInfo> & in_shader_stage_infos
        );

    void Create(
        WVkDescriptorSetLayoutInfo& out_descriptor_set_layout_info,
        const WVkDeviceInfo &device
        );

    /**
     * @brief Create a vulkan  mesh
     */
    void CreateMeshBuffers(
        WVkMeshInfo & out_mesh_info,
        const void * vertex_buffer,
        const std::uint32_t & vertes_buffer_size,
        const void * index_buffer,
        const std::uint32_t & index_buffer_size,
        const std::uint32_t & index_count,
        const WVkDeviceInfo & device,
        const WVkCommandPoolInfo & command_pool_info
    );

    void Create(
        WVkUBOInfo & out_uniform_buffer_info,
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
        const WVkDescriptorPoolInfo& descriptor_pool_info
        // ,
        // const std::vector<VkWriteDescriptorSet>& write_descriptor_sets
    );
 
    // ----------------
 
    struct WVkWriteDescriptorSetUBOStruct
    {
        uint32_t binding;
        WVkUBOInfo uniform_buffer_info;
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

    void CreateImage(
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

    void Destroy(WVkSurfaceInfo & surface_info,
                 const WVkInstanceInfo & instance_info);

    void Destroy(WVkDeviceInfo & device_info);

    void Destroy(WVkSwapChainInfo & swap_chain_info,
                 const WVkDeviceInfo & device_info);

    void DestroyImageView(WVkSwapChainInfo & swap_chain_info,
                          const WVkDeviceInfo & device_info);

    // void Destroy(WVkOffscreenRenderStruct & render_pass_info,
    //              const WVkDeviceInfo & device_info);

    void Destroy(
        WVkRenderPipelineInfo &pipeline_info,
        const WVkDeviceInfo &device);

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

    void Destroy(
        WVkTextureInfo & out_texture_info,
        const WVkDeviceInfo & in_device_info
        );

    void Destroy(
        WVkMeshInfo & out_mesh_info,
        const WVkDeviceInfo & in_device_info
        );

    void Destroy(
        WVkUBOInfo & out_ubo_info,
        const WVkDeviceInfo & in_device_info
        );

    void CreateRenderColorResource(
        VkImage & out_image,
        VkDeviceMemory & out_memory,
        VkImageView & out_image_view,
        const VkFormat & in_color_format,
        const WVkDeviceInfo & in_device_info,
        const VkExtent2D & in_extent
        );

    void CreateRenderDepthResource(
        VkImage & out_image,
        VkDeviceMemory & out_memory,
        VkImageView & out_image_view,
        const WVkDeviceInfo & in_device_info,
        const VkExtent2D & in_extent
        );

    // void CreateOffscreenFramebuffer(
    //     WVkOffscreenRenderStruct & offscreen_render_pass,
    //     const WVkDeviceInfo & in_device_info
    //     );
    
    // void CreatePostprocessFramebuffer(
    //     WVkPostprocessRenderStruct  & framebuffer_render_pass,
    //     const WVkDeviceInfo & in_device_info
    //     );

    // void CreateOffcreenRenderFrameBuffers_DEPRECATED(
    //     WVkSwapChainInfo & out_swap_chain_info,
    //     const WVkOffscreenRenderStruct & out_render_pass_info,
    //     const WVkDeviceInfo & in_device_info
    //     );

    // Descriptor Set Layout
    // ---------------------

    /**
     * @brief Adds the Camera and lightings UBOs (std140, set=0)
     */
    void AddDSL_DefaultGlobalGraphicBindings(WVkDescriptorSetLayoutInfo & out_descriptor_set_layout);

    /**
     * @brief Adds common graphic pipeline bindings, should be used in set=1
     */
    void AddDSL_DefaultGraphicBindings(WVkDescriptorSetLayoutInfo & out_descriptor_set_layout);

    constexpr void UpdateWriteDescriptorSet_UBO(
        VkWriteDescriptorSet & out_write_descriptor_set,
        const uint32_t & in_binding,
        const VkDescriptorBufferInfo & in_buffer_info,
        const VkDescriptorSet & dst_set
        )
    {
        out_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        out_write_descriptor_set.dstBinding = in_binding;
        out_write_descriptor_set.dstSet = dst_set;
        out_write_descriptor_set.dstArrayElement = 0;
        out_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        out_write_descriptor_set.descriptorCount = 1;
        out_write_descriptor_set.pBufferInfo = &in_buffer_info;
        out_write_descriptor_set.pNext = VK_NULL_HANDLE;
    }

    constexpr void UpdateWriteDescriptorSet_Texture(
        VkWriteDescriptorSet & out_write_descriptor_set,
        const uint32_t & in_binding,
        const VkDescriptorImageInfo & in_image_info,
        const VkDescriptorSet & dst_set
        )
    {
        out_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        out_write_descriptor_set.dstBinding = in_binding;
        out_write_descriptor_set.dstSet = dst_set;
        out_write_descriptor_set.dstArrayElement = 0;
        out_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        out_write_descriptor_set.descriptorCount = 1;
        out_write_descriptor_set.pImageInfo = &in_image_info;
        out_write_descriptor_set.pNext = VK_NULL_HANDLE;
    }

    void UpdateDescriptorSets(
        const std::vector<VkWriteDescriptorSet> & in_write_descriptor_sets,
        const WVkDeviceInfo & in_device_info
        );

    template<size_t N>
    void UpdateDescriptorSets(
        const std::array<VkWriteDescriptorSet, N> & in_write_descriptor_sets,
        const WVkDeviceInfo & in_device_info
        ) {
        vkUpdateDescriptorSets(
            in_device_info.vk_device,
            static_cast<uint32_t>(N),
            in_write_descriptor_sets.data(),
            0,
            nullptr
            );
    }

    inline bool UpdateUBOModel(
        WVkUBOInfo & uniform_buffer_object_info_,
        const glm::mat4 & model
        ) {
        WUBOModelStruct ubo{};

        ubo.model = model;
        
        memcpy(uniform_buffer_object_info_.mapped_data,
               &ubo,
               sizeof(WUBOModelStruct));

        return true;
    }

    inline bool UpdateUBOModel(
        WVkUBOInfo & uniform_buffer_object_info_,
        const WUBOModelStruct & in_ubo_model_struct
        ) {
        memcpy(uniform_buffer_object_info_.mapped_data,
               &in_ubo_model_struct,
               sizeof(WUBOModelStruct));

        return true;
    }

    // Helper Functions TODO: move to WVulkanUtils
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

    VkExtent2D ChooseSwapExtent(
        const VkSurfaceCapabilitiesKHR &capabilities,
        const std::uint32_t & in_width,
        const std::uint32_t & in_height
        );

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

    void TransitionImageLayout1(
        const VkDevice & device, 
        const VkCommandPool & command_pool, 
        const VkQueue & graphics_queue, 
        const VkImage & image, 
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
