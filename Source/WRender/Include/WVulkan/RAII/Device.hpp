#pragma once

#include "WVulkan/Vk/WVulkan.hpp"
#include "WString/WString.hpp"

#include <vector>
#include <set>
#include <string_view>
#include <vulkan/vulkan_core.h>
#include <stdexcept>

namespace wvk::raii {

    class Device {

    private:

        static VkPhysicalDevice CollectPhysicalDevice(
            VkInstance instance,
            VkSurfaceKHR surface,
            const std::vector<std::string_view> & device_extensions,
            std::optional<std::string> physical_device_name
            ) {

            uint32_t device_count = 0;
            vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
            if (device_count == 0)
            {
                throw std::runtime_error("Failed to find GPUs with Vulkan support!");
            }

            std::vector<VkPhysicalDevice> devices(device_count);
            vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

            std::vector<VkPhysicalDevice> valid_devices{};

            for (const auto &device : devices)
            {
                if (wvk::vulkan::IsDeviceSuitable(device, surface, device_extensions))
                {
                    valid_devices.push_back(device);
                }
            }

            std::vector<VkPhysicalDeviceProperties> properties;
            properties.resize(valid_devices.size());

            for (std::uint32_t i=0; i < properties.size(); ++i) {
                vkGetPhysicalDeviceProperties(
                    valid_devices[i],
                    &properties[i]
                    );
            }

            auto get_result = [&]
                (VkPhysicalDevice physical_device, VkPhysicalDeviceProperties prop) {
                WLOG("Selected device : {}", prop.deviceName);
                WLOG("Device uniform buffer offset alignment limit : {}",
                     prop.limits.minUniformBufferOffsetAlignment);

                return physical_device;
            };

            if (physical_device_name) {
                for (std::uint32_t i=0; i<properties.size(); ++i) {
                    if ((*physical_device_name) == properties[i].deviceName) {
                        return get_result(valid_devices[i], properties[i]);
                    }
                }

            }
            for (std::uint32_t i=0; i<properties.size(); ++i) {
                if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == properties[i].deviceType) {
                    return get_result(valid_devices[i], properties[i]);
                }
            }

            return get_result(valid_devices[0], properties[0]);
        }

    public:

        Device()=default;

        Device(const std::vector<std::string_view> & in_device_extensions,
               const VkInstance & in_instance,
               const VkSurfaceKHR & in_surface,
               bool in_enable_validation_layers,
               const std::vector<std::string_view>& in_validation_layers,
               std::optional<std::string> physical_device_name=std::nullopt
            ) {

            vk_physical_device_ =
                CollectPhysicalDevice(
                    in_instance,
                    in_surface,
                    in_device_extensions,
                    physical_device_name
                    );

            msaa_samples_ = wvk::vulkan::GetMaxUsableSampleCount(vk_physical_device_);

            if (vk_physical_device_ == VK_NULL_HANDLE)
            {
                throw std::runtime_error("Failed to find a suitable GPU!");
            }

            // Create Logical Device

            wvk::vulkan::QueueFamilyIndices indices =
                wvk::vulkan::FindQueueFamilies(vk_physical_device_, in_surface);
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

            auto enable_extension_names = wstr::ToStrings(in_device_extensions);
            auto extension_names_ptrs = wstr::ToConstCharPtrs(enable_extension_names);

            create_info.ppEnabledExtensionNames = extension_names_ptrs.data();

            create_info.pNext = &vk2_features;

            std::vector<std::string> enabled_layer_names{};
            std::vector<const char *> enabled_layer_ptrs{};
            if (in_enable_validation_layers)
            {
                create_info.enabledLayerCount = static_cast<uint32_t>(in_validation_layers.size());
                enabled_layer_names = wstr::ToStrings(in_validation_layers);
                enabled_layer_ptrs = wstr::ToConstCharPtrs(enabled_layer_names);

                create_info.ppEnabledLayerNames = enabled_layer_ptrs.data();
            }
            else
            {
                create_info.enabledLayerCount = 0;
            }

            wvk::vulkan::ExecVkProcChecked(vkCreateDevice,
                                           "Failed to create logical device!",
                                           vk_physical_device_,
                                           &create_info,
                                           nullptr,
                                           &vk_device_);

            vkGetDeviceQueue(vk_device_,
                             indices.graphics_family.value(),
                             0,
                             &vk_graphics_queue_);

            vkGetDeviceQueue(vk_device_,
                             indices.present_family.value(),
                             0,
                             &vk_present_queue_);
        }

        ~Device() {
            Destroy();
        }

        Device(const Device & other) = delete;
        Device & operator=(const Device & other) = delete;

        Device(Device && other) noexcept :
            vk_physical_device_(std::move(other.vk_physical_device_)),
            vk_device_(std::move(other.vk_device_)),
            msaa_samples_(std::move(other.msaa_samples_)),
            vk_graphics_queue_(std::move(other.vk_graphics_queue_)),
            vk_present_queue_(std::move(other.vk_present_queue_))
            {
                other.vk_physical_device_ = VK_NULL_HANDLE;
                other.vk_device_ = VK_NULL_HANDLE;
                other.msaa_samples_ = VK_SAMPLE_COUNT_1_BIT;
                other.vk_graphics_queue_ = VK_NULL_HANDLE;
                other.vk_present_queue_ = VK_NULL_HANDLE;
            }

        Device & operator=(Device && other) {
            if (this != &other) {

                Destroy();

                vk_physical_device_ = std::move(other.vk_physical_device_);
                vk_device_ = std::move(other.vk_device_);
                msaa_samples_ = std::move(other.msaa_samples_);
                vk_graphics_queue_ = std::move(other.vk_graphics_queue_);
                vk_present_queue_ = std::move(other.vk_present_queue_);

                other.vk_physical_device_ = VK_NULL_HANDLE;
                other.vk_device_ = VK_NULL_HANDLE;
                other.msaa_samples_ = VK_SAMPLE_COUNT_1_BIT;
                other.vk_graphics_queue_ = VK_NULL_HANDLE;
                other.vk_present_queue_ = VK_NULL_HANDLE;
            }

            return *this;
        }

    public:

        VkPhysicalDevice GetPhysicalDevice() const noexcept {
            return vk_physical_device_;
        }

        VkDevice GetDevice() const noexcept {
            return vk_device_;
        }

        VkSampleCountFlagBits GetMSAASamples() const noexcept {
            return msaa_samples_;
        }

        VkQueue GetGraphicsQueue() const noexcept {
            return vk_graphics_queue_;
        }

        VkQueue GetPresentQueue() const noexcept {
            return vk_present_queue_;
        }

    private:

        void Destroy() {
            if (vk_device_ != VK_NULL_HANDLE) {

                vkDeviceWaitIdle(vk_device_);
                vkDestroyDevice(vk_device_, nullptr);

                vk_device_ = VK_NULL_HANDLE;            
            }
        }

        VkPhysicalDevice vk_physical_device_{VK_NULL_HANDLE};

        VkDevice vk_device_ {VK_NULL_HANDLE};

        VkSampleCountFlagBits msaa_samples_ { VK_SAMPLE_COUNT_1_BIT };

        VkQueue vk_graphics_queue_ {VK_NULL_HANDLE};
        VkQueue vk_present_queue_ {VK_NULL_HANDLE};

    };
}
