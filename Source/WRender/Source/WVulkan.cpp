#include "WVulkan/WVkUtils/WVulkan.hpp"
#include "WLog.hpp"
#include "WVulkan/WVkUtils/WVkWengUtils.hpp"
#include "WShaderUtils.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WStructs/WTextureStructs.hpp"
#include "WStructs/WGeometryStructs.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WUtils/WStringUtils.hpp"

#include <algorithm>
#include <iostream>
#include <GLFW/glfw3.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <set>
#include <stdexcept>
#include <string>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <vulkan/vulkan_core.h>

// WVulkan
// -------

// Create functions
// ----------------

void WVulkan::CreateTexture(
    WVkTextureInfo &out_texture_info,
    const WTextureStruct &texture_struct,
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device,
    const VkQueue & in_graphics_queue,
    const VkCommandPool &in_command_pool
    )
{
    
    WTextureStruct texture_rgba;
    const WTextureStruct * texture_ptr;

    // Textures must be RGBA, graphic cards prefer RGBA padding.
    if (texture_struct.channels == ETextureChannels::kRGBA) {
        texture_ptr = &texture_struct;
    }
    else
    {
        texture_rgba = AddRGBAPadding(texture_struct);
        texture_ptr = &texture_rgba;
    }

    assert(
        texture_ptr->data.size() ==
        (texture_ptr->width * texture_ptr->height * NumOfChannels(texture_ptr->channels))
        );

    out_texture_info.mip_levels =
        static_cast<uint32_t>(
            std::floor(
                std::log2(
                    std::max(texture_ptr->width, texture_ptr->height)))) + 1;

    VkDeviceSize image_size = texture_ptr->height * texture_ptr->width * 4;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateVkBuffer(
        staging_buffer,
        staging_buffer_memory,
        in_device,
        in_physical_device,
        image_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

    void * data;
    vkMapMemory(in_device, staging_buffer_memory, 0, image_size, 0, &data);
    memcpy(
        data,
        texture_ptr->data.data(),
        std::min(texture_ptr->data.size(), static_cast<size_t>(image_size))
        );
    vkUnmapMemory(in_device, staging_buffer_memory);

    CreateImage(
        out_texture_info.image,
        out_texture_info.memory,
        in_device,
        in_physical_device,
        texture_ptr->width,
        texture_ptr->height,
        out_texture_info.mip_levels,
        VK_SAMPLE_COUNT_1_BIT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

    TransitionTextureImageLayout(
        in_device,
        in_command_pool,
        in_graphics_queue,
        out_texture_info.image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        out_texture_info.mip_levels
        );

    CopyBufferToImage(
        staging_buffer,
        out_texture_info.image,
        texture_ptr->width,
        texture_ptr->height,
        in_device,
        in_command_pool,
        in_graphics_queue
        );

    GenerateMipmaps(
        out_texture_info.image,
        VK_FORMAT_R8G8B8A8_SRGB,
        texture_ptr->width,
        texture_ptr->height,
        out_texture_info.mip_levels,
        in_device,
        in_physical_device,
        in_command_pool,
        in_graphics_queue
        );

    // Image view
    out_texture_info.view = CreateImageView(
        out_texture_info.image,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_ASPECT_COLOR_BIT,
        out_texture_info.mip_levels,
        in_device
        );

    // Sampler
    out_texture_info.sampler = CreateTextureSampler(
        in_device,
        in_physical_device,
        out_texture_info.mip_levels
        );

    vkFreeMemory(in_device, staging_buffer_memory, nullptr);
    vkDestroyBuffer(in_device, staging_buffer, nullptr);
}

void WVulkan::Create(
    WVkDescriptorSetLayoutInfo & out_descriptor_set_layout_info,
    const VkDevice & in_device
    )
{
    VkDescriptorSetLayoutCreateInfo create_info{};

    create_info.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    create_info.bindingCount = static_cast<uint32_t>(
        out_descriptor_set_layout_info.bindings.size()
        );

    create_info.pBindings =
        out_descriptor_set_layout_info.bindings.data();

    if (vkCreateDescriptorSetLayout(
            in_device,
            &create_info,
            nullptr,
            &out_descriptor_set_layout_info.descset_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}

void WVulkan::CreateImage(
    VkImage &out_image,
    VkDeviceMemory &out_image_memory,
    const VkDevice &device,
    const VkPhysicalDevice &physical_device,
    const uint32_t &width, const uint32_t &height,
    const uint32_t &mip_levels,
    const VkSampleCountFlagBits &samples,
    const VkFormat &format,
    const VkImageTiling &tiling,
    const VkImageUsageFlags &usage,
    const VkMemoryPropertyFlags &properties
    )
{
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = mip_levels;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.samples = samples;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device, &image_info, nullptr, &out_image) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create image!");
    }

    VkMemoryRequirements mem_requirements;

    vkGetImageMemoryRequirements(device, out_image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = FindMemoryType(
        physical_device,
        mem_requirements.memoryTypeBits,
        properties);

    if (vkAllocateMemory(device, &alloc_info, nullptr, &out_image_memory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate image memory!");
    }

    vkBindImageMemory(device, out_image, out_image_memory, 0);
}

void WVulkan::CreateMeshBuffers(
    WVkMeshInfo & out_mesh_info,
    const void * vertex_buffer,
    const std::uint32_t & vertex_buffer_size,
    const void * index_buffer,
    const std::uint32_t & index_buffer_size,
    const std::uint32_t & index_count,
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device,
    const VkQueue & in_graphics_queue,
    const VkCommandPool & in_command_pool
    )
{
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    // vertex buffer

    VkDeviceSize buffer_size = vertex_buffer_size;

    CreateVkBuffer(
        staging_buffer,
        staging_buffer_memory,
        in_device,
        in_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

    void * data;
    vkMapMemory(in_device, staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, vertex_buffer, static_cast<size_t>(buffer_size));
    vkUnmapMemory(in_device, staging_buffer_memory);

    CreateVkBuffer(
        out_mesh_info.vertex_buffer,
        out_mesh_info.vertex_buffer_memory,
        in_device,
        in_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

    WVkWengUtils::CopyVkBuffer(
        in_device,
        in_command_pool,
        in_graphics_queue,
        staging_buffer,
        out_mesh_info.vertex_buffer,
        buffer_size);

    vkDestroyBuffer(in_device, staging_buffer, nullptr);
    vkFreeMemory(in_device, staging_buffer_memory, nullptr);

    // index buffer

    CreateVkBuffer(
        staging_buffer,
        staging_buffer_memory,
        in_device,
        in_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

    vkMapMemory(in_device, staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, index_buffer, static_cast<size_t>(buffer_size));
    vkUnmapMemory(in_device, staging_buffer_memory);

    CreateVkBuffer(
        out_mesh_info.index_buffer,
        out_mesh_info.index_buffer_memory,
        in_device,
        in_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT, // |
          // VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

    WVkWengUtils::CopyVkBuffer(
        in_device,
        in_command_pool,
        in_graphics_queue,
        staging_buffer,
        out_mesh_info.index_buffer,
        buffer_size);

    vkDestroyBuffer(in_device, staging_buffer, nullptr);
    vkFreeMemory(in_device, staging_buffer_memory, nullptr);

    out_mesh_info.index_count = index_count;
}

void WVulkan::CreateUBO(
    WVkUBOInfo & out_ubo_info,
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device)
{
    CreateVkBuffer(
        out_ubo_info.buffer,
        out_ubo_info.buffer_memory,
        in_device,
        in_physical_device,
        out_ubo_info.range,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
}

void WVulkan::MapUBO(
    WVkUBOInfo & out_uniform_buffer_info,
    const VkDevice & in_device
    )
{
    vkMapMemory(
        in_device,
        out_uniform_buffer_info.buffer_memory,
        0,
        out_uniform_buffer_info.range,
        0,
        &out_uniform_buffer_info.mapped_memory
        );
}

void WVulkan::UnmapUBO(
    WVkUBOInfo & out_uniform_buffer_info,
    const VkDevice & in_device
    ) {
    vkUnmapMemory(
        in_device,
        out_uniform_buffer_info.buffer_memory
        );

    out_uniform_buffer_info.mapped_memory = nullptr;
}

void WVulkan::Create(
    WVkDescriptorPoolInfo & out_descriptor_pool_info,
    const VkDevice & in_device)
{
    std::array<VkDescriptorPoolSize,2> pool_sizes;

    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount =
        static_cast<uint32_t>(WENG_MAX_FRAMES_IN_FLIGHT) * 20;  // 40 descriptors in this pool
    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[1].descriptorCount =
        static_cast<uint32_t>(WENG_MAX_FRAMES_IN_FLIGHT) * 20;  // 40 descriptors in this pool

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(
        pool_sizes.size()
        );
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = static_cast<uint32_t>(WENG_MAX_FRAMES_IN_FLIGHT * 35); //70 as max sets

    if (vkCreateDescriptorPool(
            in_device,
            &pool_info,
            nullptr,
            &out_descriptor_pool_info.descriptor_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

void WVulkan::Create(
    WVkDescriptorSetInfo & out_descriptor_set_info,
    const VkDevice & in_device,
    const WVkDescriptorSetLayoutInfo & descriptor_set_layout_info,
    const WVkDescriptorPoolInfo & descriptor_pool_info
    )
{
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool_info.descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &descriptor_set_layout_info.descset_layout;

    if (vkAllocateDescriptorSets(
            in_device,
            &alloc_info,
            &out_descriptor_set_info.descriptor_set
            ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }
}

// void WVulkan::UpdateDescriptorSets(
//     const std::vector<VkWriteDescriptorSet> & in_write_descriptor_sets,
//     const WVkDeviceInfo & in_device_info
//     )
// {
//     vkUpdateDescriptorSets(
//         in_device_info.vk_device,
//         static_cast<std::uint32_t>(in_write_descriptor_sets.size()),
//         in_write_descriptor_sets.data(),
//         0,
//         nullptr
//         );
// }

// void WVulkan::Create(
//     WVkCommandBufferInfo & out_command_buffer_info,
//     const WVkDeviceInfo & device,
//     const WVkCommandPoolInfo & command_pool_info
//     )
// {
//     VkCommandBufferAllocateInfo alloc_info{};
//     alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//     alloc_info.commandPool = command_pool_info.vk_command_pool;
//     alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//     alloc_info.commandBufferCount = static_cast<uint32_t>(
//         out_command_buffer_info.command_buffers.size()
//         );

//     if (vkAllocateCommandBuffers(
//             device.vk_device,
//             &alloc_info,
//             out_command_buffer_info.command_buffers.data()) != VK_SUCCESS)
//     {
//         throw std::runtime_error("Failed to allocate command buffers!");
//     }
// }

// Destroy functions
// -----------------

void WVulkan::Destroy(
    WVkRenderPipelineInfo & pipeline_info,
    const VkDevice & in_device
    )
{
    if (pipeline_info.pipeline_layout)
    {
        // destroy pipeline layout
        vkDestroyPipelineLayout(
            in_device,
            pipeline_info.pipeline_layout,
            nullptr);

	pipeline_info.pipeline_layout = VK_NULL_HANDLE;
    }

    if (pipeline_info.pipeline)
    {
        // destroy pipeline
        vkDestroyPipeline(
            in_device,
            pipeline_info.pipeline,
            nullptr
	    );

	pipeline_info.pipeline = VK_NULL_HANDLE;
    }
}

void WVulkan::Destroy(
    WVkDescriptorSetLayoutInfo & descriptor_set_layout_info,
    const VkDevice & in_device
    )
{
    // destroy descriptor set layout
    vkDestroyDescriptorSetLayout(
        in_device,
        descriptor_set_layout_info.descset_layout,
        nullptr
        );
}

void WVulkan::Destroy(
    WVkDescriptorPoolInfo & out_descriptor_pool_info,
    const VkDevice & in_device
    )
{
    vkDestroyDescriptorPool(
        in_device,
        out_descriptor_pool_info.descriptor_pool,
        nullptr
        );

    out_descriptor_pool_info.descriptor_pool = VK_NULL_HANDLE;
}

void WVulkan::Destroy(
    WVkTextureInfo & in_texture_info,
    const VkDevice & in_device
    ) {
    
    vkDestroySampler(in_device,
                     in_texture_info.sampler,
                     nullptr);

    in_texture_info.sampler = VK_NULL_HANDLE;

    vkDestroyImageView(in_device,
                       in_texture_info.view,
                       nullptr);

    in_texture_info.view = VK_NULL_HANDLE;

    vkDestroyImage(in_device,
                   in_texture_info.image,
                   nullptr);

    in_texture_info.image = VK_NULL_HANDLE;

    vkFreeMemory(in_device,
                 in_texture_info.memory,
                 nullptr);

    in_texture_info.memory = VK_NULL_HANDLE;
    
}

void WVulkan::Destroy(
    WVkMeshInfo & out_mesh_info,
    const VkDevice & in_device
    ) {
    
    vkDestroyBuffer(in_device,
                    out_mesh_info.index_buffer,
                    nullptr);
    
    vkFreeMemory(in_device,
                 out_mesh_info.index_buffer_memory,
                 nullptr);

    vkDestroyBuffer(in_device,
                    out_mesh_info.vertex_buffer,
                    nullptr);
    
    vkFreeMemory(in_device,
                 out_mesh_info.vertex_buffer_memory,
                 nullptr);

}

void WVulkan::Destroy(
    WVkUBOInfo & out_ubo_info,
    const VkDevice & in_device
    ) {

    if (out_ubo_info.mapped_memory) {
        vkUnmapMemory(in_device, out_ubo_info.buffer_memory);
        out_ubo_info.mapped_memory = nullptr;
    }
    
    vkDestroyBuffer(
        in_device,
        out_ubo_info.buffer,
        nullptr
        );

    vkFreeMemory(
        in_device,
        out_ubo_info.buffer_memory,
        nullptr
        );

    out_ubo_info.buffer = VK_NULL_HANDLE;
    out_ubo_info.buffer_memory = VK_NULL_HANDLE;
    out_ubo_info.range = 0;
}

void WVulkan::Destroy(
    VkSampler & out_sampler,
    const VkDevice & in_device
    ) {
    vkDestroySampler(in_device, out_sampler, nullptr);
    out_sampler = VK_NULL_HANDLE;
}

// Descriptor Set Layout
// ---------------------

// Helper functions
// ----------------

bool WVulkan::CheckValidationLayerSupport(
    const std::vector<std::string_view>& in_validation_layers
    )
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (auto & layer_name : in_validation_layers)
    {
        bool layer_found = false;

        for (const auto &layer_properties : available_layers)
        {
            if (layer_name.compare(layer_properties.layerName) == 0)
            {
                layer_found = true;
                break;
            }
        }

        if (!layer_found)
        {
            return false;
        }
    }

    return true;
}

std::vector<const char *> WVulkan::GetRequiredExtensions(bool enable_validation_layers)
{
    uint32_t glfw_extension_count = 0;
    const char ** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    // iterator first last vector initialization
    std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    if (enable_validation_layers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL WVulkan::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT InMessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT InMessageType,
    const VkDebugUtilsMessengerCallbackDataEXT *InCallbackData,
    void *InUserData)
{
    std::cerr << "Validation layer: " << InCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

bool WVulkan::IsDeviceSuitable(const VkPhysicalDevice & device,
                               const VkSurfaceKHR & surface,
                               const std::vector<std::string_view> & device_extensions)
{
    QueueFamilyIndices indices = FindQueueFamilies(device, surface);

    bool extensions_supported = CheckDeviceExtensionSupport(device,
                                                            device_extensions);

    bool swap_chain_adequate = false;
    if (extensions_supported)
    {
        SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(device, surface);
        swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
    }

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(device, &supported_features);

    return indices.IsComplete() && extensions_supported && swap_chain_adequate && supported_features.samplerAnisotropy;
}

bool WVulkan::CheckDeviceExtensionSupport(const VkPhysicalDevice & device,
                                          const std::vector<std::string_view> & device_extensions)
{
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(device_extensions.begin(),
                                              device_extensions.end());

    for (const auto &extension : available_extensions)
    {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

WVulkan::QueueFamilyIndices WVulkan::FindQueueFamilies(const VkPhysicalDevice &device, const VkSurfaceKHR &surface)
{
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    int i = 0;
    for (const auto &queue_family : queue_families)
    {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphics_family = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if (present_support)
        {
            indices.present_family = i;
        }

        if (indices.IsComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

WVulkan::SwapChainSupportDetails WVulkan::QuerySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
    if (format_count != 0)
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
    if (present_mode_count != 0)
    {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
    }

    return details;
}

VkSurfaceFormatKHR WVulkan::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats)
{
    for (const auto &available_format : available_formats)
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return available_format;
        }
    }
    return available_formats[0];
}

VkPresentModeKHR WVulkan::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> & available_present_modes)
{
    for (const auto &available_present_mode : available_present_modes)
    {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return available_present_mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D WVulkan::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & in_capabilities,
                                     const std::uint32_t & in_width,
                                     const std::uint32_t & in_height)
{
    if (in_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return in_capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actual_extent = {
            in_width,
            in_height
        };

        actual_extent.width = std::clamp(
            actual_extent.width,
            in_capabilities.minImageExtent.width,
            in_capabilities.maxImageExtent.width
            );
        actual_extent.height = std::clamp(
            actual_extent.height,
            in_capabilities.minImageExtent.height,
            in_capabilities.maxImageExtent.height
            );

        return actual_extent;
    }
}

VkImageView WVulkan::CreateImageView(
    const VkImage & in_image,
    const VkFormat & in_format,
    const VkImageAspectFlags & in_aspect_flags,
    const uint32_t & in_mip_levels,
    const VkDevice & in_device
    )
{
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = in_image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = in_format;
    view_info.subresourceRange.aspectMask = in_aspect_flags;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = in_mip_levels;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    VkImageView image_view;
    if (vkCreateImageView(in_device, &view_info, nullptr, &image_view) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create texture image view!");
    }

    return image_view;
}

void WVulkan::CreateVkBuffer(
    VkBuffer &out_buffer,
    VkDeviceMemory &out_buffer_memory,
    const VkDevice &device,
    const VkPhysicalDevice &physical_device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
    )
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &buffer_info, nullptr, &out_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create buffer!");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device, out_buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = FindMemoryType(
        physical_device,
        mem_requirements.memoryTypeBits,
        properties);

    if (vkAllocateMemory(device, &alloc_info, nullptr, &out_buffer_memory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, out_buffer, out_buffer_memory, 0);

    // WFLOG("Create new Buffer {}", static_cast<void*>(out_buffer));
}

VkFormat WVulkan::FindSupportedFormat(
    const VkPhysicalDevice & device,
    const std::vector<VkFormat> & candidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags features
    )
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }
    throw std::runtime_error("Failed to find supported format!");
}

VkFormat WVulkan::FindDepthFormat(const VkPhysicalDevice & device)
{
    return FindSupportedFormat(
        device,
        {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT, // Stencil
            VK_FORMAT_D24_UNORM_S8_UINT   // Stencil
        },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

uint32_t WVulkan::FindMemoryType(const VkPhysicalDevice &device, uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(device, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
    {
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}

void WVulkan::TransitionTextureImageLayout(
    const VkDevice &device,
    const VkCommandPool &command_pool,
    const VkQueue &graphics_queue,
    const VkImage &image,
    const VkImageLayout &old_layout,
    const VkImageLayout &new_layout,
    const uint32_t &mip_levels
    )
{
    VkCommandBuffer command_buffer = BeginSingleTimeCommands(device, command_pool);

    VkImageMemoryBarrier barrier{};  // old stype barrier
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mip_levels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    if (
        old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        )
    {
        WFLOG("From VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTINAL");
        
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (
        old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
        new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        )
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        command_buffer,
        source_stage,
        destination_stage,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier);

    EndSingleTimeCommands(
        device,
        command_pool,
        graphics_queue,
        command_buffer);
}

VkCommandBuffer WVulkan::BeginSingleTimeCommands(const VkDevice &device, const VkCommandPool &command_pool)
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);
    return command_buffer;
}

void WVulkan::EndSingleTimeCommands(
    const VkDevice &device,
    const VkCommandPool &command_pool,
    const VkQueue &graphics_queue,
    const VkCommandBuffer &command_buffer)
{
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue);

    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

VkSampler WVulkan::CreateTextureSampler(
    const VkDevice &device,
    const VkPhysicalDevice &physical_device,
    const uint32_t &mip_levels)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physical_device, &properties);

    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.minLod = 0.f;
    sampler_info.maxLod = static_cast<float>(mip_levels);
    sampler_info.mipLodBias = 0.f;

    VkSampler texture_sampler;
    if (vkCreateSampler(device, &sampler_info, nullptr, &texture_sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create texture sampler!");
    }

    return texture_sampler;
}

void WVulkan::CopyBufferToImage(
    VkBuffer in_buffer,
    VkImage in_image,
    uint32_t in_width,
    uint32_t in_height,
    const VkDevice & in_device,
    const VkCommandPool & in_command_pool,
    const VkQueue & in_graphics_queue
    )
{
    VkCommandBuffer command_buffer = BeginSingleTimeCommands(
        in_device,
        in_command_pool
        );

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0,0,0};
    region.imageExtent = {
        in_width,
        in_height,
        1
    };

    vkCmdCopyBufferToImage(
        command_buffer, 
        in_buffer, 
        in_image, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
        1, 
        &region
        );

    EndSingleTimeCommands(
        in_device,
        in_command_pool,
        in_graphics_queue,
        command_buffer
        );
}

VkSampleCountFlagBits WVulkan::GetMaxUsableSampleCount(VkPhysicalDevice in_physical_device)
{
    VkPhysicalDeviceProperties PhysicalDeviceProperties;
    vkGetPhysicalDeviceProperties(
        in_physical_device, 
        &PhysicalDeviceProperties
    );

    VkSampleCountFlags Counts = 
        PhysicalDeviceProperties.limits.framebufferColorSampleCounts &
        PhysicalDeviceProperties.limits.framebufferDepthSampleCounts;

    if (Counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (Counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (Counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }

    if (Counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (Counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (Counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;

}

void WVulkan::GenerateMipmaps(
    VkImage in_image,
    VkFormat in_image_format,
    int32_t in_tex_width,
    int32_t in_tex_height,
    int32_t in_mip_levels,
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device,
    const VkCommandPool & in_command_pool,
    const VkQueue & in_graphic_queue
    )
{
    // Mip pregeneration should be pregenerated
    VkFormatProperties format_properties;
    
    vkGetPhysicalDeviceFormatProperties(
        in_physical_device,
        in_image_format,
        &format_properties
        );

    if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("Texture image format does not support linear blitting!");
    }

    VkCommandBuffer command_buffer = BeginSingleTimeCommands(in_device, in_command_pool);

    VkImageMemoryBarrier barrier={};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = in_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;
    
    int32_t mip_width = in_tex_width;
    int32_t mip_height = in_tex_height;

    for (uint32_t i=1; i < in_mip_levels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            {},
            0, nullptr,
            0, nullptr,
            1, &barrier
            );

        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mip_width, mip_height, 1};
        blit.dstOffsets[0] = {0,0,0};
        blit.dstOffsets[1] = {
            mip_width > 1 ? mip_width / 2 : 1,
            mip_height > 1 ? mip_height / 2 : 1,
            1
        };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(
            command_buffer,
            in_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            in_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR
            );

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
            );

        if (mip_width > 1) mip_width /= 2;
        if (mip_height > 1) mip_height /= 2;
    }

    barrier.subresourceRange.baseMipLevel = in_mip_levels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        command_buffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
        );

    EndSingleTimeCommands(in_device, in_command_pool, in_graphic_queue, command_buffer);

}

VkFormat WVulkan::GetImageFormat(uint8_t in_texture_channels)
{
    ETextureChannels value = static_cast<ETextureChannels>(in_texture_channels);
    
    switch(value) {
    case ETextureChannels::kR:
        return VK_FORMAT_R8_SRGB;
    case ETextureChannels::kRG:
        return VK_FORMAT_R8G8_SRGB;
    case ETextureChannels::kRGB:
        return VK_FORMAT_R8G8B8_SRGB;
    case ETextureChannels::kRGBA:
        return VK_FORMAT_R8G8B8A8_SRGB;
    default:
        return VK_FORMAT_R8G8B8A8_SRGB;
    }
}

WTextureStruct WVulkan::AddRGBAPadding(const WTextureStruct & in_texture)
{
    WTextureStruct result;
    
    result.channels = ETextureChannels::kRGBA;
    result.height = in_texture.height;
    result.width = in_texture.width;

    result.data = in_texture.data;

    size_t tex_size = result.height * result.width;

    result.data.resize(result.height * result.width * 4, 255);

    int num_channels = NumOfChannels(in_texture.channels);

    for (int i=num_channels; i < 4; i++) {
        std::memcpy(
            result.data.data() + (tex_size * i),
            result.data.data(),
            tex_size
            );
    }

    return result;
}
