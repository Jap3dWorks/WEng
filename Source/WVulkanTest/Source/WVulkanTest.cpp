#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <fstream>
#include <array>
#include <set>
#include <unordered_map>


struct Vertex{
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec2 TexCoord;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription BindingDescription{};
     
        BindingDescription.binding = 0;
        BindingDescription.stride = sizeof(Vertex);
        BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
     
        return BindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> AttributeDescriptions{};

        AttributeDescriptions[0].binding = 0;
        AttributeDescriptions[0].location = 0;
        AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        AttributeDescriptions[0].offset = offsetof(Vertex, Position);

        AttributeDescriptions[1].binding = 0;
        AttributeDescriptions[1].location = 1;
        AttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        AttributeDescriptions[1].offset = offsetof(Vertex, Color);

        AttributeDescriptions[2].binding = 0;
        AttributeDescriptions[2].location = 2;
        AttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        AttributeDescriptions[2].offset = offsetof(Vertex, TexCoord);
    
        return AttributeDescriptions;
    }

    bool operator==(const Vertex& InOther) const
    {
        return Position == InOther.Position && Color == InOther.Color && TexCoord == InOther.TexCoord;
    }
};

namespace std
{
    template<> struct hash<Vertex>
    {
        size_t operator()(Vertex const& InVertex) const
        {
            return ((hash<glm::vec3>()(InVertex.Position) ^
                    (hash<glm::vec3>()(InVertex.Color) << 1)) >> 1) ^
                    (hash<glm::vec2>()(InVertex.TexCoord) << 1);
        }
    };
}

struct SMesh
{
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
};

struct SUniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

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

namespace VulkanConfig
{
    const int MAX_FRAMES_IN_FLIGHT = 2;
    
}

namespace AssetConfig
{
    const std::string MODEL_PATH = "WVulkanTest_Content/Assets/Models/viking_room.obj";
    const std::string TEXTURE_PATH = "WVulkanTest_Content/Assets/Textures/viking_room.png";
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

    SMesh Mesh;

    VkInstance Instance;
    VkDebugUtilsMessengerEXT DebugMessenger;
    VkSurfaceKHR Surface;
    
    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
    VkSampleCountFlagBits MsaaSamples = VK_SAMPLE_COUNT_1_BIT;
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
    VkDescriptorSetLayout DescriptorSetLayout;
    VkPipelineLayout PipelineLayout;
    VkPipeline GraphicsPipeline;

    VkCommandPool CommandPool;

    uint32_t MipLevels;
    VkImage TextureImage;
    VkDeviceMemory TextureImageMemory;
    VkImageView TextureImageView;
    VkSampler TextureSampler;

    VkBuffer VertexBuffer;
    VkDeviceMemory VertexBufferMemory;
    VkBuffer IndexBuffer;
    VkDeviceMemory IndexBufferMemory;

    VkImage ColorImage;
    VkDeviceMemory ColorImageMemory;
    VkImageView ColorImageView;    

    VkImage DepthImage;
    VkDeviceMemory DepthImageMemory;
    VkImageView DepthImageView;

    std::vector<VkBuffer> UniformBuffers;
    std::vector<VkDeviceMemory> UniformBuffersMemory;
    std::vector<void*> UniformBuffersMapped;

    VkDescriptorPool DescriptorPool;
    std::vector<VkDescriptorSet> DescriptorSets;

    std::vector<VkCommandBuffer> CommandBuffers;

    std::vector<VkSemaphore> ImageAvailableSemaphores;
    std::vector<VkSemaphore> RenderFinishedSemaphores;
    std::vector<VkFence> InFlightFences;

    bool FramebufferResized = false;

    const uint32_t Width{800};
    const uint32_t Height{600};

    const int MAX_FRAMES_IN_FLIGHT = 2;

    uint32_t CurrentFrame = 0;

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

        Window = glfwCreateWindow(Width, Height, "Vulkan", nullptr, nullptr);

        glfwSetWindowUserPointer(Window, this);
        glfwSetFramebufferSizeCallback(Window, FramebufferResizeCallback);
    }

    static void FramebufferResizeCallback(GLFWwindow* InWindow, int InWidth, int InHeight)
    {
        auto App = reinterpret_cast<CHelloTriangleApplication*>(
            glfwGetWindowUserPointer(InWindow)
        );
        App->FramebufferResized = true;
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
        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();
        CreateCommandPool();
        CreateColorResources();
        CreateDepthResources();
        CreateFramebuffers();
        
        CreateTextureImage();
        CreateTextureImageView();
        CreateTextureSampler();

        LoadModel();

        CreateVertexBuffer();
        CreateIndexBuffer();
        
        CreateUniformBuffers();
        CreateDescriptorPool();
        CreateDescriptorSets();
        CreateCommandBuffers();
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

    void CleanupSwapChain()
    {
        vkDestroyImageView(Device, DepthImageView, nullptr);
        vkDestroyImage(Device, DepthImage, nullptr);
        vkFreeMemory(Device, DepthImageMemory, nullptr);
        
        vkDestroyImageView(Device, ColorImageView, nullptr);
        vkDestroyImage(Device, ColorImage, nullptr);
        vkFreeMemory(Device, ColorImageMemory, nullptr);

        for (auto Framebuffer : SwapChainFramebuffers)
        {
            vkDestroyFramebuffer(Device, Framebuffer, nullptr);
        }

        for (auto ImageView : SwapChainImageViews)
        {
            vkDestroyImageView(Device, ImageView, nullptr);
        }

        vkDestroySwapchainKHR(Device, SwapChain, nullptr);
    }

    void Cleanup()
    {
        CleanupSwapChain();

        vkDestroyPipeline(Device, GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);
        vkDestroyRenderPass(Device, RenderPass, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroyBuffer(Device, UniformBuffers[i], nullptr);
            vkFreeMemory(Device, UniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(Device, DescriptorPool, nullptr);

        vkDestroySampler(Device, TextureSampler, nullptr);
        vkDestroyImageView(Device, TextureImageView, nullptr);

        vkDestroyImage(Device, TextureImage, nullptr);
        vkFreeMemory(Device, TextureImageMemory, nullptr);

        vkDestroyDescriptorSetLayout(
            Device,
            DescriptorSetLayout,
            nullptr
        );

        vkDestroyBuffer(Device, IndexBuffer, nullptr);
        vkFreeMemory(Device, IndexBufferMemory, nullptr);

        vkDestroyBuffer(Device, VertexBuffer, nullptr);
        vkFreeMemory(Device, VertexBufferMemory, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(Device, RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(Device, ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(Device, InFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(Device, CommandPool, nullptr);
        vkDestroyDevice(Device, nullptr);

        if (EnableValidationLayers)
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

    void RecreateSwapChain()
    {
        int TempWidth = 0, TempHeight = 0;
        glfwGetFramebufferSize(Window, &TempWidth, &TempHeight);
        while(TempWidth == 0 || TempHeight == 0)
        {
            glfwGetFramebufferSize(Window, &TempWidth, &TempHeight);
            glfwWaitEvents();
        }
        
        vkDeviceWaitIdle(Device);

        CleanupSwapChain();

        CreateSwapChain();
        CreateImageViews();
        CreateColorResources();
        CreateDepthResources();
        CreateFramebuffers();
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
        // InCreateInfo.pUserData = nullptr;
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

        if (DeviceCount == 0)
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> Devices(DeviceCount);
        vkEnumeratePhysicalDevices(Instance, &DeviceCount, Devices.data());

        for (const auto& Device : Devices)
        {
            if (IsDeviceSuitable(Device))
            {
                PhysicalDevice = Device;
                MsaaSamples = GetMaxUsableSampleCount();
                break;
            }
        }

        if (PhysicalDevice == VK_NULL_HANDLE)
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
        } 
        else 
        {
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
        } 
        else 
        {
            CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        CreateInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
        CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        CreateInfo.presentMode = PresentMode;
        CreateInfo.clipped = VK_TRUE;

        // CreateInfo.oldSwapchain = VK_NULL_HANDLE;

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
            SwapChainImageViews[i] = CreateImageView(
                SwapChainImages[i], 
                SwapChainImageFormat,
                VK_IMAGE_ASPECT_COLOR_BIT,
                1
            );
        }
    }

    void CreateRenderPass()
    {
        VkAttachmentDescription ColorAttachment{};
        ColorAttachment.format = SwapChainImageFormat;
        ColorAttachment.samples = MsaaSamples;
        ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription DepthAttachment{};
        DepthAttachment.format = FindDepthFormat();
        DepthAttachment.samples = MsaaSamples;
        DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription ColorAttachmentResolve{};
        ColorAttachmentResolve.format = SwapChainImageFormat;
        ColorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT; // no multisampling
        ColorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        ColorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        ColorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        ColorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        ColorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ColorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // for presentation

        VkAttachmentReference ColorAttachmentRef{};
        ColorAttachmentRef.attachment = 0;
        ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference DepthAttachmentRef{};
        DepthAttachmentRef.attachment = 1;
        DepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference ColorAttachmentResolveRef{};
        ColorAttachmentResolveRef.attachment = 2;
        ColorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription Subpass{};
        Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        Subpass.colorAttachmentCount = 1;
        Subpass.pColorAttachments = &ColorAttachmentRef;
        Subpass.pDepthStencilAttachment = &DepthAttachmentRef;
        Subpass.pResolveAttachments = &ColorAttachmentResolveRef;

        VkSubpassDependency Dependency{};
        Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        Dependency.dstSubpass = 0;
        Dependency.srcStageMask = 
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        Dependency.srcAccessMask = 0;
        Dependency.dstStageMask = 
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        Dependency.dstAccessMask = 
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 3> Attachments = {
            ColorAttachment, DepthAttachment, ColorAttachmentResolve
        };
        VkRenderPassCreateInfo RenderPassInfo{};
        RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        RenderPassInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
        RenderPassInfo.pAttachments = Attachments.data();
        RenderPassInfo.subpassCount = 1;
        RenderPassInfo.pSubpasses = &Subpass;
        RenderPassInfo.dependencyCount = 1;
        RenderPassInfo.pDependencies = &Dependency;

        if (vkCreateRenderPass(Device, &RenderPassInfo, nullptr, &RenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass!");
        }
    }

    void CreateDescriptorSetLayout()
   {
        VkDescriptorSetLayoutBinding UboLayoutBinding{};
        UboLayoutBinding.binding = 0;
        UboLayoutBinding.descriptorCount = 1;
        UboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        UboLayoutBinding.pImmutableSamplers = nullptr;
        UboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding SamplerLayoutBinding{};
        SamplerLayoutBinding.binding = 1;
        SamplerLayoutBinding.descriptorCount = 1;
        SamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        SamplerLayoutBinding.pImmutableSamplers = nullptr;
        SamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> Binding = {
            UboLayoutBinding, SamplerLayoutBinding
        };

        VkDescriptorSetLayoutCreateInfo LayoutInfo{};
        LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        LayoutInfo.bindingCount = static_cast<uint32_t>(Binding.size());
        LayoutInfo.pBindings = Binding.data();

        if (vkCreateDescriptorSetLayout(
                Device, &LayoutInfo, nullptr, &DescriptorSetLayout
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }
   }

    void CreateGraphicsPipeline()
    {
        auto VertShaderCode = VulkanUtils::ReadFile(
            std::string("WVulkanTest_Content/Shaders/ShaderBase.vert.spv")
        );
        auto FragShaderCode = VulkanUtils::ReadFile(
            std::string("WVulkanTest_Content/Shaders/ShaderBase.frag.spv")
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

        auto BindingDescription = Vertex::GetBindingDescription();
        auto AttributeDescriptions = Vertex::GetAttributeDescriptions();

        VertexInputInfo.vertexBindingDescriptionCount = 1;
        VertexInputInfo.vertexAttributeDescriptionCount = 
            static_cast<uint32_t>(AttributeDescriptions.size());
        VertexInputInfo.pVertexBindingDescriptions = &BindingDescription;
        VertexInputInfo.pVertexAttributeDescriptions = AttributeDescriptions.data();

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
        Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // because of y-flip in the projection matrix
        Rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo Multisampling{};
        Multisampling.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        Multisampling.sampleShadingEnable = VK_FALSE;
        Multisampling.rasterizationSamples = MsaaSamples;

        VkPipelineDepthStencilStateCreateInfo DepthStencil{};
        DepthStencil.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
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
        ColorBlending.blendConstants[0] = 0.0f;
        ColorBlending.blendConstants[1] = 0.0f;
        ColorBlending.blendConstants[2] = 0.0f;
        ColorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> DynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo DynamicState{};
        DynamicState.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        DynamicState.dynamicStateCount = static_cast<uint32_t>(DynamicStates.size());
        DynamicState.pDynamicStates = DynamicStates.data();

        VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
        PipelineLayoutInfo.sType = 
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        PipelineLayoutInfo.setLayoutCount = 1;
        PipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;

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
        PipelineInfo.pDepthStencilState = &DepthStencil;
        PipelineInfo.pColorBlendState = &ColorBlending;
        PipelineInfo.pDynamicState = &DynamicState;
        PipelineInfo.layout = PipelineLayout;
        PipelineInfo.renderPass = RenderPass;
        PipelineInfo.subpass = 0;
        PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(
            Device, 
            VK_NULL_HANDLE, 
            1, 
            &PipelineInfo, 
            nullptr, 
            &GraphicsPipeline) != VK_SUCCESS
        )
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
            std::array<VkImageView, 3> Attachments = {
                ColorImageView,
                DepthImageView,
                SwapChainImageViews[i]
            };

            VkFramebufferCreateInfo FramebufferInfo{};
            FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            FramebufferInfo.renderPass = RenderPass;
            FramebufferInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
            FramebufferInfo.pAttachments = Attachments.data();
            FramebufferInfo.width = SwapChainExtent.width;
            FramebufferInfo.height = SwapChainExtent.height;
            FramebufferInfo.layers = 1;

            if (vkCreateFramebuffer(
                Device, 
                &FramebufferInfo, 
                nullptr, 
                &SwapChainFramebuffers[i]) != VK_SUCCESS
            )
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

    void CreateColorResources()
    {
        VkFormat ColorFormat = SwapChainImageFormat;

        CreateImage(
            SwapChainExtent.width,
            SwapChainExtent.height,
            1,
            MsaaSamples,
            ColorFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            ColorImage,
            ColorImageMemory
        );
        
        ColorImageView = CreateImageView(
            ColorImage,
            ColorFormat,
            VK_IMAGE_ASPECT_COLOR_BIT,
            1
        );
    }

    void CreateDepthResources()
    {
        VkFormat DepthFormat = FindDepthFormat();

        CreateImage(
            SwapChainExtent.width, 
            SwapChainExtent.height, 
            1,
            MsaaSamples,
            DepthFormat, 
            VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            DepthImage,
            DepthImageMemory
        );

        DepthImageView = CreateImageView(
            DepthImage, 
            DepthFormat,
            VK_IMAGE_ASPECT_DEPTH_BIT,
            1
        );
    }

    VkFormat FindSupportedFormat(
        const std::vector<VkFormat>& InCandidates,
        VkImageTiling InTilling,
        VkFormatFeatureFlags InFeatures
    )
    {
        for (VkFormat Format : InCandidates)
        {
            VkFormatProperties Props;
            vkGetPhysicalDeviceFormatProperties(
                PhysicalDevice, Format, &Props
            );

            if (
                InTilling == VK_IMAGE_TILING_LINEAR &&
                (Props.linearTilingFeatures & InFeatures) == InFeatures
            )
            {
                return Format;
            }
            else if (
                InTilling == VK_IMAGE_TILING_OPTIMAL &&
                (Props.optimalTilingFeatures & InFeatures) == InFeatures
            )
            {
                return Format;
            }
        }
        throw std::runtime_error("Failed to find supported format!");
    }

    VkFormat FindDepthFormat()
    {
        return FindSupportedFormat(
            {
                VK_FORMAT_D32_SFLOAT, 
                VK_FORMAT_D32_SFLOAT_S8_UINT,  // 32 bit depth + 8 bit stencil
                VK_FORMAT_D24_UNORM_S8_UINT
            },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    void CreateTextureImage()
    {
        int TexWidth, TexHeight, TexChannels;
        stbi_uc* Pixels = stbi_load(
            AssetConfig::TEXTURE_PATH.c_str(),
            &TexWidth, 
            &TexHeight, 
            &TexChannels, 
            STBI_rgb_alpha
        );
        VkDeviceSize ImageSize = TexWidth * TexHeight * 4;
        MipLevels = static_cast<uint32_t>(
            std::floor(std::log2(std::max(TexWidth, TexHeight))) + 1
        );

        if (!Pixels)
        {
            throw std::runtime_error("Failed to load texture image!");
        }

        VkBuffer StagingBuffer;
        VkDeviceMemory StagingBufferMemory;
        CreateBuffer(
            ImageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            StagingBuffer,
            StagingBufferMemory
        );
        
        void* Data;
        vkMapMemory(Device, StagingBufferMemory, 0, ImageSize, 0, &Data);
            memcpy(Data, Pixels, static_cast<size_t>(ImageSize));
        vkUnmapMemory(Device, StagingBufferMemory);

        stbi_image_free(Pixels);

        CreateImage(
            TexWidth, 
            TexHeight, 
            MipLevels,
            VK_SAMPLE_COUNT_1_BIT,
            VK_FORMAT_R8G8B8A8_SRGB, 
            VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            TextureImage,
            TextureImageMemory
        );

        TransitionImageLayout(
            TextureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            MipLevels
        );
        
        CopyBufferToImage(
            StagingBuffer,
            TextureImage,
            static_cast<uint32_t>(TexWidth),
            static_cast<uint32_t>(TexHeight)
        );

        vkDestroyBuffer(Device, StagingBuffer, nullptr);
        vkFreeMemory(Device, StagingBufferMemory, nullptr);

        GenerateMipmaps(
            TextureImage, 
            VK_FORMAT_R8G8B8A8_SRGB, 
            TexWidth, 
            TexHeight, 
            MipLevels
        );
    }

    void GenerateMipmaps(
        VkImage InImage, 
        VkFormat InImageFormat, 
        int32_t InTexWidth, 
        int32_t InTexHeight, 
        uint32_t InMipLevels
    )
    {
        // mip map generation should be pregenerated
        VkFormatProperties FormatProperties;
        vkGetPhysicalDeviceFormatProperties(
            PhysicalDevice,
            InImageFormat,
            &FormatProperties
        );

        if (!(FormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error("Texture image format does not support linear blitting!");
        }

        VkCommandBuffer CommandBuffer = BeginSingleTimeCommands();

        VkImageMemoryBarrier Barrier{};
        Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        Barrier.image = InImage;
        Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        Barrier.subresourceRange.baseArrayLayer = 0;
        Barrier.subresourceRange.layerCount = 1;
        Barrier.subresourceRange.levelCount = 1;

        int32_t MipWidth = InTexWidth;
        int32_t MipHeight = InTexHeight;

        for (uint32_t i = 1; i < MipLevels; i++)
        {
            Barrier.subresourceRange.baseMipLevel = i - 1;
            Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            Barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            Barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(
                CommandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &Barrier
            );

            VkImageBlit Blit{};
            Blit.srcOffsets[0] = {0, 0, 0};
            Blit.srcOffsets[1] = {MipWidth, MipHeight, 1};
            Blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            Blit.srcSubresource.mipLevel = i - 1;
            Blit.srcSubresource.baseArrayLayer = 0;
            Blit.srcSubresource.layerCount = 1;
            Blit.dstOffsets[0] = {0, 0, 0};
            Blit.dstOffsets[1] = {
                MipWidth > 1 ? MipWidth / 2 : 1,
                MipHeight > 1 ? MipHeight / 2 : 1,
                1
            };
            Blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            Blit.dstSubresource.mipLevel = i;
            Blit.dstSubresource.baseArrayLayer = 0;
            Blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(
                CommandBuffer,
                InImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                InImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &Blit,
                VK_FILTER_LINEAR
            );

            Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            Barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            Barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(
                CommandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &Barrier
            );

            if (MipWidth > 1) MipWidth /= 2;
            if (MipHeight > 1) MipHeight /= 2;
        }

        Barrier.subresourceRange.baseMipLevel = MipLevels - 1;
        Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        Barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            CommandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &Barrier
        );

        EndSingleTimeCommands(CommandBuffer);
    }

    VkSampleCountFlagBits GetMaxUsableSampleCount()
    {
        VkPhysicalDeviceProperties PhysicalDeviceProperties;
        vkGetPhysicalDeviceProperties(
            PhysicalDevice, 
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

    void CreateTextureImageView()
    {
        TextureImageView = CreateImageView(
            TextureImage, 
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_ASPECT_COLOR_BIT,
            MipLevels
        );
    }

    void CreateTextureSampler()
    {
        VkPhysicalDeviceProperties Properties{};
        vkGetPhysicalDeviceProperties(PhysicalDevice, &Properties);

        VkSamplerCreateInfo SamplerInfo{};
        SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        SamplerInfo.magFilter = VK_FILTER_LINEAR;
        SamplerInfo.minFilter = VK_FILTER_LINEAR;
        SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        SamplerInfo.anisotropyEnable = VK_TRUE;
        SamplerInfo.maxAnisotropy = Properties.limits.maxSamplerAnisotropy;
        SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        SamplerInfo.unnormalizedCoordinates = VK_FALSE;
        SamplerInfo.compareEnable = VK_FALSE;
        SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        SamplerInfo.minLod = 0.0f;
        SamplerInfo.maxLod = static_cast<float>(MipLevels);
        SamplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(Device, &SamplerInfo, nullptr, &TextureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create texture sampler!");
        }
    }

    VkImageView CreateImageView(
        VkImage InImage, 
        VkFormat InFormat, 
        VkImageAspectFlags InAspectFlags,
        uint32_t InMipLevels
    )
    {
        VkImageViewCreateInfo ViewInfo{};
        ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ViewInfo.image = InImage;
        ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ViewInfo.format = InFormat;
        ViewInfo.subresourceRange.aspectMask = InAspectFlags;
        ViewInfo.subresourceRange.baseMipLevel = 0;
        ViewInfo.subresourceRange.levelCount = InMipLevels;
        ViewInfo.subresourceRange.baseArrayLayer = 0;
        ViewInfo.subresourceRange.layerCount = 1;

        VkImageView ImageView;
        if (vkCreateImageView(Device, &ViewInfo, nullptr, &ImageView) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create texture image view!"
            );
        }

        return ImageView;
    }

    void CreateImage(
        uint32_t InWidth, uint32_t InHeight, 
        uint32_t InMipLevels,
        VkSampleCountFlagBits InNumSamples,
        VkFormat InFormat, 
        VkImageTiling InTiling, 
        VkImageUsageFlags InUsage,
        VkMemoryPropertyFlags InProperties,
        VkImage& OutImage,
        VkDeviceMemory& OutImageMemory
    )
    {
        VkImageCreateInfo ImageInfo{};
        ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ImageInfo.imageType = VK_IMAGE_TYPE_2D;
        ImageInfo.extent.width = InWidth;
        ImageInfo.extent.height = InHeight;
        ImageInfo.extent.depth = 1;
        ImageInfo.mipLevels = InMipLevels;
        ImageInfo.arrayLayers = 1;
        ImageInfo.format = InFormat;
        ImageInfo.tiling = InTiling;
        ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ImageInfo.usage = InUsage;
        ImageInfo.samples = InNumSamples;
        ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(Device, &ImageInfo, nullptr, &OutImage) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image!");
        }

        VkMemoryRequirements MemRequirements;

        vkGetImageMemoryRequirements(Device, OutImage, &MemRequirements);

        VkMemoryAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        AllocInfo.allocationSize = MemRequirements.size;
        AllocInfo.memoryTypeIndex = FindMemoryType(
            MemRequirements.memoryTypeBits,
            InProperties
        );

        if (vkAllocateMemory(Device, &AllocInfo, nullptr, &OutImageMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate image memory!");
        }

        vkBindImageMemory(Device, OutImage, OutImageMemory, 0);
    }

    void TransitionImageLayout(
        VkImage InImage,
        VkFormat InFormat,
        VkImageLayout InOldLayout,
        VkImageLayout InNewLayout,
        uint32_t InMipLevels
    )
    {
        VkCommandBuffer CommandBuffer = BeginSingleTimeCommands();

        VkImageMemoryBarrier Barrier{};
        Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        Barrier.oldLayout = InOldLayout;
        Barrier.newLayout = InNewLayout;
        Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        Barrier.image = InImage;
        Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        Barrier.subresourceRange.baseMipLevel = 0;
        Barrier.subresourceRange.levelCount = InMipLevels;
        Barrier.subresourceRange.baseArrayLayer = 0;
        Barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags SourceStage;
        VkPipelineStageFlags DestinationStage;

        if (
            InOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && 
            InNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 
        )
        {
            Barrier.srcAccessMask = 0;
            Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (
            InOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            InNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        )
        {
            Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            DestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("Unsupported layout transition!");
        }
        
        vkCmdPipelineBarrier(
            CommandBuffer,
            SourceStage, DestinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &Barrier
        );

        EndSingleTimeCommands(CommandBuffer);
    }

    void CopyBufferToImage(VkBuffer InBuffer, VkImage InImage, uint32_t InWidth, uint32_t InHeight)
    {
        VkCommandBuffer CommandBuffer = BeginSingleTimeCommands();

        VkBufferImageCopy Region{};
        Region.bufferOffset = 0;
        Region.bufferRowLength = 0;
        Region.bufferImageHeight = 0;

        Region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        Region.imageSubresource.mipLevel = 0;
        Region.imageSubresource.baseArrayLayer = 0;
        Region.imageSubresource.layerCount = 1;

        Region.imageOffset = {0, 0, 0};
        Region.imageExtent = {
            InWidth,
            InHeight,
            1
        };

        vkCmdCopyBufferToImage(
            CommandBuffer,
            InBuffer,
            InImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &Region
        );

        EndSingleTimeCommands(CommandBuffer);
    }

    void LoadModel()
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> Shapes;
        std::vector<tinyobj::material_t> Materials;
        std::string Warning, Error;

        if (!tinyobj::LoadObj(
                &attrib,
                &Shapes,
                &Materials,
                &Warning,
                &Error,
                AssetConfig::MODEL_PATH.c_str()
            )
        )
        {
            throw std::runtime_error(Warning + Error);
        }

        std::unordered_map<Vertex, uint32_t> UniqueVertices{};
        for (const auto& Shape : Shapes)
        {
            for (const auto& Index : Shape.mesh.indices)
            {
                Vertex Vertex{};

                Vertex.Position = {
                    attrib.vertices[3 * Index.vertex_index + 0],
                    attrib.vertices[3 * Index.vertex_index + 1],
                    attrib.vertices[3 * Index.vertex_index + 2]
                };

                Vertex.TexCoord = {
                    attrib.texcoords[2 * Index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * Index.texcoord_index + 1]
                };

                Vertex.Color = {1.0f, 1.0f, 1.0f};

                if (UniqueVertices.count(Vertex) == 0)
                {                    
                    UniqueVertices[Vertex] = static_cast<uint32_t>(Mesh.Vertices.size());
                    Mesh.Vertices.push_back(Vertex);
                }

                Mesh.Indices.push_back(UniqueVertices[Vertex]);
            }
        }
    }

    void CreateVertexBuffer()
    {
        VkDeviceSize BufferSize = sizeof(Mesh.Vertices[0]) * Mesh.Vertices.size();
        
        VkBuffer StagingBuffer;
        VkDeviceMemory StagingBufferMemory;
        CreateBuffer(
            BufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            StagingBuffer,
            StagingBufferMemory
        );

        void* Data;
        vkMapMemory(Device, StagingBufferMemory, 0, BufferSize, 0, &Data);
            memcpy(Data, Mesh.Vertices.data(), (size_t) BufferSize);
        vkUnmapMemory(Device, StagingBufferMemory);

        CreateBuffer(
            BufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            VertexBuffer,
            VertexBufferMemory
        );

        CopyBuffer(StagingBuffer, VertexBuffer, BufferSize);

        vkDestroyBuffer(Device, StagingBuffer, nullptr);
        vkFreeMemory(Device, StagingBufferMemory, nullptr);
    }

    void CreateIndexBuffer()
    {
        VkDeviceSize BufferSize = sizeof(Mesh.Indices[0]) * Mesh.Indices.size();

        VkBuffer StagingBuffer;
        VkDeviceMemory StagingBufferMemory;
        CreateBuffer(
            BufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            StagingBuffer,
            StagingBufferMemory
        );

        void* Data;

        vkMapMemory(Device, StagingBufferMemory, 0, BufferSize, 0, &Data);
            memcpy(Data, Mesh.Indices.data(), (size_t) BufferSize);
        vkUnmapMemory(Device, StagingBufferMemory);

        CreateBuffer(
            BufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            IndexBuffer,
            IndexBufferMemory
        );

        CopyBuffer(StagingBuffer, IndexBuffer, BufferSize);

        vkDestroyBuffer(Device, StagingBuffer, nullptr);
        vkFreeMemory(Device, StagingBufferMemory, nullptr);        
    }

    void CreateUniformBuffers()
    {
        VkDeviceSize BufferSize = sizeof(SUniformBufferObject);
        
        UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        UniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            CreateBuffer(
                BufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                UniformBuffers[i],
                UniformBuffersMemory[i]
            );
            vkMapMemory(
                Device, 
                UniformBuffersMemory[i],
                0,
                BufferSize,
                0, 
                &UniformBuffersMapped[i]
            );
        }
    }

    void CreateDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 2> PoolSizes{};
        PoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        PoolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        PoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        PoolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo PoolInfo{};
        PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        PoolInfo.poolSizeCount = static_cast<uint32_t>(PoolSizes.size());
        PoolInfo.pPoolSizes = PoolSizes.data();
        PoolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if(vkCreateDescriptorPool(
                Device,
                &PoolInfo,
                nullptr,
                &DescriptorPool
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error(
                "Failed to create descriptor pool!"
            );
        }
    }

    void CreateDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> Layouts(
            MAX_FRAMES_IN_FLIGHT, 
            DescriptorSetLayout
        );
        VkDescriptorSetAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        AllocInfo.descriptorPool = DescriptorPool;
        AllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        AllocInfo.pSetLayouts = Layouts.data();

        DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(
                Device,
                &AllocInfo,
                DescriptorSets.data()
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error(
                "Failed to allocate descriptor sets!"
            );
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo BufferInfo{};
            BufferInfo.buffer = UniformBuffers[i];
            BufferInfo.offset = 0;
            BufferInfo.range = sizeof(SUniformBufferObject);

            VkDescriptorImageInfo ImageInfo{};
            ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            ImageInfo.imageView = TextureImageView;
            ImageInfo.sampler = TextureSampler;

            std::array<VkWriteDescriptorSet, 2> DescriptorWrites{};

            DescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            DescriptorWrites[0].dstSet = DescriptorSets[i];
            DescriptorWrites[0].dstBinding = 0;
            DescriptorWrites[0].dstArrayElement = 0;
            DescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            DescriptorWrites[0].descriptorCount = 1;
            DescriptorWrites[0].pBufferInfo = &BufferInfo;

            DescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            DescriptorWrites[1].dstSet = DescriptorSets[i];
            DescriptorWrites[1].dstBinding = 1;
            DescriptorWrites[1].dstArrayElement = 0;
            DescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            DescriptorWrites[1].descriptorCount = 1;
            DescriptorWrites[1].pImageInfo = &ImageInfo;

            vkUpdateDescriptorSets(
                Device,
                static_cast<uint32_t>(DescriptorWrites.size()),
                DescriptorWrites.data(),
                0,
                nullptr
            );
        }
    }

    void CreateBuffer(
        VkDeviceSize InSize, 
        VkBufferUsageFlags InUsage, 
        VkMemoryPropertyFlags InProperties,
        VkBuffer& InBuffer,
        VkDeviceMemory& InBufferMemory
    )
    {
        VkBufferCreateInfo BufferInfo{};
        BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferInfo.size = InSize;
        BufferInfo.usage = InUsage;
        BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(Device, &BufferInfo, nullptr, &InBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex buffer!");
        }

        VkMemoryRequirements MemRequirements;
        vkGetBufferMemoryRequirements(Device, InBuffer, &MemRequirements);

        VkMemoryAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        AllocInfo.allocationSize = MemRequirements.size;
        AllocInfo.memoryTypeIndex = FindMemoryType(
            MemRequirements.memoryTypeBits, 
            InProperties
        );

        if (vkAllocateMemory(Device, &AllocInfo, nullptr, &InBufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(Device, InBuffer, InBufferMemory, 0);
    }

    VkCommandBuffer BeginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        AllocInfo.commandPool = CommandPool;
        AllocInfo.commandBufferCount = 1;

        VkCommandBuffer CommandBuffer;
        vkAllocateCommandBuffers(Device, &AllocInfo, &CommandBuffer);

        VkCommandBufferBeginInfo BeginInfo{};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

        return CommandBuffer;
    }

    void EndSingleTimeCommands(VkCommandBuffer InCommandBuffer)
    {
        vkEndCommandBuffer(InCommandBuffer);

        VkSubmitInfo SubmitInfo{};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &InCommandBuffer;

        vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(GraphicsQueue);

        vkFreeCommandBuffers(Device, CommandPool, 1, &InCommandBuffer);
    }

    void CopyBuffer(VkBuffer InSrcBuffer, VkBuffer DstBuffer, VkDeviceSize InSize)
    {
        VkCommandBufferAllocateInfo AllocInfo;
        AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        AllocInfo.commandPool = CommandPool;
        AllocInfo.commandBufferCount = 1;
        AllocInfo.pNext = nullptr;

        VkCommandBuffer CommandBuffer;
        vkAllocateCommandBuffers(Device, &AllocInfo, &CommandBuffer);

        VkCommandBufferBeginInfo BeginInfo{};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

            VkBufferCopy CopyRegion{};
            CopyRegion.size = InSize;
            vkCmdCopyBuffer(CommandBuffer, InSrcBuffer, DstBuffer, 1, &CopyRegion);
        
        vkEndCommandBuffer(CommandBuffer);

        VkSubmitInfo SubmitInfo{};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &CommandBuffer;

        vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(GraphicsQueue);

        vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
    }

    uint32_t FindMemoryType(uint32_t InTypeFilter, VkMemoryPropertyFlags InProperties)
    {
        VkPhysicalDeviceMemoryProperties MemProperties;
        vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &MemProperties);

        for (uint32_t i = 0; i < MemProperties.memoryTypeCount; i++)
        {
            if (
                (InTypeFilter & (1 << i)) && 
                (MemProperties.memoryTypes[i].propertyFlags & InProperties) == InProperties
            )
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type!");
    }

    void CreateCommandBuffers()
    {
        CommandBuffers.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocInfo.commandPool = CommandPool;
        AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        AllocInfo.commandBufferCount = (uint32_t)CommandBuffers.size();

        if (vkAllocateCommandBuffers(Device, &AllocInfo, CommandBuffers.data()) != VK_SUCCESS)
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

        std::array<VkClearValue, 2> ClearValues{};
        ClearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        ClearValues[1].depthStencil = {1.0f, 0};
        
        RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
        RenderPassInfo.pClearValues = ClearValues.data();

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

            VkBuffer VertexBufers[] = {VertexBuffer};
            VkDeviceSize offsets[] = {0};

            vkCmdBindVertexBuffers(InCommandBuffer, 0, 1, VertexBufers, offsets);
            vkCmdBindIndexBuffer(InCommandBuffer, IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(
                InCommandBuffer, 
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                PipelineLayout,
                0,
                1,
                &DescriptorSets[CurrentFrame],
                0,
                nullptr
            );

            vkCmdDrawIndexed(
                InCommandBuffer, 
                static_cast<uint32_t>(Mesh.Indices.size()), 
                1, 
                0, 
                0, 
                0
            );

        vkCmdEndRenderPass(InCommandBuffer);

        if (vkEndCommandBuffer(InCommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer!");
        }
    }

    void CreateSyncObjects()
    {
        ImageAvailableSemaphores.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
        RenderFinishedSemaphores.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
        InFlightFences.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo SemaphoreInfo{};
        SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo FenceInfo{};
        FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (
                vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &ImageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &RenderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(Device, &FenceInfo, nullptr, &InFlightFences[i]) != VK_SUCCESS
            )
            {
                throw std::runtime_error("Failed to create synchronization objects for a frame!");
            }
        }
    }

    void UpdateUniformBuffer(uint32_t CurrentImage)
    {
        static auto StartTime = std::chrono::high_resolution_clock::now();
        
        auto CurrentTime = std::chrono::high_resolution_clock::now();
        float Time = std::chrono::duration<float, std::chrono::seconds::period>(
            CurrentTime - StartTime
        ).count();

        SUniformBufferObject Ubo{};
        Ubo.model = glm::rotate(
            glm::mat4(1.f), 
            Time * glm::radians(90.f), 
            glm::vec3(0.f, 0.f, 1.f)
        );
        Ubo.view = glm::lookAt(
            glm::vec3(2.f, 2.f, 2.f), 
            glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(0.f, 0.f, 1.f)
        );
        Ubo.proj = glm::perspective(
            glm::radians(45.f),
            SwapChainExtent.width / (float) SwapChainExtent.height,
            1.f,
            10.f
        );

        Ubo.proj[1][1] *= -1;  // Fix OpenGL Y inversion

        memcpy(UniformBuffersMapped[CurrentImage], &Ubo, sizeof(Ubo));
    }

    void DrawFrame()
    {
        vkWaitForFences(Device, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);

        uint32_t ImageIndex;
        VkResult Result = vkAcquireNextImageKHR(
            Device, 
            SwapChain, 
            UINT64_MAX, 
            ImageAvailableSemaphores[CurrentFrame], 
            VK_NULL_HANDLE, 
            &ImageIndex
        );

        if (Result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return;
        }
        else if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }

        UpdateUniformBuffer(CurrentFrame);

        vkResetFences(Device, 1, &InFlightFences[CurrentFrame]);

        vkResetCommandBuffer(CommandBuffers[CurrentFrame], 0);
        RecordCommandBuffer(CommandBuffers[CurrentFrame], ImageIndex);

        VkSubmitInfo SubmitInfo{};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore WaitSemaphores[] = {ImageAvailableSemaphores[CurrentFrame]};
        VkPipelineStageFlags WaitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        SubmitInfo.waitSemaphoreCount = 1;
        SubmitInfo.pWaitSemaphores = WaitSemaphores;
        SubmitInfo.pWaitDstStageMask = WaitStages;

        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &CommandBuffers[CurrentFrame];

        VkSemaphore SignalSemaphores[] = {RenderFinishedSemaphores[CurrentFrame]};
        SubmitInfo.signalSemaphoreCount = 1;
        SubmitInfo.pSignalSemaphores = SignalSemaphores;

        if (vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, InFlightFences[CurrentFrame]) != VK_SUCCESS)
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

        Result = vkQueuePresentKHR(PresentQueue, &PresentInfo);

        if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || FramebufferResized)
        {
            FramebufferResized = false;
            RecreateSwapChain();
        }
        else if (Result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swap chain image!");
        }

        CurrentFrame = (CurrentFrame + 1) % VulkanConfig::MAX_FRAMES_IN_FLIGHT;
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

            ActualExtent.width = std::clamp(
                ActualExtent.width, 
                InCapabilities.minImageExtent.width, 
                InCapabilities.maxImageExtent.width
            );
            ActualExtent.height = std::clamp(
                ActualExtent.height, 
                InCapabilities.minImageExtent.height, 
                InCapabilities.maxImageExtent.height
            ); 

            return ActualExtent;
        }
    }

    VulkanUtils::SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice InDevice)
    {
        VulkanUtils::SwapChainSupportDetails Details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(InDevice, Surface, &Details.Capabilities);

        uint32_t FormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(InDevice, Surface, &FormatCount, nullptr);

        if (FormatCount != 0)
        {
            Details.Formats.resize(FormatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                InDevice, 
                Surface, 
                &FormatCount, 
                Details.Formats.data()
            );
        }

        uint32_t PresentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            InDevice, 
            Surface, 
            &PresentModeCount, 
            nullptr
        );

        if (PresentModeCount != 0)
        {
            Details.PresentModes.resize(PresentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                InDevice, 
                Surface, 
                &PresentModeCount, 
                Details.PresentModes.data()
            );
        }

        return Details;
    }

    bool IsDeviceSuitable(VkPhysicalDevice InDevice)
    {
        VulkanUtils::QueueFamilyIndices Indices = FindQueueFamilies(InDevice);

        bool ExtensionsSupported = CheckDeviceExtensionSupport(InDevice);

        bool SwapChainAdequate = false;
        if(ExtensionsSupported)
        {
            VulkanUtils::SwapChainSupportDetails SwapChainSupport = 
                QuerySwapChainSupport(InDevice);
            SwapChainAdequate = 
                !SwapChainSupport.Formats.empty() && !SwapChainSupport.PresentModes.empty();
        }

        VkPhysicalDeviceFeatures SupportedFeatures;
        vkGetPhysicalDeviceFeatures(InDevice, &SupportedFeatures);

        return Indices.IsComplete() && 
            ExtensionsSupported && 
            SwapChainAdequate && 
            SupportedFeatures.samplerAnisotropy;
    }

    bool CheckDeviceExtensionSupport(VkPhysicalDevice InDevice)
    {
        uint32_t ExtensionCount;
        vkEnumerateDeviceExtensionProperties(
            InDevice, 
            nullptr, 
            &ExtensionCount, 
            nullptr
        );

        std::vector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
        vkEnumerateDeviceExtensionProperties(
            InDevice, 
            nullptr, 
            &ExtensionCount, 
            AvailableExtensions.data()
        );

        std::set<std::string> RequiredExtensions(
            DeviceExtensions.begin(), 
            DeviceExtensions.end()
        );

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
        vkGetPhysicalDeviceQueueFamilyProperties(
            InDevice, &QueueFamilyCount, nullptr
        );

        std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            InDevice, &QueueFamilyCount, QueueFamilies.data()
        );

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

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT InMessageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT InMessageType,
        const VkDebugUtilsMessengerCallbackDataEXT* InCallbackData,
        void* InUserData
    )
    {
        std::cerr << "Validation layer: " << InCallbackData->pMessage << std::endl;
        return VK_FALSE;
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
