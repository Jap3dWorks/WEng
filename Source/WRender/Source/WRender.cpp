#include "WRender.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef NDEBUG
    #define _CREATE_VALIDATON_LAYERS false
#else
    #define _CREATE_VALIDATON_LAYERS true
#endif

// WRender
// -------

WRender::WRender()
{
    instance_info_ = {};
    instance_info_.wid = 0; // WId::GenerateId();

    window_info_ = {};
    window_info_.wid = 0; // WId::GenerateId();

    surface_info_ = {};
    surface_info_.wid = 0; // WId::GenerateId();

    device_ = {};
    device_.wid = 0; // WId::GenerateId();

    render_pipelines_ = {};
    render_pipelines_.wid = 0; // WId::GenerateId();

    initialize();
}

void WRender::initialize()
{
    // Create Vulkan Instance
    WVulkan::CreateInstance(
        instance_info_,
        _CREATE_VALIDATON_LAYERS
    );

    WVulkan::CreateWindow(
        window_info_
    );

    surface_info_.surface = WVulkan::CreateSurface(
        surface_info_,
        instance_info_, 
        window_info_
    );

    // Create Vulkan Device
    device_.vk_device = WVulkan::CreateDevice(instance_info_.instance, surface);
}

WRender::~WRender()
{
    // Destroy Vulkan Device
    vkDestroyDevice(device_.vk_device, nullptr);

    // Destroy Vulkan Surface
    vkDestroySurfaceKHR(
        instance_info_.instance, 
        instance_info_.surface, 
        nullptr
    );

    // Destroy Vulkan Instance
    vkDestroyInstance(instance_info_.instance, nullptr);

    // Destroy Window
    glfwDestroyWindow(window_info_.window);

    // Terminate GLFW
    glfwTerminate();
}

// WVulkan
// -------

bool WVulkan::CheckValidationLayerSupport()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char* layer_name : validation_layers)
    {
        bool layer_found = false;

        for (const auto& layer_properties : available_layers)
        {
            if (strcmp(layer_name, layer_properties.layerName) == 0)
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

void WVulkan::CreateInstance(WInstanceInfo &out_instance_info,  enable_validation_layers)
{
    if (enable_validation_layers && !CheckValidationLayerSupport())
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

    auto extensions = GetRequiredExtensions(enable_validation_layers);
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    if (enable_validation_layers)
    {
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();

        PopulateDebugMessengerCreateInfo(debug_create_info);
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

VkSurfaceKHR WVulkan::CreateSurface(VkInstance instance, GLFWwindow* window)
{
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface!");
    }
    return surface;
}

void WVulkan::InitWindow(WWindowInfo &out_window_info)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    out_window_info.window = glfwCreateWindow(
        out_window_info.width, out_window_info.height, out_window_info.title.c_str(), nullptr, nullptr
    );

    glfwSetWindowUserPointer(out_window_info.window, this);
    if (out_window_info.framebuffer_size_callback)
    {
        glfwSetFramebufferSizeCallback(
            out_window_info.window, 
            out_window_info.framebuffer_size_callback
        );
    }
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

