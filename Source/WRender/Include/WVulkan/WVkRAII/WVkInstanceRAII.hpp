#pragma once

#include "WUtils/WStringUtils.hpp"
#include "WVulkan/WVulkan.hpp"

#include <vulkan/vulkan_core.h>

#include <vector>
#include <string_view>

class WVkInstanceRAII {
public:

    WVkInstanceRAII()=default;

    WVkInstanceRAII(bool in_enable_validation_layers,
                    const std::vector<std::string_view>& in_validation_layers,
                    const PFN_vkDebugUtilsMessengerCallbackEXT & in_debug_callback,
                    const VkDebugUtilsMessengerEXT & in_debug_messenger) {
        if (in_enable_validation_layers &&
            !WVulkan::CheckValidationLayerSupport(in_validation_layers))
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }

        VkApplicationInfo app_info = WVulkan::VkStructs::CreateVkApplicationInfo();
        app_info.pApplicationName = "WEngine";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "WEngine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo create_info = WVulkan::VkStructs::CreateVkInstanceCreateInfo();
        create_info.pApplicationInfo = &app_info;

        auto extensions = WVulkan::GetRequiredExtensions(in_enable_validation_layers);
    
        create_info.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info =
            WVulkan::VkStructs::CreateVkDebugUtilsMessengerCreateInfoEXT();

        std::vector<const char*> enabled_layers_names{};
        if (in_enable_validation_layers)
        {
            create_info.enabledLayerCount = static_cast<std::uint32_t>(in_validation_layers.size());
        
            WStringUtils::ToConstCharPtrs(in_validation_layers, enabled_layers_names);

            create_info.ppEnabledLayerNames = enabled_layers_names.data();

            debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

            debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT      |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT   |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            if (in_debug_callback)
            {
                debug_create_info.pfnUserCallback = in_debug_callback;
            }
            else
            {
                debug_create_info.pfnUserCallback = &WVulkan::DebugCallback;
            }

            debug_create_info.flags = 0;

            create_info.pNext = &debug_create_info;
        }
        else
        {
            create_info.enabledLayerCount = 0;
        }

        WVulkan::ExecVkProcChecked(
            vkCreateInstance,
            "Failed to create instance!",
            &create_info,
            nullptr,
            &vk_instance_
            );
    }

    ~WVkInstanceRAII() {
        Destroy();
    }

    WVkInstanceRAII(const WVkInstanceRAII & other) = delete;
    WVkInstanceRAII & operator=(const WVkInstanceRAII & other) = delete;

    WVkInstanceRAII(WVkInstanceRAII && other) noexcept :
        vk_instance_(std::move(other.vk_instance_)) {
        other.vk_instance_ = VK_NULL_HANDLE;
    }

    WVkInstanceRAII & operator=(WVkInstanceRAII && other) {
        if (this != &other) {
            
            Destroy();

            vk_instance_ = std::move(other.vk_instance_);
            other.vk_instance_ = VK_NULL_HANDLE;
        }

        return *this;
    }

public:

    VkInstance Instance() const noexcept {
        return vk_instance_;
    }

private:

    void Destroy() {
        if (vk_instance_ != VK_NULL_HANDLE) {
            vkDestroyInstance(vk_instance_, nullptr);
            vk_instance_ = VK_NULL_HANDLE;
        }
    }

    VkInstance vk_instance_{VK_NULL_HANDLE};

};
