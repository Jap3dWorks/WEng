#include "WRender.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstring>
#include <set>
#include <string>

#ifdef NDEBUG
    #define _ENABLE_VALIDATON_LAYERS false
#else
    #define _ENABLE_VALIDATON_LAYERS true
#endif

// WRender
// -------

WRender::WRender() : render_pipelines_(device_info_)
{
    instance_info_ = {};
    instance_info_.wid = 0; // WId::GenerateId();

    window_info_ = {};
    window_info_.wid = 0; // WId::GenerateId();
    window_info_.user_pointer = this;

    surface_info_ = {};
    surface_info_.wid = 0; // WId::GenerateId();

    device_info_ = {};
    device_info_.wid = 0; // WId::GenerateId();

    debug_info_ = {};
    debug_info_.wid = 0; // WId::GenerateId();
    debug_info_.enable_validation_layers = _ENABLE_VALIDATON_LAYERS;
}

void WRender::initialize()
{
    // Create Vulkan Instance
    WVulkan::CreateInstance(
        instance_info_,
        debug_info_
    );

    WVulkan::InitWindow(
        window_info_
    );

    WVulkan::CreateSurface(
        surface_info_,
        instance_info_, 
        window_info_
    );

    // Create Vulkan Device
    WVulkan::CreateDevice(
        device_info_, 
        instance_info_, 
        surface_info_,
        debug_info_
    );
}

WRender::~WRender()
{
    // Destroy Vulkan Device
    vkDestroyDevice(device_info_.vk_device, nullptr);

    // Destroy Vulkan Surface
    vkDestroySurfaceKHR(
        instance_info_.instance, 
        surface_info_.surface, 
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

void WVulkan::InitWindow(WWindowInfo &out_window_info)
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
