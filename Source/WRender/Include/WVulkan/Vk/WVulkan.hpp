#pragma once

#include "WLog.hpp"

#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <vulkan/vulkan_core.h>
#include <optional>
#include <string_view>
#include <stdexcept>
#include <vector>

namespace wvk::vulkan
{
    /**
     * @brief Execute and check if the result is differrent to VK_SUCCESS.
     */
    template<typename TFn, typename ...Args>
    inline void ExecVkProcChecked(TFn && in_fn, std::string_view message, Args && ... args) {
        
        if (std::forward<TFn>(in_fn) (std::forward<Args>(args)...) != VK_SUCCESS) {
            throw std::runtime_error(std::string(message));
            }
    }

    /**
     * @brief Checks if the requested validation layers are available.
     */
    bool CheckValidationLayerSupport(const std::vector<std::string_view>& debug_info);

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool IsComplete()
            {
                return graphics_family.has_value() && present_family.has_value();
            }
    };

    QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    /**
     * Return a list of required vulkan instance extensions.
     */
    std::vector<const char*> GetRequiredExtensions(bool enable_validation_layers=false);

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT InMessageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT InMessageType,
        const VkDebugUtilsMessengerCallbackDataEXT* InCallbackData,
        void* InUserData
        );

    bool IsDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, const std::vector<std::string_view>& device_extensions);

    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device, const std::vector<std::string_view> & device_extensions);

    VkFormat FindSupportedFormat(const VkPhysicalDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkFormat FindDepthFormat(const VkPhysicalDevice& device);

    uint32_t FindMemoryType(const VkPhysicalDevice& device, uint32_t type_filter, VkMemoryPropertyFlags properties);

    VkCommandBuffer BeginSingleTimeCommands(
        const VkDevice & device, 
        const VkCommandPool & command_pool
        );

    void EndSingleTimeCommands(
        const VkDevice& device, 
        const VkCommandPool& command_pool, 
        const VkQueue& graphics_queue, 
        const VkCommandBuffer& command_buffer
        );

    VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice in_physical_device);

}
