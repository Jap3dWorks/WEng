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
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
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
    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec3 Color;

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
    std::vector<VkDescriptorSet> computeDescriptorSets;

    std::vector<VkCommandBuffer> CommandBuffers;
    std::vector<VkCommandBuffer> ComputeCommandBuffers;

    std::vector<VkSemaphore> ImageAvailableSemaphores;
    std::vector<VkSemaphore> RenderFinishedSemaphores;
    std::vector<VkSemaphore> ComputeFinishedSemaphores;
    std::vector<VkFence> InFlightFences;
    std::vector<VkFence> CompueInFlightFences;
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

    }

    void MainLoop()
    {
        while(!glfwWindowShouldClose(Window))
        {
            glfwPoolEvents();
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
            vkDestroyFence(Device, CompueInFlightFences[i], nullptr);
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

};