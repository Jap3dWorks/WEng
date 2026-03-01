#include "WVulkan/WVulkan.hpp"
#include "WLog.hpp"
#include "WVulkan/WVulkanUtils.hpp"
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

// void WVulkan::Create(VkInstance & out_instance,
//                      bool in_enable_validation_layers,
//                      const std::vector<std::string_view>& in_validation_layers,
//                      const PFN_vkDebugUtilsMessengerCallbackEXT & in_debug_callback,
//                      const VkDebugUtilsMessengerEXT & in_debug_messenger
//     )
// {
//     if (in_enable_validation_layers && !CheckValidationLayerSupport(in_validation_layers))
//     {
//         throw std::runtime_error("Validation layers requested, but not available!");
//     }

//     VkApplicationInfo app_info =
//         VkStructs::CreateVkApplicationInfo();
//     app_info.pApplicationName = "WEngine";
//     app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//     app_info.pEngineName = "WEngine";
//     app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//     app_info.apiVersion = VK_API_VERSION_1_3;

//     VkInstanceCreateInfo create_info =
//         VkStructs::CreateVkInstanceCreateInfo();
//     create_info.pApplicationInfo = &app_info;

//     auto extensions = GetRequiredExtensions(in_enable_validation_layers);
    
//     create_info.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
//     create_info.ppEnabledExtensionNames = extensions.data();

//     VkDebugUtilsMessengerCreateInfoEXT debug_create_info =
//         VkStructs::CreateVkDebugUtilsMessengerCreateInfoEXT();

//     std::vector<const char*> enabled_layers_names{};
//     if (in_enable_validation_layers)
//     {
//         create_info.enabledLayerCount = static_cast<std::uint32_t>(in_validation_layers.size());
        
//         WStringUtils::ToConstCharPtrs(in_validation_layers, enabled_layers_names);

//         create_info.ppEnabledLayerNames = enabled_layers_names.data();

//         debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
//                                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
//                                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

//         debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT      |
//                                         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT   |
//                                         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
//         if (in_debug_callback)
//         {
//             debug_create_info.pfnUserCallback = in_debug_callback;
//         }
//         else
//         {
//             debug_create_info.pfnUserCallback = &DebugCallback;
//         }

//         debug_create_info.flags = 0;

//         create_info.pNext = &debug_create_info;
//     }
//     else
//     {
//         create_info.enabledLayerCount = 0;
//     }

//     ExecVkProcChecked(
//         vkCreateInstance,
//         "Failed to create instance!",
//         &create_info,
//         nullptr,
//         &out_instance
//         );
// }

// void WVulkan::Create(VkSurfaceKHR & surface,
//                      const VkInstance & in_instance,
//                      GLFWwindow * in_window)
// {
//     VkResult result =
//         glfwCreateWindowSurface(
//             in_instance,
//             in_window,
//             nullptr,
//             &surface
//             );
    
//     if (result != VK_SUCCESS)
//     {
//         throw std::runtime_error("Failed to create window surface!");
//     }
// }

void WVulkan::Create(
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
    const std::uint32_t & in_height // ,
    )
{
    SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(
        in_physical_device,
        in_surface
        );

    VkSurfaceFormatKHR surface_format =
        ChooseSwapSurfaceFormat(swap_chain_support.formats);
    VkPresentModeKHR present_mode =
        ChooseSwapPresentMode(swap_chain_support.present_modes);
    VkExtent2D extent =
        ChooseSwapExtent(swap_chain_support.capabilities,
                         in_width,
                         in_height);

    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
    
    if (swap_chain_support.capabilities.maxImageCount > 0 &&
        image_count > swap_chain_support.capabilities.maxImageCount)
    {
        image_count = swap_chain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info =
        WVulkan::VkStructs::CreateVkSwapchainCreateInfoKHR();
    
    create_info.surface = in_surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent; // Swap chain image resolution
    create_info.imageArrayLayers = 1;
    // VK_IMAGE_USAGE_TRANSFER_DST_BIT for post-processing
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices =
        FindQueueFamilies(in_physical_device, in_surface);

    uint32_t queue_family_indices[] = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };

    if (indices.graphics_family != indices.present_family)
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    create_info.preTransform = swap_chain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    ExecVkProcChecked(vkCreateSwapchainKHR,
                      "Failed to create swap chain!",
                      in_device,
                      &create_info,
                      nullptr,
                      &out_swap_chain);

    // Retrieve Swap Chain Images
    vkGetSwapchainImagesKHR(in_device,
                            out_swap_chain,
                            &image_count,
                            nullptr);
    out_images.resize(image_count);
    vkGetSwapchainImagesKHR(
        in_device,
        out_swap_chain,
        &image_count,
        out_images.data());

    // Save Swap Chain Image Format
    out_format = surface_format.format;
    out_extent = extent;
    // out_swap_chain_info.image_count=image_count;

    // Swap chain image views
    out_views.resize(
        out_images.size()
        );

    for (size_t i = 0; i < out_images.size(); i++)
    {
        out_views[i] = CreateImageView(out_images[i],
                                       out_format,
                                       VK_IMAGE_ASPECT_COLOR_BIT,
                                       1,
                                       in_device);
    }

}

// void WVulkan::Create(
//     WVkCommandPoolInfo & command_pool_info,
//     const WVkDeviceInfo & device_info,
//     const WVkSurfaceInfo & surface_info
//     )
// {
//     QueueFamilyIndices queue_family_indices =
// 	FindQueueFamilies(device_info.vk_physical_device, surface_info.surface);

//     VkCommandPoolCreateInfo pool_info{};

//     pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//     pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
//     pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

//     if (
// 	vkCreateCommandPool(
// 	    device_info.vk_device,
// 	    &pool_info,
// 	    nullptr,
// 	    &command_pool_info.vk_command_pool) != VK_SUCCESS
// 	)
//     {
//         throw std::runtime_error("Failed to create command pool!");
//     }
// }

// void WVulkan::Create(VkDevice & out_device,
//                      VkPhysicalDevice & out_physical_device,
//                      VkSampleCountFlagBits & out_max_msaa_samples,
//                      VkQueue & out_graphics_queue,
//                      VkQueue & out_present_queue,
//                      const std::vector<std::string_view> & in_device_extensions,
//                      const VkInstance & in_instance,
//                      const VkSurfaceKHR & in_surface,
//                      bool in_enable_validation_layers,
//                      const std::vector<std::string_view>& in_validation_layers)
// {
//     // Pick Physical Device
    
//     uint32_t device_count = 0;
//     vkEnumeratePhysicalDevices(in_instance, &device_count, nullptr);
//     if (device_count == 0)
//     {
//         throw std::runtime_error("Failed to find GPUs with Vulkan support!");
//     }
    
//     std::vector<VkPhysicalDevice> devices(device_count);
//     vkEnumeratePhysicalDevices(in_instance, &device_count, devices.data());

//     for (const auto &device : devices)
//     {
//         if (IsDeviceSuitable(device, in_surface, in_device_extensions))
//         {
//             // TODO device checks
//             out_physical_device = device;
//             out_max_msaa_samples = GetMaxUsableSampleCount(device);
//             break;
//         }
//     }

//     if (out_physical_device == VK_NULL_HANDLE)
//     {
//         throw std::runtime_error("Failed to find a suitable GPU!");
//     }

//     // Create Logical Device

//     QueueFamilyIndices indices =
//         FindQueueFamilies(out_physical_device, in_surface);
//     std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
//     std::set<uint32_t> unique_queue_families = {
//         indices.graphics_family.value(),
//         indices.present_family.value()
//     };

//     float queue_priority = 1.0f;
//     for (uint32_t queue_family : unique_queue_families)
//     {
//         VkDeviceQueueCreateInfo queue_create_info{};
//         queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//         queue_create_info.queueFamilyIndex = queue_family;
//         queue_create_info.queueCount = 1;
//         queue_create_info.pQueuePriorities = &queue_priority;
//         queue_create_infos.push_back(queue_create_info);
//     }

//     // device features

//     VkPhysicalDeviceFeatures2 vk2_features{};
//     vk2_features.sType= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
//     vk2_features.features.samplerAnisotropy = VK_TRUE;

//     VkPhysicalDeviceVulkan13Features vk13_features{};
//     vk13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
//     vk13_features.dynamicRendering = VK_TRUE;
//     vk13_features.synchronization2 = VK_TRUE;

//     VkPhysicalDeviceExtendedDynamicStateFeaturesEXT vkext_features={};
//     vkext_features.sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
//     vkext_features.extendedDynamicState = VK_TRUE;

//     vkext_features.pNext = nullptr;
//     vk13_features.pNext = &vkext_features;
//     vk2_features.pNext = &vk13_features;

//     // Start device creation

//     VkDeviceCreateInfo create_info{};
//     create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

//     create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
//     create_info.pQueueCreateInfos = queue_create_infos.data();

//     create_info.pEnabledFeatures = nullptr;

//     create_info.enabledExtensionCount = static_cast<uint32_t>(in_device_extensions.size());
    
//     std::vector<const char *> enable_extension_names{};
//     WStringUtils::ToConstCharPtrs(in_device_extensions, enable_extension_names);
    
//     create_info.ppEnabledExtensionNames = enable_extension_names.data();
    
//     create_info.pNext = &vk2_features;

//     std::vector<const char *> enabled_layer_names{};
//     if (in_enable_validation_layers)
//     {
//         create_info.enabledLayerCount = static_cast<uint32_t>(in_validation_layers.size());
//         WStringUtils::ToConstCharPtrs(in_validation_layers, enabled_layer_names);
//         create_info.ppEnabledLayerNames = enabled_layer_names.data();
//     }
//     else
//     {
//         create_info.enabledLayerCount = 0;
//     }

//     ExecVkProcChecked(vkCreateDevice,
//                       "Failed to create logical device!",
//                       out_physical_device,
//                       &create_info,
//                       nullptr,
//                       &out_device);
    
//     vkGetDeviceQueue(out_device, indices.graphics_family.value(), 0, &out_graphics_queue);
//     vkGetDeviceQueue(out_device, indices.present_family.value(), 0, &out_present_queue);
// }

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

    WVulkanUtils::CopyVkBuffer(
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

    WVulkanUtils::CopyVkBuffer(
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

void WVulkan::UpdateDescriptorSets(
    const std::vector<VkWriteDescriptorSet> & in_write_descriptor_sets,
    const WVkDeviceInfo & in_device_info
    )
{
    vkUpdateDescriptorSets(
        in_device_info.vk_device,
        static_cast<std::uint32_t>(in_write_descriptor_sets.size()),
        in_write_descriptor_sets.data(),
        0,
        nullptr
        );
}

void WVulkan::Create(
    WVkCommandBufferInfo & out_command_buffer_info,
    const WVkDeviceInfo & device,
    const WVkCommandPoolInfo & command_pool_info
    )
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool_info.vk_command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = static_cast<uint32_t>(
        out_command_buffer_info.command_buffers.size()
        );

    if (vkAllocateCommandBuffers(
            device.vk_device,
            &alloc_info,
            out_command_buffer_info.command_buffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}

// Destroy functions
// -----------------

void WVulkan::Destroy(WVkInstanceInfo &instance_info)
{
    WFLOG("Destroy Vulkan Instance.");
    vkDestroyInstance(instance_info.instance, nullptr);
    instance_info = {};
}

void WVulkan::Destroy(WVkSurfaceInfo &surface_info, const WVkInstanceInfo &instance_info)
{
    WFLOG("Destroy Vulkan Surface.");
    vkDestroySurfaceKHR(instance_info.instance, surface_info.surface, nullptr);
    surface_info={};
}

void WVulkan::Destroy(WVkDeviceInfo &device_info)
{
    WFLOG("Destroy Vulkan Device.");
    vkDestroyDevice(device_info.vk_device, nullptr);
    device_info = {};
}

void WVulkan::Destroy(WVkSwapChainInfo &swap_chain_info,
                      const VkDevice &in_device)
{
    for (auto & view: swap_chain_info.views) {
        vkDestroyImageView(in_device, view, nullptr);
    }

    for (auto & mem : swap_chain_info.memory) {
        vkFreeMemory(in_device, mem, nullptr);
    }

    vkDestroySwapchainKHR(in_device, swap_chain_info.swap_chain, nullptr);
    
    swap_chain_info = {};
}

void WVulkan::DestroyImageView(WVkSwapChainInfo &swap_chain_info, const WVkDeviceInfo &device_info)
{
    for (auto image_view : swap_chain_info.views)
    {
        vkDestroyImageView(device_info.vk_device, image_view, nullptr);
    }
}

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
    WVkCommandPoolInfo & out_command_pool,
    const WVkDeviceInfo & in_device_info
    )
{
    WFLOG("Destroy Command Pool");
    vkDestroyCommandPool(
        in_device_info.vk_device,
        out_command_pool.vk_command_pool,
        nullptr
        );

    out_command_pool.vk_command_pool = nullptr;
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

// void WVulkan::AddDSL_DefaultGlobalGraphicBindings(WVkDescriptorSetLayoutInfo & out_descriptor_set_layout) {

//     VkDescriptorSetLayoutBinding camera_binding{};
//     camera_binding.binding=0;
//     camera_binding.descriptorCount = 1;
//     camera_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//     camera_binding.pImmutableSamplers = nullptr;
//     camera_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

//     // TODO lights here as another ubo

//     out_descriptor_set_layout.bindings = {
//         camera_binding
//     };
// }

// void WVulkan::AddDSL_DefaultGraphicBindings(WVkDescriptorSetLayoutInfo & out_descriptor_set_layout)
// {
//     // Model UBO
//     VkDescriptorSetLayoutBinding ubo_layout_binding{};
//     ubo_layout_binding.binding = 0;
//     ubo_layout_binding.descriptorCount = 1;
//     ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//     ubo_layout_binding.pImmutableSamplers = nullptr;
//     ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

//     // A texture in the fragment shader
//     VkDescriptorSetLayoutBinding sampler_layout_binding{};
//     sampler_layout_binding.binding = 1;
//     sampler_layout_binding.descriptorCount = 1;
//     sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//     sampler_layout_binding.pImmutableSamplers = nullptr;
//     sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

//     out_descriptor_set_layout.bindings = {
//         ubo_layout_binding,
//         sampler_layout_binding
//     };
// }

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

// WVkShaderStageInfo WVulkan::BuildGraphicsShaderStageInfo(
//     const char * in_shader_file_path,
//     const char * in_entry_point,
//     EShaderStageFlag in_shader_type
//     )
// {
//     WVkShaderStageInfo result;

//     std::string file_path = in_shader_file_path;

//     result.code = WShaderUtils::ReadShader(file_path);

//     result.entry_point = in_entry_point;
//     result.type = in_shader_type;

//     if (in_shader_type == EShaderStageFlag::Vertex)
//     {
//         result.attribute_descriptors.resize(3);

//         // Geometry data structure

//         result.attribute_descriptors[0].binding = 0;
//         result.attribute_descriptors[0].location = 0;
//         result.attribute_descriptors[0].format = VK_FORMAT_R32G32B32_SFLOAT;
//         result.attribute_descriptors[0].offset = offsetof(WVertexStruct, Position);

//         result.attribute_descriptors[1].binding = 0;
//         result.attribute_descriptors[1].location = 1;
//         result.attribute_descriptors[1].format = VK_FORMAT_R32G32_SFLOAT;
//         result.attribute_descriptors[1].offset = offsetof(WVertexStruct, TexCoords);

//         result.attribute_descriptors[2].binding = 0;
//         result.attribute_descriptors[2].location = 2;
//         result.attribute_descriptors[2].format = VK_FORMAT_R32G32B32_SFLOAT;
//         result.attribute_descriptors[2].offset = offsetof(WVertexStruct, Color);

//         // TODO Normals

//         // more vertex data bindings here

//         result.binding_descriptors.resize(1);
//         result.binding_descriptors[0].binding = 0;
//         result.binding_descriptors[0].stride = sizeof(WVertexStruct);
//         result.binding_descriptors[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//     }

//     return result;
// }

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
