#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <optional>
#include <set>
#include <random>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const uint32_t PARTICLE_COUNT = 8192;

const int MAX_FRAMES_IN_FLIGHT = 2;

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

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger
)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
        instance, 
        "vkCreateDebugUtilsMessengerEXT"
    );
    if (func != nullptr)
    {
        return func(
            instance, 
            pCreateInfo, 
            pAllocator, 
            pDebugMessenger
        );
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance Instance,
    VkDebugUtilsMessengerEXT DebugMessenger,
    const VkAllocationCallbacks* pAllocator
)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
        Instance,
        "vkDestroyDebugUtilsMessengerEXT"
    );
    if (func != nullptr)
    {
        func(
            Instance,
            DebugMessenger,
            pAllocator
        );
    }
}

struct QueueFamilyIndices
{
    std::optional<uint32_t> GraphicsAndComputeFamily;
    std::optional<uint32_t> PresentFamily;

    bool IsComplete()
    {
        return GraphicsAndComputeFamily.has_value() && PresentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

struct UniformBufferObject
{
    float DeltaTime = 1.f;
};

struct Particle
{
    glm::vec2 Position;
    glm::vec2 Velocity;
    glm::vec4 Color;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription BindingDescription = {};
        BindingDescription.binding = 0;
        BindingDescription.stride = sizeof(Particle);
        BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return BindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> AttributeDescriptions = {};

        AttributeDescriptions[0].binding = 0;
        AttributeDescriptions[0].location = 0;
        AttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        AttributeDescriptions[0].offset = offsetof(Particle, Position);

        AttributeDescriptions[1].binding = 0;
        AttributeDescriptions[1].location = 1;
        AttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        AttributeDescriptions[1].offset = offsetof(Particle, Color);

        return AttributeDescriptions;
    }
};

class ComputeShaderApplication
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
    GLFWwindow* Window;

    VkInstance Instance;
    VkDebugUtilsMessengerEXT DebugMessenger;
    VkSurfaceKHR Surface;

    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
    VkDevice Device;

    VkQueue GraphicsQueue;
    VkQueue ComputeQueue;
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

    VkDescriptorSetLayout ComputeDescriptorSetLayout;
    VkPipelineLayout ComputePipelineLayout;
    VkPipeline ComputePipeline;

    VkCommandPool CommandPool;

    std::vector<VkBuffer> ShaderStorageBuffers;
    std::vector<VkDeviceMemory> ShaderStorageBuffersMemory;

    std::vector<VkBuffer> UniformBuffers;
    std::vector<VkDeviceMemory> UniformBuffersMemory;
    std::vector<void*> UniformBuffersMapped;

    VkDescriptorPool DescriptionPool;
    std::vector<VkDescriptorSet> ComputeDescriptorSets;

    std::vector<VkCommandBuffer> CommandBuffers;
    std::vector<VkCommandBuffer> ComputeCommandBuffers;

    std::vector<VkSemaphore> ImageAvailableSemaphores;
    std::vector<VkSemaphore> RenderFinishedSemaphores;
    std::vector<VkSemaphore> ComputeFinishedSemaphores;
    std::vector<VkFence> InFlightFences;
    std::vector<VkFence> ComputeInFlightFences;
    uint32_t CurrentFrame = 0;

    float LastFrameTime = 0.f;
    bool FramebufferResized = false;
    double LastTime = 0.f;

    void InitWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(Window, this);
        glfwSetFramebufferSizeCallback(Window, FramebufferResizeCallback);

        LastTime = glfwGetTime();
    }

    static void FramebufferResizeCallback(GLFWwindow* Window, int Width, int Height)
    {
        auto App = reinterpret_cast<ComputeShaderApplication*>(glfwGetWindowUserPointer(Window));
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
        CreateComputeDescriptorSetLayout();
        CreateGraphicsPipeline();
        CreateComputePipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateShaderStorageBuffers();
        CreateUniformBuffers();
        CreateDescriptorPool();
        CreateComputeDescriptorSets();
        CreateCommandBuffers();
        CreateComputeCommandBuffers();
        CreateSyncObjects();
    }

    void MainLoop()
    {
        while(!glfwWindowShouldClose(Window))
        {
            glfwPollEvents();
            DrawFrame();

            double CurrentTime = glfwGetTime();
            LastFrameTime = (CurrentTime - LastTime) * 1000.f;
            LastTime = CurrentTime;
        }

        vkDeviceWaitIdle(Device);
    }

    void CleanupSwapChain()
    {
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

    void CreateSwapChain()
    {
        SwapChainSupportDetails SwapChainSupport = QuerySwapChainSupport(PhysicalDevice);

        VkSurfaceFormatKHR SurfaceFormat = ChooseSwapSurfaceFormat(SwapChainSupport.Formats);
        VkPresentModeKHR PresentMode = ChooseSwapPresentMode(SwapChainSupport.PresentModes);
        VkExtent2D Extent = ChooseSwapExtent(SwapChainSupport.Capabilities);

        uint32_t ImageCount = SwapChainSupport.Capabilities.minImageCount + 1;
        if (
            SwapChainSupport.Capabilities.maxImageCount > 0 && 
            ImageCount > SwapChainSupport.Capabilities.maxImageCount
        )
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

        QueueFamilyIndices Indices = FindQueueFamilies(PhysicalDevice);
        uint32_t QueueFamilyIndices[] = {
            Indices.GraphicsAndComputeFamily.value(),
            Indices.PresentFamily.value()
        };

        if (Indices.GraphicsAndComputeFamily != Indices.PresentFamily)
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

        for (size_t i=0; i < SwapChainImages.size(); i++)
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

            if (
                vkCreateImageView(
                    Device,
                    &CreateInfo,
                    nullptr,
                    &SwapChainImageViews[i]
                ) != VK_SUCCESS
            )
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

    void CreateComputeDescriptorSetLayout()
    {
        std::array<VkDescriptorSetLayoutBinding, 3> LayoutBindings{};
        LayoutBindings[0].binding = 0;
        LayoutBindings[0].descriptorCount = 1;
        LayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        LayoutBindings[0].pImmutableSamplers = nullptr;
        LayoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        LayoutBindings[1].binding = 1;
        LayoutBindings[1].descriptorCount = 1;
        LayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        LayoutBindings[1].pImmutableSamplers = nullptr;
        LayoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        LayoutBindings[2].binding = 2;
        LayoutBindings[2].descriptorCount = 1;
        LayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        LayoutBindings[2].pImmutableSamplers = nullptr;
        LayoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        VkDescriptorSetLayoutCreateInfo LayoutInfo{};
        LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        LayoutInfo.bindingCount = 3;
        LayoutInfo.pBindings = LayoutBindings.data();

        if (vkCreateDescriptorSetLayout(Device, &LayoutInfo, nullptr, &ComputeDescriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }
    }

    void CreateGraphicsPipeline()
    {
        auto VertShaderCode = ReadFile("WVulkanComputeTest_Content/Shaders/CShaderCompute.vert.spv");
        auto FragShaderCode = ReadFile("WVulkanComputeTest_Content/Shaders/CShaderCompute.frag.spv");

        VkShaderModule VertShaderModule = CreateShaderModule(VertShaderCode);
        VkShaderModule FragShaderModule = CreateShaderModule(FragShaderCode);

        VkPipelineShaderStageCreateInfo VertShaderStageInfo{};
        VertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        VertShaderStageInfo.module = VertShaderModule;
        VertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo FragShaderStageInfo{};
        FragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        FragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        FragShaderStageInfo.module = FragShaderModule;
        FragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo ShaderStages[] = {
            VertShaderStageInfo,
            FragShaderStageInfo
        };

        VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
        VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto BindingDescription = Particle::GetBindingDescription();
        auto AttributeDescriptions = Particle::GetAttributeDescriptions();

        VertexInputInfo.vertexBindingDescriptionCount = 1;
        VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(AttributeDescriptions.size());
        VertexInputInfo.pVertexBindingDescriptions = &BindingDescription;
        VertexInputInfo.pVertexAttributeDescriptions = AttributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
        InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
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
        Rasterizer.lineWidth = 1.f;
        Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        Rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo Multisampling{};
        Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        Multisampling.sampleShadingEnable = VK_FALSE;
        Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState ColorBlendAttachment{};
        ColorBlendAttachment.colorWriteMask = 
            VK_COLOR_COMPONENT_R_BIT | 
            VK_COLOR_COMPONENT_G_BIT | 
            VK_COLOR_COMPONENT_B_BIT | 
            VK_COLOR_COMPONENT_A_BIT;

        ColorBlendAttachment.blendEnable = VK_TRUE;
        ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // additive blending?
        ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

        VkPipelineColorBlendStateCreateInfo ColorBlending{};
        ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
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
        PipelineLayoutInfo.pSetLayouts = nullptr;

        if (vkCreatePipelineLayout(Device, &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo PipelineInfo{};
        PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
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

        if (vkCreateGraphicsPipelines(
            Device,
            VK_NULL_HANDLE,
            1,
            &PipelineInfo,
            nullptr,
            &GraphicsPipeline
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(Device, FragShaderModule, nullptr);
        vkDestroyShaderModule(Device, VertShaderModule, nullptr);
    }

    void CreateComputePipeline()
    {
        auto ComputeShaderCode = ReadFile("WVulkanComputeTest_Content/Shaders/CShaderCompute.comp.spv");
        
        VkShaderModule ComputeShaderModule = CreateShaderModule(ComputeShaderCode);

        VkPipelineShaderStageCreateInfo ComputeShaderStageInfo{};
        ComputeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ComputeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        ComputeShaderStageInfo.module = ComputeShaderModule;
        ComputeShaderStageInfo.pName = "main";

        VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
        PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        PipelineLayoutInfo.setLayoutCount = 1;
        PipelineLayoutInfo.pSetLayouts = &ComputeDescriptorSetLayout;

        if (vkCreatePipelineLayout(
                Device, &PipelineLayoutInfo, nullptr, &ComputePipelineLayout
            ) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create compute pipeline layout!");
        }

        VkComputePipelineCreateInfo PipelineInfo{};
        PipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        PipelineInfo.layout = ComputePipelineLayout;
        PipelineInfo.stage = ComputeShaderStageInfo;

        if (vkCreateComputePipelines(
                Device, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &ComputePipeline
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("Failed to create compute pipeline!");
        }

        vkDestroyShaderModule(Device, ComputeShaderModule, nullptr);
    }

    uint32_t FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties)
    {
        VkPhysicalDeviceMemoryProperties MemProperties;
        vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &MemProperties);

        for (uint32_t i=0; i < MemProperties.memoryTypeCount; i++)
        {
            if (
                (TypeFilter & (1 << i)) &&
                (MemProperties.memoryTypes[i].propertyFlags & Properties) == Properties
            )
            {
                return i;
            }
        }
        throw std::runtime_error("Failed to find suitable memory type!");
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

            if (vkCreateFramebuffer(
                    Device, &FramebufferInfo, nullptr, &SwapChainFramebuffers[i]
                ) != VK_SUCCESS
            )
            {
                throw std::runtime_error("Failed to create framebuffer!");
            }
        }
    }

    void CreateCommandPool()
    {
        QueueFamilyIndices QueueFamilyIndices = FindQueueFamilies(PhysicalDevice);

        VkCommandPoolCreateInfo PoolInfo{};
        PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        PoolInfo.queueFamilyIndex = QueueFamilyIndices.GraphicsAndComputeFamily.value();

        if (vkCreateCommandPool(Device, &PoolInfo, nullptr, &CommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create command pool!");
        }

    }

    void CreateShaderStorageBuffers()
    {
        // Initialize particles
        std::default_random_engine RndEngine((uint32_t)time(nullptr));
        std::uniform_real_distribution<float> RndDist(0.f, 1.f);

        // Initial particle position on a circle
        std::vector<Particle> Particles(PARTICLE_COUNT);
        for (auto& Particle : Particles)
        {
            float r = 0.25f * sqrt(RndDist(RndEngine));
            float theta = RndDist(RndEngine) * 2.f * 3.14159265359f;
            float x = r * cos(theta) * HEIGHT / WIDTH;
            float y = r * sin(theta);
            Particle.Position = glm::vec2(x, y);
            Particle.Velocity = glm::normalize(glm::vec2(-y, x)) * 0.5f;
            Particle.Color = glm::vec4(
                RndDist(RndEngine),
                RndDist(RndEngine),
                RndDist(RndEngine),
                1.f
            );
        }

        VkDeviceSize BufferSize = sizeof(Particle) * PARTICLE_COUNT;

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
            memcpy(Data, Particles.data(), (size_t)BufferSize);
        vkUnmapMemory(Device, StagingBufferMemory);

        ShaderStorageBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        ShaderStorageBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i=0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            // Create a shader storage buffer for each frame in flight
            CreateBuffer(
                BufferSize,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                ShaderStorageBuffers[i],
                ShaderStorageBuffersMemory[i]
            );

            CopyBuffer(
                StagingBuffer,
                ShaderStorageBuffers[i],
                BufferSize
            );
        }

        vkDestroyBuffer(Device, StagingBuffer, nullptr);
        vkFreeMemory(Device, StagingBufferMemory, nullptr);
    }

    void CreateUniformBuffers()
    {
        VkDeviceSize BufferSize = sizeof(UniformBufferObject);

        UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        UniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i=0; i < MAX_FRAMES_IN_FLIGHT; i++)
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

        PoolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        PoolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo PoolInfo{};
        PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        PoolInfo.poolSizeCount = static_cast<uint32_t>(PoolSizes.size());
        PoolInfo.pPoolSizes = PoolSizes.data();
        PoolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(
            Device, &PoolInfo, nullptr, &DescriptionPool
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool!");
        }

    }

    void CreateComputeDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> Layouts(MAX_FRAMES_IN_FLIGHT, ComputeDescriptorSetLayout);
        VkDescriptorSetAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        AllocInfo.descriptorPool = DescriptionPool;
        AllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        AllocInfo.pSetLayouts = Layouts.data();

        ComputeDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(
            Device, &AllocInfo, ComputeDescriptorSets.data()
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo UniformBufferInfo{};
            UniformBufferInfo.buffer = UniformBuffers[i];
            UniformBufferInfo.offset = 0;
            UniformBufferInfo.range = sizeof(UniformBufferObject);

            std::array<VkWriteDescriptorSet, 3> DescriptorWrites{};
            DescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            DescriptorWrites[0].dstSet = ComputeDescriptorSets[i];
            DescriptorWrites[0].dstBinding = 0;
            DescriptorWrites[0].dstArrayElement = 0;
            DescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            DescriptorWrites[0].descriptorCount = 1;
            DescriptorWrites[0].pBufferInfo = &UniformBufferInfo;

            VkDescriptorBufferInfo StorageBufferInfoLastFrame{};
            StorageBufferInfoLastFrame.buffer = ShaderStorageBuffers[(i - 1) % MAX_FRAMES_IN_FLIGHT];
            StorageBufferInfoLastFrame.offset = 0;
            StorageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_COUNT;

            DescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            DescriptorWrites[1].dstSet = ComputeDescriptorSets[i];
            DescriptorWrites[1].dstBinding = 1;
            DescriptorWrites[1].dstArrayElement = 0;
            DescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            DescriptorWrites[1].descriptorCount = 1;
            DescriptorWrites[1].pBufferInfo = &StorageBufferInfoLastFrame;

            VkDescriptorBufferInfo StorageBufferInfoCurrentFrame{};
            StorageBufferInfoCurrentFrame.buffer = ShaderStorageBuffers[i];
            StorageBufferInfoCurrentFrame.offset = 0;
            StorageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_COUNT;

            DescriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            DescriptorWrites[2].dstSet = ComputeDescriptorSets[i];
            DescriptorWrites[2].dstBinding = 2;
            DescriptorWrites[2].dstArrayElement = 0;
            DescriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            DescriptorWrites[2].descriptorCount = 1;
            DescriptorWrites[2].pBufferInfo = &StorageBufferInfoCurrentFrame;

            vkUpdateDescriptorSets(
                Device,
                static_cast<uint32_t>(DescriptorWrites.size()),
                DescriptorWrites.data(),
                0,
                nullptr
            );
        }
    }

    VkShaderModule CreateShaderModule(const std::vector<char>& Code)
    {
        VkShaderModuleCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        CreateInfo.codeSize = Code.size();
        CreateInfo.pCode = reinterpret_cast<const uint32_t*>(Code.data());

        VkShaderModule ShaderModule;
        if (vkCreateShaderModule(Device, &CreateInfo, nullptr, &ShaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module!");
        }

        return ShaderModule;
    }

    void CreateBuffer(
        VkDeviceSize Size,
        VkBufferUsageFlags Usage,
        VkMemoryPropertyFlags Properties,
        VkBuffer& Buffer,
        VkDeviceMemory& BufferMemory
    )
    {
        VkBufferCreateInfo BufferInfo{};
        BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferInfo.size = Size;
        BufferInfo.usage = Usage;
        BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(Device, &BufferInfo, nullptr, &Buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create buffer!");
        }

        VkMemoryRequirements MemRequirements;
        vkGetBufferMemoryRequirements(Device, Buffer,  &MemRequirements);

        VkMemoryAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        AllocInfo.allocationSize = MemRequirements.size;
        AllocInfo.memoryTypeIndex = FindMemoryType(
            MemRequirements.memoryTypeBits,
            Properties
        );

        if (vkAllocateMemory(Device, &AllocInfo, nullptr, &BufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate buffer memory!");
        }

        vkBindBufferMemory(Device, Buffer, BufferMemory, 0);
    }

    void CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
    {
        // Create a command buffer
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

        // Start recording
        vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

        VkBufferCopy CopyRegion{};
        CopyRegion.size = Size;
            vkCmdCopyBuffer(CommandBuffer, SrcBuffer, DstBuffer, 1, &CopyRegion);

        // End recording
        vkEndCommandBuffer(CommandBuffer);

        VkSubmitInfo SubmitInfo{};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &CommandBuffer;

        // Submit and wait
        vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(GraphicsQueue);

        // Free command buffer
        vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
    }

    void CreateCommandBuffers()
    {
        CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        
        VkCommandBufferAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocInfo.commandPool = CommandPool;
        AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        AllocInfo.commandBufferCount = (uint32_t) CommandBuffers.size();

        if (vkAllocateCommandBuffers(Device, &AllocInfo, CommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    void CreateComputeCommandBuffers()
    {
        ComputeCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocInfo.commandPool = CommandPool;
        AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        AllocInfo.commandBufferCount = (uint32_t)ComputeCommandBuffers.size();

        if (vkAllocateCommandBuffers(Device, &AllocInfo, ComputeCommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate compute command buffers!");
        }
    }

    void RecordCommandBuffer(VkCommandBuffer CommandBuffer, uint32_t ImageIndex)
    {
        VkCommandBufferBeginInfo BeginInfo{};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(CommandBuffer, &BeginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo RenderPassInfo{};
        RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassInfo.renderPass = RenderPass;
        RenderPassInfo.framebuffer = SwapChainFramebuffers[ImageIndex];
        RenderPassInfo.renderArea.offset = {0, 0};
        RenderPassInfo.renderArea.extent = SwapChainExtent;

        VkClearValue ClearColor = {{{0.f, 0.f, 0.f, 1.f}}};
        RenderPassInfo.clearValueCount = 1;
        RenderPassInfo.pClearValues = &ClearColor;

        vkCmdBeginRenderPass(CommandBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);

            VkViewport Viewport{};
            Viewport.x = 0.f;
            Viewport.y = 0.f;
            Viewport.width = (float) SwapChainExtent.width;
            Viewport.height = (float) SwapChainExtent.height;
            Viewport.minDepth = 0.f;
            Viewport.maxDepth = 1.f;
            vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);

            VkRect2D Scissor{};
            Scissor.offset = {0, 0};
            Scissor.extent = SwapChainExtent;
            vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);

            VkDeviceSize Offsets[] = {0};
            vkCmdBindVertexBuffers(
                CommandBuffer, 
                0, 
                1, 
                &ShaderStorageBuffers[CurrentFrame], 
                Offsets
            );

            vkCmdDraw(CommandBuffer, PARTICLE_COUNT, 1, 0, 0);
        
        vkCmdEndRenderPass(CommandBuffer);

        if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer!");
        }
    }

    void CreateSyncObjects()
    {
        ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        ComputeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        ComputeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo SemaphoreInfo{};
        SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo FenceInfo{};
        FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (
                vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &ImageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &RenderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(Device, &FenceInfo, nullptr, &InFlightFences[i]) != VK_SUCCESS
            )
            {
                throw std::runtime_error("Failed to create synchronization objects for a frame!");
            }
            if (
                vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &ComputeFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(Device, &FenceInfo, nullptr, &ComputeInFlightFences[i]) != VK_SUCCESS
            )
            {
                throw std::runtime_error("Failed to create compute synchronization objects for a frame!");
            }
        }
    }

    void UpdateUniformBuffer(uint32_t CurrentImage)
    {
        UniformBufferObject UBO{};
        UBO.DeltaTime = LastFrameTime * 2.f;

        memcpy(UniformBuffersMapped[CurrentImage], &UBO, sizeof(UBO));
    }

    void DrawFrame()
    {
        VkSubmitInfo SubmitInfo{};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        vkWaitForFences(
            Device,
            1,
            &ComputeInFlightFences[CurrentFrame],
            VK_TRUE,
            UINT64_MAX
        );

        UpdateUniformBuffer(CurrentFrame);

        vkResetFences(Device, 1, &ComputeInFlightFences[CurrentFrame]);

        vkResetCommandBuffer(ComputeCommandBuffers[CurrentFrame], 0);
        RecordComputeCommandBuffer(ComputeCommandBuffers[CurrentFrame]);

        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &ComputeCommandBuffers[CurrentFrame];
        SubmitInfo.signalSemaphoreCount = 1;
        SubmitInfo.pSignalSemaphores = &ComputeFinishedSemaphores[CurrentFrame];

        if (vkQueueSubmit(
            ComputeQueue,
            1,
            &SubmitInfo,
            ComputeInFlightFences[CurrentFrame]
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit compute command buffer!");
        }

        vkWaitForFences(
            Device,
            1,
            &InFlightFences[CurrentFrame],
            VK_TRUE,
            UINT64_MAX
        );

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
        vkResetFences(Device, 1, &InFlightFences[CurrentFrame]);

        vkResetCommandBuffer(CommandBuffers[CurrentFrame], 0);
        RecordCommandBuffer(CommandBuffers[CurrentFrame], ImageIndex);

        VkSemaphore WaitSemaphores[] = {
            ComputeFinishedSemaphores[CurrentFrame],
            ImageAvailableSemaphores[CurrentFrame]
        };
        VkPipelineStageFlags WaitStages[] = {
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        SubmitInfo.waitSemaphoreCount = 2;
        SubmitInfo.pWaitSemaphores = WaitSemaphores;
        SubmitInfo.pWaitDstStageMask = WaitStages;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &CommandBuffers[CurrentFrame];
        SubmitInfo.signalSemaphoreCount = 1;
        SubmitInfo.pSignalSemaphores = &RenderFinishedSemaphores[CurrentFrame];

        if (vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, InFlightFences[CurrentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit draw command buffer!");
        }

        VkPresentInfoKHR PresentInfo{};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pWaitSemaphores = &RenderFinishedSemaphores[CurrentFrame];
        
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

        CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void RecordComputeCommandBuffer(VkCommandBuffer CommandBuffer)
    {
        VkCommandBufferBeginInfo BeginInfo{};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(CommandBuffer, &BeginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline);

        vkCmdBindDescriptorSets(
            CommandBuffer,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            ComputePipelineLayout,
            0, 1,
            &ComputeDescriptorSets[CurrentFrame],
            0, nullptr
        );

        vkCmdDispatch(CommandBuffer, PARTICLE_COUNT / 256, 1, 1);

        if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer!");
        }
    }

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes)
    {
        for (const auto& AvailablePresentMode : AvailablePresentModes)
        {
            if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return AvailablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilities)
    {
        if (Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return Capabilities.currentExtent;
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
                Capabilities.minImageExtent.width,
                Capabilities.maxImageExtent.width
            );
            ActualExtent.height = std::clamp(
                ActualExtent.height,
                Capabilities.minImageExtent.height,
                Capabilities.maxImageExtent.height
            );

            return ActualExtent;
        }
    }

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats)
    {
        for (const auto& AvailableFormat : AvailableFormats)
        {
            if (AvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return AvailableFormat;
            }
        }
        return AvailableFormats[0];
    }

    void Cleanup()
    {
        CleanupSwapChain();

        vkDestroyPipeline(Device, ComputePipeline, nullptr);
        vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);

        vkDestroyPipeline(Device, ComputePipeline, nullptr);
        vkDestroyPipelineLayout(Device, ComputePipelineLayout, nullptr);

        vkDestroyRenderPass(Device, RenderPass, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroyBuffer(Device, UniformBuffers[i], nullptr);
            vkFreeMemory(Device, UniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(Device, DescriptionPool, nullptr);
        vkDestroyDescriptorSetLayout(Device, ComputeDescriptorSetLayout, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroyBuffer(Device, ShaderStorageBuffers[i], nullptr);
            vkFreeMemory(Device, ShaderStorageBuffersMemory[i], nullptr);
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(Device, RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(Device, ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(Device, ComputeFinishedSemaphores[i], nullptr);
            vkDestroyFence(Device, InFlightFences[i], nullptr);
            vkDestroyFence(Device, ComputeInFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(Device, CommandPool, nullptr);

        vkDestroyDevice(Device, nullptr);

        if (EnableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(Instance, Surface, nullptr);
        vkDestroyInstance(Instance, nullptr);

        glfwDestroyWindow(Window);

        glfwTerminate();
    }

    void RecreateSwapChain()
    {
        int Width=0, Height=0;
        glfwGetFramebufferSize(Window, &Width, &Height);
        while (Width == 0 || Height == 0)
        {
            glfwGetFramebufferSize(Window, &Width, &Height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(Device);

        CleanupSwapChain();

        CreateSwapChain();
        CreateImageViews();
        CreateFramebuffers();
    }

    void CreateInstance()
    {
        if (EnableValidationLayers && !CheckValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }

        VkApplicationInfo AppInfo = {};
        AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        AppInfo.pApplicationName = "Vulkan Compute Test";
        AppInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
        AppInfo.pEngineName = "No Engine";
        AppInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
        AppInfo.apiVersion = VK_API_VERSION_1_3; // check if this is correct

        VkInstanceCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        CreateInfo.pApplicationInfo = &AppInfo;

        auto Extensions = GetRequiredExtensions();
        CreateInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions.size());
        CreateInfo.ppEnabledExtensionNames = Extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo{};
        if (EnableValidationLayers)
        {
            CreateInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
            CreateInfo.ppEnabledLayerNames = ValidationLayers.data();

            PopulateDebugMessengerCreateInfo(DebugCreateInfo);
            CreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&DebugCreateInfo;
        }
        else
        {
            CreateInfo.enabledLayerCount = 0;
            CreateInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&CreateInfo, nullptr, &Instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create instance!");
        }
    }

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& CreateInfo)
    {
        CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        CreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        CreateInfo.messageType = 
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        CreateInfo.pfnUserCallback = DebugCallback;
    }

    void SetupDebugMessenger()
    {
        if (!EnableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT CreateInfo;
        PopulateDebugMessengerCreateInfo(CreateInfo);

        if (CreateDebugUtilsMessengerEXT(Instance, &CreateInfo, nullptr, &DebugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to set up debug messenger!");
        }
    }

    void CreateSurface()
    {
        if (glfwCreateWindowSurface(Instance, Window, nullptr, &Surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    void PickPhysicalDevice()
    {
        uint32_t DeviceCount = 0;
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
        QueueFamilyIndices Indices = FindQueueFamilies(PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
        std::set<uint32_t> UniqueQueueFamilies = {
            Indices.GraphicsAndComputeFamily.value(),
            Indices.PresentFamily.value()
        };
        
        float QueuePriority = 1.f;
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

        vkGetDeviceQueue(Device, Indices.GraphicsAndComputeFamily.value(), 0, &GraphicsQueue);
        vkGetDeviceQueue(Device, Indices.GraphicsAndComputeFamily.value(), 0, &ComputeQueue);
        vkGetDeviceQueue(Device, Indices.PresentFamily.value(), 0, &PresentQueue);
    }

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device)
    {
        SwapChainSupportDetails Details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &Details.Capabilities);

        uint32_t FormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, nullptr);

        if (FormatCount != 0)
        {
            Details.Formats.resize(FormatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                Device, 
                Surface, 
                &FormatCount, 
                Details.Formats.data()
            );
        }

        uint32_t PresentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            Device, 
            Surface, 
            &PresentModeCount, 
            nullptr
        );

        if (PresentModeCount != 0)
        {
            Details.PresentModes.resize(PresentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                Device,
                Surface,
                &PresentModeCount,
                Details.PresentModes.data()
            );
        }
        return Details;
    }

    bool IsDeviceSuitable(const VkPhysicalDevice& InDevice)
    {
        QueueFamilyIndices Indices = FindQueueFamilies(InDevice);

        bool ExtensionsSupported = CheckDeviceExtensionSupport(InDevice);

        bool SwapChainAdequate = false;
        if (ExtensionsSupported)
        {
            SwapChainSupportDetails SwapChainSupport = QuerySwapChainSupport(InDevice);
            SwapChainAdequate = !SwapChainSupport.Formats.empty() && !SwapChainSupport.PresentModes.empty();
        }
        return Indices.IsComplete() && ExtensionsSupported && SwapChainAdequate;
    }

    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& InDevice)
    {
        uint32_t ExtensionCount;
        vkEnumerateDeviceExtensionProperties(InDevice, nullptr, &ExtensionCount, nullptr);

        std::vector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
        vkEnumerateDeviceExtensionProperties(InDevice, nullptr, &ExtensionCount, AvailableExtensions.data());

        std::set<std::string> RequiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

        for (const auto& Extension : AvailableExtensions)
        {
            RequiredExtensions.erase(Extension.extensionName);
        }

        return RequiredExtensions.empty();
    }

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice Device)
    {
        QueueFamilyIndices Indices;

        uint32_t QueueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilies.data());

        int i = 0;
        for (const auto& QueueFamily : QueueFamilies)
        {
            if ((QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (QueueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
            {
                Indices.GraphicsAndComputeFamily = i;
            }

            VkBool32 PresentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(Device, i, Surface, &PresentSupport);

            if (PresentSupport)
            {
                Indices.PresentFamily = i;
            }

            if (Indices.IsComplete())
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

        if (EnableValidationLayers)
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

        for (const char* LayerName : ValidationLayers)
        {
            bool LayerFound = false;

            for (const auto& LayerProperties : AvailableLayers)
            {
                if (strcmp(LayerName, LayerProperties.layerName) == 0)
                {
                    LayerFound = true;
                    break;
                }
            }

            if (!LayerFound)
            {
                return false;
            }
        }
        return true;
    }

    static std::vector<char> ReadFile(const std::string& Filename)
    {
        std::ifstream File(Filename, std::ios::ate | std::ios::binary);
        if (!File.is_open())
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

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT MessageType,
        const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,
        void* UserData
    )
    {
        std::cerr << "Validation layer: " << CallbackData->pMessage << std::endl;
        return VK_FALSE;
    }
};

int main()
{
    ComputeShaderApplication App;

    try {
        App.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}