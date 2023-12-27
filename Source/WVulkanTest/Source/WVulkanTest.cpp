#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <fstream>
#include <set>


namespace VulkanUtils
{
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance InInstance, 
        const VkDebugUtilsMessengerCreateInfoEXT* InCreateInfo,
        const VkAllocationCallbacks* InAllocator,
        VkDebugUtilsMessengerEXT* OutDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(InInstance, "vkCreateDebugUtilsMessengerEXT");
        if(func != nullptr)
        {
            return func(InInstance, InCreateInfo, InAllocator, OutDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT (
        VkInstance InInstance,
        VkDebugUtilsMessengerEXT InDebugMessenger,
        const VkAllocationCallbacks* InAllocator
    )
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(InInstance, "vkDestroyDebugUtilsMessengerEXT");
        if(func != nullptr)
        {
            func(InInstance, InDebugMessenger, InAllocator);
        }
    }

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> GraphicsFamily;
        std::optional<uint32_t> PresentFamily;

        bool IsComplete()
        {
            return GraphicsFamily.has_value() && PresentFamily.has_value();
        }
    };
    
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    static std::vector<char> ReadFile(const std::string& InFileName)
    {
        std::ifstream File(InFileName, std::ios::ate | std::ios::binary);

        if(!File.is_open())
        {
            throw std::runtime_error("Failed to open file!");
        }

        size_t FileSize = (size_t)File.tellg();
        std::vector<char> Buffer(FileSize);

        File.seekg(0);
        File.read(Buffer.data(), FileSize);

        File.close();

        return Buffer;
    }
}

class CHelloTriangleApplication
{
public:
    void Run()
    {
        InitWindow();
        InitVulkan();
        MainLoop();
        Cleanup();
    }

private:
    GLFWwindow* Window=nullptr;
    VkDebugUtilsMessengerEXT DebugMessenger;
    VkSurfaceKHR Surface;
    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
    VkDevice Device;

    VkQueue GraphicsQueue;
    VkQueue PresentQueue;

    VkSwapchainKHR SwapChain;
    std::vector<VkImage> SwapChainImages;
    VkFormat SwapChainImageFormat;
    VkExtent2D SwapChainExtent;
    std::vector<VkImageView> SwapChainImageViews;
    std::vector<VkFramebuffer> SwapChainFramebuffers;

    VkRenderPass RenderPass;
    VkPipelineLayout PipelineLayout;
    VkPipeline GraphicsPipeline;

    VkCommandPool CommandPool;
    VkCommandBuffer CommandBuffer;

    VkSemaphore ImageAvailableSemaphore;
    VkSemaphore RenderFinishedSemaphore;
    VkFence InFlightFence;

    const uint32_t Width{800};
    const uint32_t Height{600};

    const int MAX_FRAMES_IN_FLIGHT = 2;

    VkInstance Instance;

    const std::vector<const char*> ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char*> DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

#ifdef NDEBUG
    const bool EnableValidationLayers = false;
#else
    const bool EnableValidationLayers = true;
#endif

private:
    void InitWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // it is not OpenGL
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   // Not resizable

        Window = glfwCreateWindow(Width, Height, "Vulkan", nullptr, nullptr);
    }

    void InitVulkan()
    {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        CreateGraphicsPipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateCommandBuffer();
        CreateSyncObjects();
    }

    void MainLoop()
    {
        while (!glfwWindowShouldClose(Window))
        {
            glfwPollEvents();
            DrawFrame();
        }
        vkDeviceWaitIdle(Device);
    }

    void Cleanup()
    {
        vkDestroySemaphore(Device, RenderFinishedSemaphore, nullptr);
        vkDestroySemaphore(Device, ImageAvailableSemaphore, nullptr);
        vkDestroyFence(Device, InFlightFence, nullptr);

        vkDestroyCommandPool(Device, CommandPool, nullptr);

        for (auto Framebuffer : SwapChainFramebuffers)
        {
            vkDestroyFramebuffer(Device, Framebuffer, nullptr);
        }

        vkDestroyPipeline(Device, GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);
        vkDestroyRenderPass(Device, RenderPass, nullptr);

        for(auto ImageView : SwapChainImageViews)
        {
            vkDestroyImageView(Device, ImageView, nullptr);
        }

        vkDestroySwapchainKHR(Device, SwapChain, nullptr);
        vkDestroyDevice(Device, nullptr);

        if(EnableValidationLayers)
        {
            VulkanUtils::DestroyDebugUtilsMessengerEXT(
                Instance, 
                DebugMessenger, 
                nullptr
            );
        }

        vkDestroySurfaceKHR(Instance, Surface, nullptr);
        vkDestroyInstance(Instance, nullptr);

        glfwDestroyWindow(Window);

        glfwTerminate();
    }

    void CreateInstance()
    {
        if(EnableValidationLayers && !CheckValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }

        VkApplicationInfo AppInfo{};
        AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        AppInfo.pApplicationName = "Hello Triangle";
        AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.pEngineName = "No Engine";
        AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        CreateInfo.pApplicationInfo = &AppInfo;

        auto Extensions = GetRequiredExtensions();
        CreateInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions.size());
        CreateInfo.ppEnabledExtensionNames = Extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo;
        if(EnableValidationLayers)
        {
            CreateInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
            CreateInfo.ppEnabledLayerNames = ValidationLayers.data();

            PopulateDebugMessengerCreateInfo(DebugCreateInfo);
            CreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&DebugCreateInfo;
        } else {
            CreateInfo.enabledLayerCount = 0;
            CreateInfo.pNext = nullptr;
        }

        if(vkCreateInstance(&CreateInfo, nullptr, &Instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create instance!");
        }
    }

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& InCreateInfo)
    {
        InCreateInfo = {};
        InCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        InCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        InCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        InCreateInfo.pfnUserCallback = DebugCallback;
        InCreateInfo.pUserData = nullptr;
    }

    void SetupDebugMessenger()
    {
        if(!EnableValidationLayers) return;
        
        VkDebugUtilsMessengerCreateInfoEXT CreateInfo;
        PopulateDebugMessengerCreateInfo(CreateInfo);

        if (VulkanUtils::CreateDebugUtilsMessengerEXT(Instance, &CreateInfo, nullptr, &DebugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to set up debug messenger!");
        }
    }

    void CreateSurface()
    {
        // Create the glfw window surface
        if(glfwCreateWindowSurface(Instance, Window, nullptr, &Surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    void PickPhysicalDevice()
    {
        uint32_t DeviceCount=0;
        vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);
        
        if(DeviceCount == 0)
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }
        
        std::vector<VkPhysicalDevice> Devices(DeviceCount);
        vkEnumeratePhysicalDevices(Instance, &DeviceCount, Devices.data());

        for(const auto& Device : Devices)
        {
            if (IsDeviceSuitable(Device))
            {
                PhysicalDevice = Device;
                break;
            }
        }

        if(PhysicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }
    }

    void CreateLogicalDevice()
    {
        VulkanUtils::QueueFamilyIndices Indices = FindQueueFamilies(PhysicalDevice);
        std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;    
        std::set<uint32_t> UniqueQueueFamilies = {Indices.GraphicsFamily.value(), Indices.PresentFamily.value()};

        float QueuePriority = 1.0f;
        for (uint32_t QueueFamily : UniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo QueueCreateInfo{};
            QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            QueueCreateInfo.queueFamilyIndex = QueueFamily;
            QueueCreateInfo.queueCount = 1;
            QueueCreateInfo.pQueuePriorities = &QueuePriority;
            QueueCreateInfos.push_back(QueueCreateInfo);
        }

        VkPhysicalDeviceFeatures DeviceFeatures{};

        VkDeviceCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        CreateInfo.queueCreateInfoCount = static_cast<uint32_t>(QueueCreateInfos.size());
        CreateInfo.pQueueCreateInfos = QueueCreateInfos.data();

        CreateInfo.pEnabledFeatures = &DeviceFeatures;

        CreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
        CreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();

        if (EnableValidationLayers)
        {
            CreateInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
            CreateInfo.ppEnabledLayerNames = ValidationLayers.data();
        } else {
            CreateInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(PhysicalDevice, &CreateInfo, nullptr, &Device) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create logical device!");
        }

        vkGetDeviceQueue(Device, Indices.GraphicsFamily.value(), 0, &GraphicsQueue);
        vkGetDeviceQueue(Device, Indices.PresentFamily.value(), 0, &PresentQueue);
    }

    void CreateSwapChain()
    {
        VulkanUtils::SwapChainSupportDetails SwapChainSupport = QuerySwapChainSupport(PhysicalDevice);
        
        VkSurfaceFormatKHR SurfaceFormat = ChooseSwapSurfaceFormat(SwapChainSupport.Formats); 
        VkPresentModeKHR PresentMode = ChooseSwapPresentMode(SwapChainSupport.PresentModes);
        VkExtent2D Extent = ChooseSwapExtent(SwapChainSupport.Capabilities);

        uint32_t ImageCount = SwapChainSupport.Capabilities.minImageCount + 1;
        if (SwapChainSupport.Capabilities.maxImageCount > 0 && ImageCount > SwapChainSupport.Capabilities.maxImageCount)
        {
            ImageCount = SwapChainSupport.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        CreateInfo.surface = Surface;

        CreateInfo.minImageCount = ImageCount;
        CreateInfo.imageFormat = SurfaceFormat.format;
        CreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
        CreateInfo.imageExtent = Extent;
        CreateInfo.imageArrayLayers = 1;
        CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VulkanUtils::QueueFamilyIndices Indices = FindQueueFamilies(PhysicalDevice);
        uint32_t QueueFamilyIndices[] = {Indices.GraphicsFamily.value(), Indices.PresentFamily.value()};

        if (Indices.GraphicsFamily != Indices.PresentFamily)
        {
            CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            CreateInfo.queueFamilyIndexCount = 2;
            CreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
        } else {
            CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        CreateInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
        CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        CreateInfo.presentMode = PresentMode;
        CreateInfo.clipped = VK_TRUE;

        CreateInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(Device, &CreateInfo, nullptr, &SwapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, nullptr);
        SwapChainImages.resize(ImageCount);
        vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, SwapChainImages.data());

        SwapChainImageFormat = SurfaceFormat.format;
        SwapChainExtent = Extent;
    }

    void CreateImageViews()
    {
        SwapChainImageViews.resize(SwapChainImages.size());

        for (size_t i = 0; i < SwapChainImages.size(); i++)
        {
            VkImageViewCreateInfo CreateInfo{};
            CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            CreateInfo.image = SwapChainImages[i];
            CreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            CreateInfo.format = SwapChainImageFormat;
            CreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            CreateInfo.subresourceRange.baseMipLevel = 0;
            CreateInfo.subresourceRange.levelCount = 1;
            CreateInfo.subresourceRange.baseArrayLayer = 0;
            CreateInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(Device, &CreateInfo, nullptr, &SwapChainImageViews[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image views!");
            }
        }
    }

    void CreateRenderPass()
    {
        VkAttachmentDescription ColorAttachment{};
        ColorAttachment.format = SwapChainImageFormat;
        ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference ColorAttachmentRef{};
        ColorAttachmentRef.attachment = 0;
        ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription Subpass{};
        Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        Subpass.colorAttachmentCount = 1;
        Subpass.pColorAttachments = &ColorAttachmentRef;

        VkSubpassDependency Dependency{};
        Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        Dependency.dstSubpass = 0;
        Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        Dependency.srcAccessMask = 0;
        Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo RenderPassInfo{};
        RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        RenderPassInfo.attachmentCount = 1;
        RenderPassInfo.pAttachments = &ColorAttachment;
        RenderPassInfo.subpassCount = 1;
        RenderPassInfo.pSubpasses = &Subpass;
        RenderPassInfo.dependencyCount = 1;
        RenderPassInfo.pDependencies = &Dependency;

        if (vkCreateRenderPass(Device, &RenderPassInfo, nullptr, &RenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass!");
        }

    }

    void CreateGraphicsPipeline()
    {
        auto VertShaderCode = VulkanUtils::ReadFile(
            "Source/WVulkanTest/Shaders/ShaderBase.frag.spv"
        );
        auto FragShaderCode = VulkanUtils::ReadFile(
            "Source/WVulkanTest/Shaders/ShaderBase.vert.spv"
        );

        VkShaderModule VertShaderModule = CreateShaderModule(VertShaderCode);
        VkShaderModule FragShaderModule = CreateShaderModule(FragShaderCode);

        VkPipelineShaderStageCreateInfo VertShaderStageInfo{};
        VertShaderStageInfo.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        VertShaderStageInfo.module = VertShaderModule;
        VertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo FragShaderStageInfo{};
        FragShaderStageInfo.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        FragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        FragShaderStageInfo.module = FragShaderModule;
        FragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo ShaderStages[] = {
            VertShaderStageInfo, FragShaderStageInfo
        };

        VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
        VertexInputInfo.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        VertexInputInfo.vertexBindingDescriptionCount = 0;
        VertexInputInfo.vertexAttributeDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
        InputAssembly.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        InputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo ViewportState{};
        ViewportState.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        ViewportState.viewportCount = 1;
        ViewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo Rasterizer{};
        Rasterizer.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        Rasterizer.depthClampEnable = VK_FALSE;
        Rasterizer.rasterizerDiscardEnable = VK_FALSE;
        Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        Rasterizer.lineWidth = 1.0f;
        Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        Rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        Rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo Multisampling{};
        Multisampling.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        Multisampling.sampleShadingEnable = VK_FALSE;
        Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

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
        ColorBlending.blendConstants[0] = 0.0f;
        ColorBlending.blendConstants[1] = 0.0f;
        ColorBlending.blendConstants[2] = 0.0f;
        ColorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> DynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };
        VkPipelineDynamicStateCreateInfo DynamicState{};
        DynamicState.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        DynamicState.dynamicStateCount = static_cast<uint32_t>(DynamicStates.size());
        DynamicState.pDynamicStates = DynamicStates.data();

        VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
        PipelineLayoutInfo.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        PipelineLayoutInfo.setLayoutCount = 0;
        PipelineLayoutInfo.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(Device, &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo PipelineInfo{};
        PipelineInfo.sType = 
            VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        PipelineInfo.stageCount = 2;
        PipelineInfo.pStages = ShaderStages;
        PipelineInfo.pVertexInputState = &VertexInputInfo;
        PipelineInfo.pInputAssemblyState = &InputAssembly;
        PipelineInfo.pViewportState = &ViewportState;
        PipelineInfo.pRasterizationState = &Rasterizer;
        PipelineInfo.pMultisampleState = &Multisampling;
        PipelineInfo.pColorBlendState = &ColorBlending;
        PipelineInfo.pDynamicState = &DynamicState;
        PipelineInfo.layout = PipelineLayout;
        PipelineInfo.renderPass = RenderPass;
        PipelineInfo.subpass = 0;
        PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &GraphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(Device, FragShaderModule, nullptr);
        vkDestroyShaderModule(Device, VertShaderModule, nullptr);

    }

    void CreateFramebuffers()
    {
        SwapChainFramebuffers.resize(SwapChainImageViews.size());
        for (size_t i=0; i<SwapChainImageViews.size(); i++)
        {
            VkImageView Attachments[] = {
                SwapChainImageViews[i]
            };

            VkFramebufferCreateInfo FramebufferInfo{};
            FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            FramebufferInfo.renderPass = RenderPass;
            FramebufferInfo.attachmentCount = 1;
            FramebufferInfo.pAttachments = Attachments;
            FramebufferInfo.width = SwapChainExtent.width;
            FramebufferInfo.height = SwapChainExtent.height;
            FramebufferInfo.layers = 1;

            if (vkCreateFramebuffer(Device, &FramebufferInfo, nullptr, &SwapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create framebuffer!");
            }
        }
    }

    void CreateCommandPool()
    {
        VulkanUtils::QueueFamilyIndices QueueFamilyIndices = FindQueueFamilies(PhysicalDevice);

        VkCommandPoolCreateInfo PoolInfo{};
        PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        PoolInfo.queueFamilyIndex = QueueFamilyIndices.GraphicsFamily.value();

        if (vkCreateCommandPool(Device, &PoolInfo, nullptr, &CommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create command pool!");
        }
    }

    void CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocInfo.commandPool = CommandPool;
        AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        AllocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(Device, &AllocInfo, &CommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    void RecordCommandBuffer(VkCommandBuffer InCommandBuffer, uint32_t InImageIndex)
    {
        VkCommandBufferBeginInfo BeginInfo{};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(InCommandBuffer, &BeginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo RenderPassInfo{};
        RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassInfo.renderPass = RenderPass;
        RenderPassInfo.framebuffer = SwapChainFramebuffers[InImageIndex];
        RenderPassInfo.renderArea.offset = {0, 0};
        RenderPassInfo.renderArea.extent = SwapChainExtent;

        VkClearValue ClearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        RenderPassInfo.clearValueCount = 1;
        RenderPassInfo.pClearValues = &ClearColor;

        vkCmdBeginRenderPass(InCommandBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(InCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);
            VkViewport Viewport{};
            Viewport.x = 0.0f;
            Viewport.y = 0.0f;
            Viewport.width = (float)SwapChainExtent.width;
            Viewport.height = (float)SwapChainExtent.height;
            Viewport.minDepth = 0.0f;
            Viewport.maxDepth = 1.0f;
            vkCmdSetViewport(InCommandBuffer, 0, 1, &Viewport);

            VkRect2D Scissor{};
            Scissor.offset = {0, 0};
            Scissor.extent = SwapChainExtent;
            vkCmdSetScissor(InCommandBuffer, 0, 1, &Scissor);

            vkCmdDraw(InCommandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(InCommandBuffer);

        if (vkEndCommandBuffer(InCommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer!");
        }
    }

    void CreateSyncObjects()
    {
        VkSemaphoreCreateInfo SemaphoreInfo{};
        SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo FenceInfo{};
        FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (
            vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &ImageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &RenderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(Device, &FenceInfo, nullptr, &InFlightFence) != VK_SUCCESS
        )
        {
            throw std::runtime_error("Failed to create synchronization objects for a frame!");
        }
    }

    void DrawFrame()
    {
        vkWaitForFences(Device, 1, &InFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(Device, 1, &InFlightFence);

        uint32_t ImageIndex;
        vkAcquireNextImageKHR(Device, SwapChain, UINT64_MAX, ImageAvailableSemaphore, VK_NULL_HANDLE, &ImageIndex);

        vkResetCommandBuffer(CommandBuffer, 0);
        RecordCommandBuffer(CommandBuffer, ImageIndex);

        VkSubmitInfo SubmitInfo{};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore WaitSemaphores[] = {ImageAvailableSemaphore};
        VkPipelineStageFlags WaitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        SubmitInfo.waitSemaphoreCount = 1;
        SubmitInfo.pWaitSemaphores = WaitSemaphores;
        SubmitInfo.pWaitDstStageMask = WaitStages;
        
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &CommandBuffer;

        VkSemaphore SignalSemaphores[] = {RenderFinishedSemaphore};
        SubmitInfo.signalSemaphoreCount = 1;
        SubmitInfo.pSignalSemaphores = SignalSemaphores;

        if (vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, InFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit draw command buffer!");
        }

        VkPresentInfoKHR PresentInfo{};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pWaitSemaphores = SignalSemaphores;

        VkSwapchainKHR SwapChains[] = {SwapChain};
        PresentInfo.swapchainCount = 1;
        PresentInfo.pSwapchains = SwapChains;

        PresentInfo.pImageIndices = &ImageIndex;

        vkQueuePresentKHR(PresentQueue, &PresentInfo);
    }

    VkShaderModule CreateShaderModule(const std::vector<char>& InCode)
    {
        VkShaderModuleCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        CreateInfo.codeSize = InCode.size();
        CreateInfo.pCode = reinterpret_cast<const uint32_t*>(InCode.data());

        VkShaderModule ShaderModule;
        if (vkCreateShaderModule(Device, &CreateInfo, nullptr, &ShaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module!");
        }
        return ShaderModule;
    }

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& InAvailableFormats)
    {
        for(const auto& AvailableFormat : InAvailableFormats)
        {
            if(AvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return AvailableFormat;
            }
        }
        return InAvailableFormats[0];
    }

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& InAvailablePresentModes)
    {
        for(const auto& AvailablePresentMode : InAvailablePresentModes)
        {
            if(AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return AvailablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& InCapabilities)
    {
        if(InCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return InCapabilities.currentExtent;
        }
        else
        {
            int Width, Height;
            glfwGetFramebufferSize(Window, &Width, &Height);

            VkExtent2D ActualExtent = {
                static_cast<uint32_t>(Width),
                static_cast<uint32_t>(Height)
            };

            ActualExtent.width = std::max(InCapabilities.minImageExtent.width, std::min(InCapabilities.maxImageExtent.width, ActualExtent.width));
            ActualExtent.height = std::max(InCapabilities.minImageExtent.height, std::min(InCapabilities.maxImageExtent.height, ActualExtent.height));

            return ActualExtent;
        }
    }

    VulkanUtils::SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device)
    {
        VulkanUtils::SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, Surface, &details.Capabilities);

    }

    bool IsDeviceSuitable(VkPhysicalDevice InDevice)
    {
        VulkanUtils::QueueFamilyIndices Indices = FindQueueFamilies(InDevice);

        bool ExtensionsSupported = CheckDeviceExtensionSupport(InDevice);

        bool SwapChainAdequate = false;
        if(ExtensionsSupported)
        {
            VulkanUtils::SwapChainSupportDetails SwapChainSupport = QuerySwapChainSupport(InDevice);
            SwapChainAdequate = !SwapChainSupport.Formats.empty() && !SwapChainSupport.PresentModes.empty();
        }

        return Indices.IsComplete() && ExtensionsSupported && SwapChainAdequate;
    }

    bool CheckDeviceExtensionSupport(VkPhysicalDevice InDevice)
    {
        uint32_t ExtensionCount;
        vkEnumerateDeviceExtensionProperties(InDevice, nullptr, &ExtensionCount, nullptr);
        std::vector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
        vkEnumerateDeviceExtensionProperties(InDevice, nullptr, &ExtensionCount, AvailableExtensions.data());

        std::set<std::string> RequiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

        for(const auto& Extension : AvailableExtensions)
        {
            RequiredExtensions.erase(Extension.extensionName);
        }
        return RequiredExtensions.empty();
    }

    VulkanUtils::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice InDevice)
    {
        VulkanUtils::QueueFamilyIndices Indices;

        uint32_t QueueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &QueueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &QueueFamilyCount, QueueFamilies.data());

        int i=0;
        for(const auto& QueueFamily : QueueFamilies)
        {
            if(QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                Indices.GraphicsFamily = i;
            }

            VkBool32 PresentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(InDevice, i, Surface, &PresentSupport);
            if(PresentSupport)
            {
                Indices.PresentFamily = i;
            }

            if(Indices.IsComplete())
            {
                break;
            }

            i++;
        }

        return Indices;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT InMessageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT InMessageType,
        const VkDebugUtilsMessengerCallbackDataEXT* InCallbackData,
        void* InUserData)
    {
        std::cerr << "Validation layer: " << InCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    std::vector<const char*> GetRequiredExtensions()
    {
        uint32_t GLFWExtensionCount = 0;
        const char** GLFWExtensions;
        GLFWExtensions = glfwGetRequiredInstanceExtensions(&GLFWExtensionCount);

        std::vector<const char*> Extensions(GLFWExtensions, GLFWExtensions + GLFWExtensionCount);

        if(EnableValidationLayers)
        {
            Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return Extensions;
    }

    bool CheckValidationLayerSupport()
    {
        uint32_t LayerCount;
        vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
        std::vector<VkLayerProperties> AvailableLayers(LayerCount);
        vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());

        for(const char* LayerName : ValidationLayers)
        {
            bool LayerFound = false;
            for(const auto& LayerProperties : AvailableLayers)
            {
                if(strcmp(LayerName, LayerProperties.layerName) == 0)
                {
                    LayerFound = true;
                    break;
                }
            }
            if(!LayerFound)
            {
                return false;
            }
        }
        return true;
    }
};

int main()
{
    CHelloTriangleApplication App;

    try {
        App.Run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

