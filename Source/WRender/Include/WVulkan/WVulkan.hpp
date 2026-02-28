#pragma once

#include "WCore/WCore.hpp"
#include "WLog.hpp"
#include "WVulkan/WVulkanStructs.hpp"

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
    // TODO this file should be more WENG agnostic
    //  try to use only vulkan types.
    //  No WENG dependent functions move to WVulkanUtils.

    // WVulkan Create Structs
    // ---------------
    namespace VkStructs {

        inline constexpr VkApplicationInfo CreateVkApplicationInfo() noexcept {
            VkApplicationInfo result{};
            result.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkInstanceCreateInfo CreateVkInstanceCreateInfo() noexcept {
            VkInstanceCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkDebugUtilsMessengerCreateInfoEXT CreateVkDebugUtilsMessengerCreateInfoEXT() noexcept {
            VkDebugUtilsMessengerCreateInfoEXT result{};
            result.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkSwapchainCreateInfoKHR CreateVkSwapchainCreateInfoKHR() noexcept {
            VkSwapchainCreateInfoKHR result{};
            result.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkWriteDescriptorSet CreateVkWriteDescriptorSet() noexcept {
            VkWriteDescriptorSet result{};
            result.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            return result;
        }

        inline constexpr VkDescriptorImageInfo CreateVkDescriptorImageInfo() noexcept {
            return {};
        }

        inline constexpr VkRenderingInfo CreateVkRenderingInfo()  noexcept {
            VkRenderingInfo result{};
            result.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
            return result;
        }

        inline constexpr VkRenderingAttachmentInfo CreateVkRenderingAttachmentInfo() noexcept {
            VkRenderingAttachmentInfo result {};
            result.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            return result;
        }

        inline constexpr VkDescriptorSetLayoutBinding CreateVkDescriptorSetLayoutBinding() noexcept {
            return {};
        }

        inline constexpr VkPipelineColorBlendAttachmentState CreateVkPipelineColorBlendAttachmentState() noexcept {
            return {};
        }

        inline constexpr VkPipelineVertexInputStateCreateInfo CreateVkPipelineVertexInputStateCreateInfo() noexcept {
            VkPipelineVertexInputStateCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkPipelineInputAssemblyStateCreateInfo CreateVkPipelineInputAssemblyStateCreateInfo() noexcept {
            VkPipelineInputAssemblyStateCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkPipelineViewportStateCreateInfo CreateVkPipelineViewportStateCreateInfo() noexcept {
            VkPipelineViewportStateCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkPipelineRasterizationStateCreateInfo CreateVkPipelineRasterizationStateCreateInfo() noexcept {
            VkPipelineRasterizationStateCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkPipelineMultisampleStateCreateInfo CreateVkPipelineMultisampleStateCreateInfo() noexcept {
            VkPipelineMultisampleStateCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkPipelineDepthStencilStateCreateInfo CreateVkPipelineDepthStencilStateCreateInfo() noexcept {
            VkPipelineDepthStencilStateCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkPipelineColorBlendStateCreateInfo CreateVkPipelineColorBlendStateCreateInfo() noexcept {
            VkPipelineColorBlendStateCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkPipelineDynamicStateCreateInfo CreateVkPipelineDynamicStateCreateInfo() noexcept {
            VkPipelineDynamicStateCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkPipelineLayoutCreateInfo CreateVkPipelineLayoutCreateInfo() noexcept {
            VkPipelineLayoutCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkGraphicsPipelineCreateInfo CreateVkGraphicsPipelineCreateInfo() noexcept {
            VkGraphicsPipelineCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkPipelineRenderingCreateInfo CreateVkPipelineRenderingCreateInfo() noexcept {
            VkPipelineRenderingCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkShaderModuleCreateInfo CreateVkShaderModuleCreateInfo() noexcept {
            VkShaderModuleCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            return result;
        }

        inline constexpr VkPipelineShaderStageCreateInfo CreateVkPipelineShaderStageCreateInfo() noexcept {
            VkPipelineShaderStageCreateInfo result{};
            result.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkSubmitInfo CreateVkSubmitInfo() noexcept {
            VkSubmitInfo result{};
            result.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

        inline constexpr VkPresentInfoKHR CreateVkPresentInfoKHR() noexcept {
            VkPresentInfoKHR result{};
            result.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            result.pNext = VK_NULL_HANDLE;
            return result;
        }

    }

    /**
     * @brief Execute and check if the result is differrent to VK_SUCCESS.
     */
    template<typename TFn, typename ...Args>
    inline void ExecVkProcChecked(TFn && in_fn, std::string_view message, Args && ... args) {
        
        if (std::forward<TFn>(in_fn) (std::forward<Args>(args)...) != VK_SUCCESS) {
            throw std::runtime_error(std::string(message));
            }
    }

    // Create functions
    // ----------------

    // TODO, use pure vulkan classes as inputs
    /**
     * Creates a Vulkan Instance.
     */
    void Create(VkInstance & out_instance,
                bool in_enable_validation_layers,
                const std::vector<std::string_view>& in_validation_layers,
                const PFN_vkDebugUtilsMessengerCallbackEXT & in_debug_callback,
                const VkDebugUtilsMessengerEXT & in_debug_messenger
        );

    /**
     * Creates a Vulkan Surface.
     */
    void Create(VkSurfaceKHR & surface_info, const VkInstance & instance, GLFWwindow * window);

    /**
     * Creates a Vulkan Device.
     */
    void Create(VkDevice & out_device,
                VkPhysicalDevice & out_physical_device,
                VkSampleCountFlagBits & out_max_msaa_samples,
                VkQueue & out_graphics_queue,
                VkQueue & out_present_queue,
                const std::vector<std::string_view> & in_device_extensions,
                const VkInstance & in_instance,
                const VkSurfaceKHR & in_surface,
                bool in_enable_validation_layers,
                const std::vector<std::string_view> & validation_layers);

    /**
     * Creates a Vulkan Swap Chain.
     */
    void Create(
        VkSwapchainKHR & out_swap_chain,
        VkFormat & out_format,
        VkExtent2D & out_extent,
        std::vector<VkImage> & out_images,
        std::vector<VkImageView> & out_views,
        std::vector<VkDeviceMemory> & out_memory,
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device,
        const VkSurfaceKHR & in_surface,
        const std::uint32_t & in_width,
        const std::uint32_t & in_height
        );

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
        WVkDescriptorSetLayoutInfo& out_descriptor_set_layout_info,
        const VkDevice & device
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

    void CreateUBO(
        WVkUBOInfo & out_uniform_buffer_info,
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device
        );

    void MapUBO(
        WVkUBOInfo & out_uniform_buffer_info,
        const VkDevice & in_device
        );

    void UnmapUBO(
        WVkUBOInfo & out_uniform_buffer_info,
        const VkDevice & in_device
        );

    void Create(
        WVkDescriptorPoolInfo & out_descriptor_pool_info,
        const VkDevice & device
        );

    void Create(
        WVkDescriptorSetInfo& out_descriptor_set_info,
        const VkDevice &device,
        const WVkDescriptorSetLayoutInfo& descriptor_set_layout_info,
        const WVkDescriptorPoolInfo& descriptor_pool_info
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

    void Destroy(
        WVkRenderPipelineInfo &pipeline_info,
        const VkDevice & device);

    void Destroy(
        WVkDescriptorSetLayoutInfo & descriptor_set_layout_info,
        const VkDevice & device
        );

    void Destroy(
        WVkDescriptorPoolInfo & out_descriptor_pool_info,
        const VkDevice & in_device
        );

    void Destroy(
        WVkCommandPoolInfo & out_command_pool,
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
        const VkDevice & in_device
        );

    void Destroy(
        VkSampler & out_sampler,
        const WVkDeviceInfo & in_device_info
        );

    template<std::size_t N>
    void DestroyDescPools(std::array<VkDescriptorPool, N> & out_desc_pools,
                          const WVkDeviceInfo & in_device_info) {
        for (std::uint32_t i=0; i<N; i++) {
            if(out_desc_pools[i]) {
                vkDestroyDescriptorPool(
                    in_device_info.vk_device,
                    out_desc_pools[i],
                    nullptr
                    );
            }

            out_desc_pools[i] = VK_NULL_HANDLE;
        }
    }

    // Descriptor Set Layout
    // ---------------------

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
        out_write_descriptor_set.pImageInfo = VK_NULL_HANDLE;
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
        out_write_descriptor_set.pBufferInfo = VK_NULL_HANDLE;
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

    inline bool UpdateUBO(
        WVkUBOInfo & in_ubo_info,
        const void * in_data,
        const std::size_t & in_size,
        const std::size_t & in_offset=0
        ) {

        char * mapped_mem = reinterpret_cast<char*>(in_ubo_info.mapped_memory);
        memcpy((mapped_mem + in_offset),
               in_data,
               in_size               
            );

        return true;
    }

    inline bool UpdateUBOModel(
        WVkUBOInfo & uniform_buffer_object_info_,
        const glm::mat4 & model
        ) {
        WUBOGraphicsStruct ubo{};

        ubo.model = model;
        
        memcpy(uniform_buffer_object_info_.mapped_memory,
               &ubo,
               sizeof(WUBOGraphicsStruct));

        return true;
    }

    inline bool UpdateUBOModel(
        WVkUBOInfo & uniform_buffer_object_info_,
        const WUBOGraphicsStruct & in_ubo_model_struct
        ) {
        
        memcpy(uniform_buffer_object_info_.mapped_memory,
               &in_ubo_model_struct,
               sizeof(WUBOGraphicsStruct));

        return true;
    }

    // Helper Functions TODO: move to WVulkanUtils
    // ----------------

    /**
     * @brief Checks if the requested validation layers are available.
     */
    bool CheckValidationLayerSupport(const std::vector<std::string_view>& debug_info);

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

    bool IsDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, const std::vector<std::string_view>& device_extensions);

    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device, const std::vector<std::string_view> & device_extensions);

    VkFormat FindSupportedFormat(const VkPhysicalDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkFormat FindDepthFormat(const VkPhysicalDevice& device);

    uint32_t FindMemoryType(const VkPhysicalDevice& device, uint32_t type_filter, VkMemoryPropertyFlags properties);

    void TransitionTextureImageLayout(
        const VkDevice & in_device, 
        const VkCommandPool & in_command_pool, 
        const VkQueue & in_graphics_queue, 
        const VkImage & in_image, 
        const VkImageLayout & in_old_layout, 
        const VkImageLayout & in_new_layout, 
        const uint32_t & in_mip_levels
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
