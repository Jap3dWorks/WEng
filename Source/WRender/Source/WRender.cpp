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

void WRender::initialize()
{
    instance_info_.wid = 0; // WId::GenerateId();
    // Create Vulkan Instance
    instance_info_.instance = WVulkan::CreateInstance(
        _CREATE_VALIDATON_LAYERS
    );

    // Create Window
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    instance_info_.window = glfwCreateWindow(800, 600, "WEngine", nullptr, nullptr);

    glfwSetWindowUserPointer(Window, this);
    glfwSetFramebufferSizeCallback(Window, FramebufferResizeCallback);

    // Create Vulkan Surface
    if (glfwCreateWindowSurface(
        instance_info_.instance, 
        instance_info_.window, 
        nullptr, 
        &instance_info_.surface
        ) != VK_SUCCESS
    )
    {
        throw std::runtime_error("Failed to create window surface!");
    }

    // Create Vulkan Device
    device_.vk_device = WVulkan::CreateDevice(instance_info_.instance, surface);
}

WRender::~WRender()
{
    // Destroy Vulkan Device
    vkDestroyDevice(device_.vk_device, nullptr);

    // Destroy Vulkan Surface
    // vkDestroySurfaceKHR(instance_info_.instance, surface, nullptr);

    // Destroy Vulkan Instance
    vkDestroyInstance(instance_info_.instance, nullptr);

    // Destroy Window
    glfwDestroyWindow(instance_info_.window);

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

VkInstance WVulkan::CreateInstance(bool enable_validation_layers)
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

    VkInstanceCreateInfo instance_info{};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;

    auto extensions = GetRequiredExtensions(enable_validation_layers);
    instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instance_info.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    if (enable_validation_layers)
    {
        instance_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        instance_info.ppEnabledLayerNames = validation_layers.data();

        PopulateDebugMessengerCreateInfo(debug_create_info);
        instance_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    }
    else
    {
        instance_info.enabledLayerCount = 0;
        instance_info.pNext = nullptr;
    }

    VkInstance instance;
    if (vkCreateInstance(&instance_info, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance!");
    }

    return instance;
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

