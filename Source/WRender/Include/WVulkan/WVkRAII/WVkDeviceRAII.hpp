#pragma once

#include "WVulkan/WVulkan.hpp"
#include "WUtils/WStringUtils.hpp"

#include <vector>
#include <set>
#include <string_view>
#include <vulkan/vulkan_core.h>
#include <stdexcept>

class WVkDeviceRAII {

public:
    
    WVkDeviceRAII()=default;

    WVkDeviceRAII(const std::vector<std::string_view> & in_device_extensions,
                  const VkInstance & in_instance,
                  const VkSurfaceKHR & in_surface,
                  bool in_enable_validation_layers,
                  const std::vector<std::string_view>& in_validation_layers) {

        // Pick Physical Device
    
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(in_instance, &device_count, nullptr);
        if (device_count == 0)
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }
    
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(in_instance, &device_count, devices.data());

        for (const auto &device : devices)
        {
            if (WVulkan::IsDeviceSuitable(device, in_surface, in_device_extensions))
            {
                // TODO device checks
                vk_physical_device = device;
                msaa_samples = WVulkan::GetMaxUsableSampleCount(device);
                break;
            }
        }

        if (vk_physical_device == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }

        // Create Logical Device

        WVulkan::QueueFamilyIndices indices =
            WVulkan::FindQueueFamilies(vk_physical_device, in_surface);
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = {
            indices.graphics_family.value(),
            indices.present_family.value()
        };

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

        // device features

        VkPhysicalDeviceFeatures2 vk2_features{};
        vk2_features.sType= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        vk2_features.features.samplerAnisotropy = VK_TRUE;

        VkPhysicalDeviceVulkan13Features vk13_features{};
        vk13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        vk13_features.dynamicRendering = VK_TRUE;
        vk13_features.synchronization2 = VK_TRUE;

        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT vkext_features={};
        vkext_features.sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
        vkext_features.extendedDynamicState = VK_TRUE;

        vkext_features.pNext = nullptr;
        vk13_features.pNext = &vkext_features;
        vk2_features.pNext = &vk13_features;

        // Start device creation

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        create_info.pQueueCreateInfos = queue_create_infos.data();

        create_info.pEnabledFeatures = nullptr;

        create_info.enabledExtensionCount = static_cast<uint32_t>(in_device_extensions.size());
    
        std::vector<const char *> enable_extension_names{};
        WStringUtils::ToConstCharPtrs(in_device_extensions, enable_extension_names);
    
        create_info.ppEnabledExtensionNames = enable_extension_names.data();
    
        create_info.pNext = &vk2_features;

        std::vector<const char *> enabled_layer_names{};
        if (in_enable_validation_layers)
        {
            create_info.enabledLayerCount = static_cast<uint32_t>(in_validation_layers.size());
            WStringUtils::ToConstCharPtrs(in_validation_layers, enabled_layer_names);
            create_info.ppEnabledLayerNames = enabled_layer_names.data();
        }
        else
        {
            create_info.enabledLayerCount = 0;
        }

        WVulkan::ExecVkProcChecked(vkCreateDevice,
                                   "Failed to create logical device!",
                                   vk_physical_device,
                                   &create_info,
                                   nullptr,
                                   &vk_device);
    
        vkGetDeviceQueue(vk_device,
                         indices.graphics_family.value(),
                         0,
                         &vk_graphics_queue);
    
        vkGetDeviceQueue(vk_device,
                         indices.present_family.value(),
                         0,
                         &vk_present_queue);
    }

    ~WVkDeviceRAII() {
        Destroy();
    }

    WVkDeviceRAII(const WVkDeviceRAII & other) = delete;
    WVkDeviceRAII & operator=(const WVkDeviceRAII & other) = delete;
    
    WVkDeviceRAII(WVkDeviceRAII && other) noexcept :
        vk_physical_device(std::move(other.vk_physical_device)),
        vk_device(std::move(other.vk_device)),
        msaa_samples(std::move(other.msaa_samples)),
        vk_graphics_queue(std::move(other.vk_graphics_queue)),
        vk_present_queue(std::move(other.vk_present_queue))
        {
            other.vk_physical_device = VK_NULL_HANDLE;
            other.vk_device = VK_NULL_HANDLE;
            other.msaa_samples = VK_SAMPLE_COUNT_1_BIT;
            other.vk_graphics_queue = VK_NULL_HANDLE;
            other.vk_present_queue = VK_NULL_HANDLE;
        }

    WVkDeviceRAII & operator=(WVkDeviceRAII && other) {
        if (this != &other) {

            Destroy();

            vk_physical_device = std::move(other.vk_physical_device);
            vk_device = std::move(other.vk_device);
            msaa_samples = std::move(other.msaa_samples);
            vk_graphics_queue = std::move(other.vk_graphics_queue);
            vk_present_queue = std::move(other.vk_present_queue);

            other.vk_physical_device = VK_NULL_HANDLE;
            other.vk_device = VK_NULL_HANDLE;
            other.msaa_samples = VK_SAMPLE_COUNT_1_BIT;
            other.vk_graphics_queue = VK_NULL_HANDLE;
            other.vk_present_queue = VK_NULL_HANDLE;
        }

        return *this;
    }

public:

    VkPhysicalDevice PhysicalDevice() const noexcept {
        return vk_physical_device;
    }

    VkDevice Device() const noexcept {
        return vk_device;
    }

    VkSampleCountFlagBits MSAASamples() const noexcept {
        return msaa_samples;
    }

    VkQueue GraphicsQueue() const noexcept {
        return vk_graphics_queue;
    }

    VkQueue PresentQueue() const noexcept {
        return vk_present_queue;
    }

private:

    void Destroy() {
        if (vk_device != VK_NULL_HANDLE) {
            
            vkDeviceWaitIdle(vk_device);
            vkDestroyDevice(vk_device, nullptr);
            
            vk_device = VK_NULL_HANDLE;            
        }
    }

    VkPhysicalDevice vk_physical_device{VK_NULL_HANDLE};
    VkDevice vk_device {VK_NULL_HANDLE};

    VkSampleCountFlagBits msaa_samples { VK_SAMPLE_COUNT_1_BIT };

    VkQueue vk_graphics_queue {VK_NULL_HANDLE};
    VkQueue vk_present_queue {VK_NULL_HANDLE};

};
