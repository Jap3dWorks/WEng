#include "WVulkan.h"

#include <GLFW/glfw3.h>
#include <array>
#include <set>
#include <string>
#include <cstring>
#include <cmath>
#include "WStructs/WTextureStructs.h"
#include "WStructs/WGeometryStructs.h"


// WVulkan
// -------

// Create functions
// ----------------

void WVulkan::CreateInstance(WInstanceInfo &out_instance_info,  const WRenderDebugInfo &debug_info)
{
    if (debug_info.enable_validation_layers && !CheckValidationLayerSupport(debug_info))
    {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "WEngine"; // 
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "WEngine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    auto extensions = GetRequiredExtensions(debug_info.enable_validation_layers);
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    if (debug_info.enable_validation_layers)
    {
        create_info.enabledLayerCount = static_cast<uint32_t>(debug_info.validation_layers.size());
        create_info.ppEnabledLayerNames = debug_info.validation_layers.data();

        debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        if (debug_info.debug_callback)
        {
            debug_create_info.pfnUserCallback = debug_info.debug_callback;
        }
        else
        {
            debug_create_info.pfnUserCallback = DebugCallback;
        }

        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    }
    else
    {
        create_info.enabledLayerCount = 0;
        create_info.pNext = nullptr;
    }

    if (vkCreateInstance(&create_info, nullptr, &out_instance_info.instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance!");
    }
}   

void WVulkan::CreateSurface(WSurfaceInfo &surface, const WInstanceInfo &instance_info, const WWindowInfo &window)
{
    if (glfwCreateWindowSurface(instance_info.instance, window.window, nullptr, &surface.surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface!");
    }
}

void WVulkan::CreateWindow(WWindowInfo &out_window_info)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    out_window_info.window = glfwCreateWindow(
        out_window_info.width, out_window_info.height, out_window_info.title.c_str(), nullptr, nullptr
    );

    glfwSetWindowUserPointer(out_window_info.window, out_window_info.user_pointer);
    if (out_window_info.framebuffer_size_callback)
    {
        glfwSetFramebufferSizeCallback(
            out_window_info.window, 
            out_window_info.framebuffer_size_callback
        );
    }
}

void WVulkan::CreateSwapChain(
    WSwapChainInfo& out_swap_chain_info, 
    const WDeviceInfo& device_info, 
    const WSurfaceInfo &surface_info,
    const WWindowInfo &window_info,
    const WRenderPassInfo &render_pass_info,
    const WRenderDebugInfo &debug_info
)
{
    SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(
        device_info.vk_physical_device,
        surface_info.surface
    );

    VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
    VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.present_modes);
    VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities, window_info.window);

    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
    if(swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
    {
        image_count = swap_chain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface_info.surface;
    
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;  // Swap chain image resolution
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;  // VK_IMAGE_USAGE_TRANSFER_DST_BIT for post-processing

    QueueFamilyIndices indices = FindQueueFamilies(device_info.vk_physical_device, surface_info.surface);
    uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};

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

    if (vkCreateSwapchainKHR(device_info.vk_device, &create_info, nullptr, &out_swap_chain_info.swap_chain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain!");
    }

    // Retrieve Swap Chain Images
    vkGetSwapchainImagesKHR(device_info.vk_device, out_swap_chain_info.swap_chain, &image_count, nullptr);
    out_swap_chain_info.swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device_info.vk_device, out_swap_chain_info.swap_chain, &image_count, out_swap_chain_info.swap_chain_images.data());

    // Save Swap Chain Image Format
    out_swap_chain_info.swap_chain_image_format = surface_format.format;
    out_swap_chain_info.swap_chain_extent = extent;

    // Color Resources, renders are drawn to this image, later to be presented to the swap chain
    CreateVkImage(
        device_info.vk_device,
        device_info.vk_physical_device,
        out_swap_chain_info.swap_chain_extent.width,
        out_swap_chain_info.swap_chain_extent.height,
        1,
        device_info.msaa_samples,
        out_swap_chain_info.swap_chain_image_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        out_swap_chain_info.color_image,
        out_swap_chain_info.color_image_memory
    );

    out_swap_chain_info.color_image_view = CreateVkImageView(
        device_info.vk_device,
        out_swap_chain_info.color_image,
        out_swap_chain_info.swap_chain_image_format,
        VK_IMAGE_ASPECT_COLOR_BIT,
        1
    );

    // Depth Resources
    VkFormat depth_format = FindDepthFormat(device_info.vk_physical_device);
    CreateVkImage(
        device_info.vk_device,
        device_info.vk_physical_device,
        out_swap_chain_info.swap_chain_extent.width,
        out_swap_chain_info.swap_chain_extent.height,
        1,
        device_info.msaa_samples,
        depth_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        out_swap_chain_info.depth_image,
        out_swap_chain_info.depth_image_memory
    );

    out_swap_chain_info.depth_image_view = CreateVkImageView(
        device_info.vk_device,
        out_swap_chain_info.depth_image,
        depth_format,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        1
    );

    //  Framebuffer is needed to render to the image
    out_swap_chain_info.swap_chain_framebuffers.resize(out_swap_chain_info.swap_chain_images.size());
    for(size_t i = 0; i < out_swap_chain_info.swap_chain_images.size(); i++)
    {
        std::array<VkImageView, 3> attachments = {
            out_swap_chain_info.color_image_view,
            out_swap_chain_info.depth_image_view,
            out_swap_chain_info.swap_chain_image_views[i]
        };

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = render_pass_info.render_pass;
        framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.width = out_swap_chain_info.swap_chain_extent.width;
        framebuffer_info.height = out_swap_chain_info.swap_chain_extent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(device_info.vk_device, &framebuffer_info, nullptr, &out_swap_chain_info.swap_chain_framebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
}

void WVulkan::CreateImageViews(WSwapChainInfo &swap_chain_info, const WDeviceInfo &device_info)
{
    swap_chain_info.swap_chain_image_views.resize(swap_chain_info.swap_chain_images.size());

    for (size_t i = 0; i < swap_chain_info.swap_chain_images.size(); i++)
    {
        swap_chain_info.swap_chain_image_views[i] = CreateVkImageView(
            device_info.vk_device,
            swap_chain_info.swap_chain_images[i],
            swap_chain_info.swap_chain_image_format,
            VK_IMAGE_ASPECT_COLOR_BIT,
            1
        );
    }
}

void WVulkan::CreateRenderPass(WRenderPassInfo &out_render_pass_info, const WSwapChainInfo &swap_chain_info, const WDeviceInfo& device_info)
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = swap_chain_info.swap_chain_image_format;
    color_attachment.samples = device_info.msaa_samples;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = FindDepthFormat(device_info.vk_physical_device);
    depth_attachment.samples = device_info.msaa_samples;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription color_attachment_resolve{};
    color_attachment_resolve.format = swap_chain_info.swap_chain_image_format;
    color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference color_attachment_resolve_ref{};
    color_attachment_resolve_ref.attachment = 2;
    color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;
    subpass.pResolveAttachments = &color_attachment_resolve_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> Attachments = {
        color_attachment, depth_attachment, color_attachment_resolve
    };
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = static_cast<uint32_t>(Attachments.size());
    render_pass_info.pAttachments = Attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if(vkCreateRenderPass(device_info.vk_device, &render_pass_info, nullptr, &out_render_pass_info.render_pass) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create render pass!");
    }
}

void WVulkan::CreateCommandPool(WCommandPoolInfo& command_pool_info, const WDeviceInfo& device_info, const WSurfaceInfo &surface_info)
{
    QueueFamilyIndices queue_family_indices = FindQueueFamilies(device_info.vk_physical_device, surface_info.surface);
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

    if (vkCreateCommandPool(device_info.vk_device, &pool_info, nullptr, &command_pool_info.vk_command_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command pool!");
    }
}

void WVulkan::CreateDevice(WDeviceInfo &device_info, const WInstanceInfo &instance_info, const WSurfaceInfo &surface_info, const WRenderDebugInfo &debug_info)
{
    // Pick Physical Device
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance_info.instance, &device_count, nullptr);
    if (device_count == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance_info.instance, &device_count, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device, surface_info.surface, device_info.device_extensions))
        {
            device_info.vk_physical_device = device;
            break;
        }
    }

    if (device_info.vk_physical_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    // Create Logical Device
    QueueFamilyIndices indices = FindQueueFamilies(device_info.vk_physical_device, surface_info.surface);
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

    float queue_priority = 1.0f;
    for (uint32_t queue_family : unique_queue_families)
    {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features{};
    device_features.samplerAnisotropy = VK_TRUE;
    device_features.sampleRateShading = VK_TRUE;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();

    create_info.pEnabledFeatures = &device_features;

    create_info.enabledExtensionCount = static_cast<uint32_t>(device_info.device_extensions.size());
    create_info.ppEnabledExtensionNames = device_info.device_extensions.data();

    if (debug_info.enable_validation_layers)
    {
        create_info.enabledLayerCount = static_cast<uint32_t>(debug_info.validation_layers.size());
        create_info.ppEnabledLayerNames = debug_info.validation_layers.data();
    }
    else
    {
        create_info.enabledLayerCount = 0;
    }

    if(vkCreateDevice(device_info.vk_physical_device, &create_info, nullptr, &device_info.vk_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device!");
    }
    vkGetDeviceQueue(device_info.vk_device, indices.graphics_family.value(), 0, &device_info.vk_graphics_queue);
    vkGetDeviceQueue(device_info.vk_device, indices.present_family.value(), 0, &device_info.vk_present_queue);

}

WShaderModuleManager WVulkan::CreateShaderModule(const WDeviceInfo& device, const WShaderStageInfo& out_shader_info)
{
    return WShaderModuleManager(out_shader_info, device);
}

void WVulkan::CreateVkTexture(
    WTextureInfo& out_texture_info,
    const WTextureStruct& texture_struct,
    const WDeviceInfo& device_info,
    const WCommandPoolInfo& command_pool_info
)
{
    out_texture_info.mip_levels = static_cast<uint32_t>(
        std::floor(
            std::log2(
                std::max(texture_struct.width, texture_struct.height)
            )
        )
    ) + 1;

    uint8_t channels_num = NumOfChannels(texture_struct.channels);

    VkDeviceSize image_size = texture_struct.width * texture_struct.height * channels_num;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateVkBuffer(
        device_info.vk_device,
        device_info.vk_physical_device,
        image_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer,
        staging_buffer_memory
    );

    void* data;
    vkMapMemory(device_info.vk_device, staging_buffer_memory, 0, image_size, 0, &data);
        memcpy(data, texture_struct.data.data(), static_cast<size_t>(image_size));
    vkUnmapMemory(device_info.vk_device, staging_buffer_memory);

    CreateVkImage(
        device_info.vk_device,
        device_info.vk_physical_device,
        texture_struct.width,
        texture_struct.height,
        out_texture_info.mip_levels,
        VK_SAMPLE_COUNT_1_BIT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        out_texture_info.image,
        out_texture_info.image_memory
    );

    TransitionImageLayout(
        device_info.vk_device,
        command_pool_info.vk_command_pool,
        device_info.vk_graphics_queue,
        out_texture_info.image,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        out_texture_info.mip_levels
    );

    // Image view
    out_texture_info.image_view = CreateVkImageView(
        device_info.vk_device,
        out_texture_info.image,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_ASPECT_COLOR_BIT,
        out_texture_info.mip_levels
    );

    // Sampler
    out_texture_info.sampler = CreateVkTextureSampler(
        device_info.vk_device,
        device_info.vk_physical_device,
        out_texture_info.mip_levels
    );

}

void WVulkan::CreateVkRenderPipeline(
    const WDeviceInfo &device,
    const WDescriptorSetLayoutInfo& descriptor_set_layout_info, 
    const WRenderPassInfo& render_pass_info,
    WRenderPipelineInfo& out_pipeline_info
)
{
    // Create Shader Stages
    std::vector<VkPipelineShaderStageCreateInfo> ShaderStages(
        out_pipeline_info.shaders.size()
    );

    WShaderStageInfo *vertex_shader_stage = nullptr;

    for (uint32_t i = 0; i < out_pipeline_info.shaders.size(); i++)
    {
        ShaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStages[i].stage = WVulkan::ToShaderStageFlagBits(
            out_pipeline_info.shaders[i].type
        );
        
        if(out_pipeline_info.shaders[i].type == WShaderType::Vertex)
        {
            vertex_shader_stage = &out_pipeline_info.shaders[i];
        }

        WShaderModuleManager shader_module(out_pipeline_info.shaders[i], device);

        ShaderStages[i].module = shader_module.GetShaderModule();
        ShaderStages[i].pName = out_pipeline_info.shaders[i].entry_point.c_str();
    }

    if (vertex_shader_stage == nullptr)
    {
        throw std::runtime_error("Vertex shader stage not found!");
    }

    VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
    VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(
        vertex_shader_stage->binding_descriptors.size()
    );
    VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>( 
        vertex_shader_stage->attribute_descriptors.size()
    );
    VertexInputInfo.pVertexBindingDescriptions = 
        vertex_shader_stage->binding_descriptors.data();
    VertexInputInfo.pVertexAttributeDescriptions = 
        vertex_shader_stage->attribute_descriptors.data();

    VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
    InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    InputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo ViewportState{};
    ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportState.viewportCount = 1;
    ViewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo Rasterizer{};
    Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    Rasterizer.depthClampEnable = VK_FALSE;
    Rasterizer.rasterizerDiscardEnable = VK_FALSE;
    Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    Rasterizer.lineWidth = 1.0f;
    Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // because of y-flip in the projection matrix
    Rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo Multisampling{};
    Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    Multisampling.sampleShadingEnable = VK_FALSE;
    Multisampling.rasterizationSamples = out_pipeline_info.sample_count;

    VkPipelineDepthStencilStateCreateInfo DepthStencil{};
    DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    DepthStencil.depthTestEnable = VK_TRUE;
    DepthStencil.depthWriteEnable = VK_TRUE;
    DepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    DepthStencil.depthBoundsTestEnable = VK_FALSE;
    DepthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState ColorBlendAttachment{};
    ColorBlendAttachment.colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT | 
        VK_COLOR_COMPONENT_G_BIT | 
        VK_COLOR_COMPONENT_B_BIT | 
        VK_COLOR_COMPONENT_A_BIT;
    ColorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo ColorBlending{};
    ColorBlending.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlending.logicOpEnable = VK_FALSE;
    ColorBlending.logicOp = VK_LOGIC_OP_COPY;
    ColorBlending.attachmentCount = 1;
    ColorBlending.pAttachments = &ColorBlendAttachment;
    ColorBlending.blendConstants[0] = 0.f;
    ColorBlending.blendConstants[1] = 0.f;
    ColorBlending.blendConstants[2] = 0.f;
    ColorBlending.blendConstants[3] = 0.f;

    std::vector<VkDynamicState> DynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo DynamicState{};
    DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    DynamicState.dynamicStateCount = static_cast<uint32_t>(DynamicStates.size());
    DynamicState.pDynamicStates = DynamicStates.data();

    VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = 0;
    PipelineLayoutInfo.pSetLayouts = &descriptor_set_layout_info.descriptor_set_layout;

    if (vkCreatePipelineLayout(
        device.vk_device,
        &PipelineLayoutInfo,
        nullptr,
        &out_pipeline_info.pipeline_layout
    ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo PipelineInfo{};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    PipelineInfo.stageCount = static_cast<uint32_t>(ShaderStages.size());
    PipelineInfo.pStages = ShaderStages.data();
    PipelineInfo.pVertexInputState = &VertexInputInfo;
    PipelineInfo.pInputAssemblyState = &InputAssembly;
    PipelineInfo.pViewportState = &ViewportState;
    PipelineInfo.pRasterizationState = &Rasterizer;
    PipelineInfo.pMultisampleState = &Multisampling;
    PipelineInfo.pDepthStencilState = &DepthStencil;
    PipelineInfo.pColorBlendState = &ColorBlending;
    PipelineInfo.pDynamicState = &DynamicState;
    PipelineInfo.layout = out_pipeline_info.pipeline_layout;
    PipelineInfo.renderPass = render_pass_info.render_pass;
    PipelineInfo.subpass = out_pipeline_info.subpass;
    PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(
        device.vk_device,
        VK_NULL_HANDLE,
        1,
        &PipelineInfo,
        nullptr,
        &out_pipeline_info.pipeline
    ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline!");
    }
}

void WVulkan::CreateVkDescriptorSetLayout(
    const WDeviceInfo &device, WDescriptorSetLayoutInfo& out_descriptor_set_layout_info)
{
    VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutInfo{};
    DescriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    DescriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(
        out_descriptor_set_layout_info.bindings.size()
    );
    DescriptorSetLayoutInfo.pBindings = out_descriptor_set_layout_info.bindings.data();

    if (vkCreateDescriptorSetLayout(
        device.vk_device,
        &DescriptorSetLayoutInfo,
        nullptr,
        &out_descriptor_set_layout_info.descriptor_set_layout
    ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}

void WVulkan::CreateVkImage(
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
        properties
    );

    if (vkAllocateMemory(device, &alloc_info, nullptr, &out_image_memory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate image memory!");
    }

    vkBindImageMemory(device, out_image, out_image_memory, 0);
}

void WVulkan::CreateVkMesh(
    WMeshInfo &out_mesh_ingo,
    const WMeshStruct &mesh_struct,
    const WDeviceInfo &device, 
    const WCommandPoolInfo &command_pool_info
)
{
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    
    // vertex buffer

     VkDeviceSize buffer_size = 
        sizeof(mesh_struct.vertices[0]) * mesh_struct.vertices.size();
    
    CreateVkBuffer(
        device.vk_device,
        device.vk_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer,
        staging_buffer_memory
    );

    void* data;
    vkMapMemory(device.vk_device, staging_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, mesh_struct.vertices.data(), static_cast<size_t>(buffer_size));
    vkUnmapMemory(device.vk_device, staging_buffer_memory);

    CreateVkBuffer(
        device.vk_device,
        device.vk_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        out_mesh_ingo.vertex_buffer,
        out_mesh_ingo.vertex_buffer_memory
    );
    
    CopyVkBuffer(
        device.vk_device,
        command_pool_info.vk_command_pool,
        device.vk_graphics_queue,
        staging_buffer,
        out_mesh_ingo.vertex_buffer,
        buffer_size
    );

    vkDestroyBuffer(device.vk_device, staging_buffer, nullptr);
    vkFreeMemory(device.vk_device, staging_buffer_memory, nullptr);

    // index buffer

    buffer_size = sizeof(mesh_struct.indices[0]) * mesh_struct.indices.size();

    CreateVkBuffer(
        device.vk_device,
        device.vk_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer,
        staging_buffer_memory
    );
    
    vkMapMemory(device.vk_device, staging_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, mesh_struct.indices.data(), static_cast<size_t>(buffer_size));
    vkUnmapMemory(device.vk_device, staging_buffer_memory);

    CreateVkBuffer(
        device.vk_device,
        device.vk_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        out_mesh_ingo.index_buffer,
        out_mesh_ingo.index_buffer_memory
    );

    CopyVkBuffer(
        device.vk_device,
        command_pool_info.vk_command_pool,
        device.vk_graphics_queue,
        staging_buffer,
        out_mesh_ingo.index_buffer,
        buffer_size
    );

    vkDestroyBuffer(device.vk_device, staging_buffer, nullptr);
    vkFreeMemory(device.vk_device, staging_buffer_memory, nullptr);
}

void WVulkan::CreateVkUniformBuffer(
    WUniformBufferObjectInfo& out_uniform_buffer_object_info,
    const WDeviceInfo& device
)
{
    VkDeviceSize buffer_size = sizeof(WUniformBufferObject);
    CreateVkBuffer(
        device.vk_device,
        device.vk_physical_device,
        buffer_size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        out_uniform_buffer_object_info.uniform_buffer,
        out_uniform_buffer_object_info.uniform_buffer_memory
    );

    vkMapMemory(
        device.vk_device,
        out_uniform_buffer_object_info.uniform_buffer_memory,
        0,
        buffer_size,
        0,
        &out_uniform_buffer_object_info.mapped_data
    );
}

// Destroy functions
// -----------------

void WVulkan::DestroyInstance(WInstanceInfo &instance_info)
{
    vkDestroyInstance(instance_info.instance, nullptr);
}

void WVulkan::DestroySurface(WSurfaceInfo &surface_info, const WInstanceInfo &instance_info)
{
    vkDestroySurfaceKHR(instance_info.instance, surface_info.surface, nullptr);
}

void WVulkan::DestroyDevice(WDeviceInfo &device_info)
{
    vkDestroyDevice(device_info.vk_device, nullptr);
}

void WVulkan::DestroySwapChain(WSwapChainInfo &swap_chain_info, const WDeviceInfo &device_info)
{

    vkDestroyImageView(device_info.vk_device, swap_chain_info.color_image_view, nullptr);
    vkDestroyImage(device_info.vk_device, swap_chain_info.color_image, nullptr);
    vkFreeMemory(device_info.vk_device, swap_chain_info.color_image_memory, nullptr);

    vkDestroyImageView(device_info.vk_device, swap_chain_info.depth_image_view, nullptr);
    vkDestroyImage(device_info.vk_device, swap_chain_info.depth_image, nullptr);
    vkFreeMemory(device_info.vk_device, swap_chain_info.depth_image_memory, nullptr);

    for(auto framebuffer : swap_chain_info.swap_chain_framebuffers)
    {
        vkDestroyFramebuffer(device_info.vk_device, framebuffer, nullptr);
    }

    for (auto image_view : swap_chain_info.swap_chain_image_views)
    {
        vkDestroyImageView(device_info.vk_device, image_view, nullptr);
    }
    vkDestroySwapchainKHR(device_info.vk_device, swap_chain_info.swap_chain, nullptr);
}

void WVulkan::DestroyImageViews(WSwapChainInfo &swap_chain_info, const WDeviceInfo &device_info)
{
    for (auto image_view : swap_chain_info.swap_chain_image_views)
    {
        vkDestroyImageView(device_info.vk_device, image_view, nullptr);
    }
}

void WVulkan::DestroyRenderPass(WRenderPassInfo &render_pass_info, const WDeviceInfo &device_info)
{
    vkDestroyRenderPass(device_info.vk_device, render_pass_info.render_pass, nullptr);
}

void WVulkan::DestroyWindow(WWindowInfo &window_info)
{
    glfwDestroyWindow(window_info.window);
    glfwTerminate();
}

void WVulkan::DestroyVkRenderPipeline(const WDeviceInfo &device, const WRenderPipelineInfo& pipeline_info)
{
    if (pipeline_info.pipeline_layout)
    {
        // destroy pipeline layout
        vkDestroyPipelineLayout(
            device.vk_device,
            pipeline_info.pipeline_layout,
            nullptr
        );
    }

    if (pipeline_info.pipeline)
    {   
        // destroy pipeline
        vkDestroyPipeline(
            device.vk_device,
            pipeline_info.pipeline,
            nullptr
        );
    }
}

void WVulkan::DestroyDescriptorSetLayout(const WDeviceInfo &device, const WDescriptorSetLayoutInfo& descriptor_set_layout_info)
{
    // destroy descriptor set layout
    vkDestroyDescriptorSetLayout(
        device.vk_device,
        descriptor_set_layout_info.descriptor_set_layout,
        nullptr
    );
}

// Helper functions
// ----------------

bool WVulkan::CheckValidationLayerSupport(const WRenderDebugInfo &debug_info)
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char* layer_name : debug_info.validation_layers)
    {
        bool layer_found = false;

        for (const auto& layer_properties : available_layers)
        {
            if (std::strcmp(layer_name, layer_properties.layerName) == 0)
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

std::vector<const char*> WVulkan::GetRequiredExtensions(bool enable_validation_layers)
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    if (enable_validation_layers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL WVulkan::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT InMessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT InMessageType,
    const VkDebugUtilsMessengerCallbackDataEXT* InCallbackData,
    void* InUserData
)
{
    std::cerr << "Validation layer: " << InCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

bool WVulkan::IsDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, const std::vector<const char*>& device_extensions)
{
    QueueFamilyIndices indices = FindQueueFamilies(device, surface);

    bool extensions_supported = CheckDeviceExtensionSupport(device, device_extensions);

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

bool WVulkan::CheckDeviceExtensionSupport(const VkPhysicalDevice& device, const std::vector<const char*>& device_extensions)
{
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

    for (const auto& extension : available_extensions)
    {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

WVulkan::QueueFamilyIndices WVulkan::FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
{
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    int i = 0;
    for (const auto& queue_family : queue_families)
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

WVulkan::SwapChainSupportDetails WVulkan::QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
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
    for(const auto& available_format : available_formats)
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return available_format;
        }
    }
    return available_formats[0];
}

VkPresentModeKHR WVulkan::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes)
{
    for (const auto& available_present_mode : available_present_modes)
    {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return available_present_mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D WVulkan::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow* window)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        VkExtent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actual_extent.width = std::max(
            capabilities.minImageExtent.width, 
            std::min(capabilities.maxImageExtent.width, actual_extent.width)
        );
        actual_extent.height = std::max(
            capabilities.minImageExtent.height, 
            std::min(capabilities.maxImageExtent.height, actual_extent.height)
        );

        return actual_extent;
    }
}

VkImageView  WVulkan::CreateVkImageView(const VkDevice& device, const VkImage& image, const VkFormat& format, const VkImageAspectFlags& aspect_flags, const uint32_t& mip_levels)
{
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = format;
    view_info.subresourceRange.aspectMask = aspect_flags;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = mip_levels;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    VkImageView image_view;
    if (vkCreateImageView(device, &view_info, nullptr, &image_view) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create texture image view!");
    }

    return image_view;
}

void WVulkan::CreateVkBuffer(
    const VkDevice& device,
    const VkPhysicalDevice& physical_device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& out_buffer,
    VkDeviceMemory& out_buffer_memory
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
        properties
    );

    if (vkAllocateMemory(device, &alloc_info, nullptr, &out_buffer_memory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, out_buffer, out_buffer_memory, 0);
}

VkFormat WVulkan::FindSupportedFormat(const VkPhysicalDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
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

VkFormat WVulkan::FindDepthFormat(const VkPhysicalDevice& device)
{
    return FindSupportedFormat(
        device,
        {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,  // Stencil
            VK_FORMAT_D24_UNORM_S8_UINT   // Stencil
        },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

uint32_t WVulkan::FindMemoryType(const VkPhysicalDevice& device, uint32_t type_filter, VkMemoryPropertyFlags properties)
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

VkShaderStageFlagBits WVulkan::ToShaderStageFlagBits(const WShaderType& type)
{
    switch (type)
    {
    case WShaderType::Vertex:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case WShaderType::Fragment:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    case WShaderType::Compute:
        return VK_SHADER_STAGE_COMPUTE_BIT;
    default:
        throw std::runtime_error("Invalid shader type!");
    }
}

void WVulkan::TransitionImageLayout(
    const VkDevice& device,
    const VkCommandPool& command_pool, 
    const VkQueue& graphics_queue, 
    const VkImage& image, 
    const VkFormat& format, 
    const VkImageLayout& old_layout, 
    const VkImageLayout& new_layout, 
    const uint32_t& mip_levels
)
{
    VkCommandBuffer command_buffer = BeginSingleTimeCommands(device, command_pool);

    VkImageMemoryBarrier barrier{};
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
        &barrier
    );

    EndSingleTimeCommands(
        device, 
        command_pool, 
        graphics_queue, 
        command_buffer
    );
}

VkCommandBuffer WVulkan::BeginSingleTimeCommands(const VkDevice& device, const VkCommandPool& command_pool)
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
    const VkDevice& device,
    const VkCommandPool& command_pool,
    const VkQueue& graphics_queue,
    const VkCommandBuffer& command_buffer
)
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

VkSampler WVulkan::CreateVkTextureSampler(
    const VkDevice& device,
    const VkPhysicalDevice& physical_device,
    const uint32_t& mip_levels
)
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
    sampler_info.minLod = 0;
    sampler_info.maxLod = static_cast<float>(mip_levels);
    sampler_info.mipLodBias = 0;
    
    VkSampler texture_sampler;
    if (vkCreateSampler(device, &sampler_info, nullptr, &texture_sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create texture sampler!");
    }

    return texture_sampler;
}

void WVulkan::CopyVkBuffer(
    const VkDevice& device,
    const VkCommandPool& command_pool,
    const VkQueue& graphics_queue,
    const VkBuffer& src_buffer,
    const VkBuffer& dst_buffer,
    const VkDeviceSize& size
)
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = command_pool;
    alloc_info.commandBufferCount = 1;
    alloc_info.pNext = nullptr;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);

        VkBufferCopy copy_region{};
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue);

    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}


