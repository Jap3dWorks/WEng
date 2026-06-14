#pragma once

#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVulkan.hpp"

#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <vulkan/vulkan_core.h>
#include <array>
#include <stdexcept>
#include <vector>

/**
 * Engine Types related operations.
 * Casts and conversions.
 */
namespace wvk::types {

    /**
     * @brief Enum representing the shader stage flags for Vulkan.
     */
    inline VkShaderStageFlagBits ToShaderStageFlagBits(const EShaderStageFlag & type) {
        switch (type)
        {
        case EShaderStageFlag::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case EShaderStageFlag::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case EShaderStageFlag::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            throw std::runtime_error("Invalid shader type!");
        }
    }

    inline VkShaderStageFlags ToVkShaderStageFlag(const EShaderStageFlag & in_pipe_flags) {

        VkShaderStageFlags result{0};
        for(const auto & f : WRenderUtils::SHADER_STAGE_FLAGS_LIST) {
            
            if ((f & in_pipe_flags) == EShaderStageFlag::None)
                continue; 

            VkShaderStageFlags n{0};
            
            switch(f) {
            case EShaderStageFlag::Vertex:
                n = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case EShaderStageFlag::Fragment:
                n = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case EShaderStageFlag::Compute:
                n = VK_SHADER_STAGE_COMPUTE_BIT;
                break;
            default:
                continue;
            }

            result |= n;
        }

        return result;
    }

    inline constexpr VkApplicationInfo CreateVkApplicationInfo() noexcept {
        VkApplicationInfo result{};
        result.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkInstanceCreateInfo CreateVkInstanceCreateInfo() noexcept {
        VkInstanceCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkDebugUtilsMessengerCreateInfoEXT CreateVkDebugUtilsMessengerCreateInfoEXT() noexcept {
        VkDebugUtilsMessengerCreateInfoEXT result{};
        result.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkSwapchainCreateInfoKHR CreateVkSwapchainCreateInfoKHR() noexcept {
        VkSwapchainCreateInfoKHR result{};
        result.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkWriteDescriptorSet CreateVkWriteDescriptorSet() noexcept {
        VkWriteDescriptorSet result{};
        result.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        return result;
    }

    inline constexpr VkDescriptorImageInfo CreateVkDescriptorImageInfo() noexcept {
        return {};
    }

    inline constexpr VkRenderingInfo CreateVkRenderingInfo()  noexcept {
        VkRenderingInfo result{};
        result.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        return result;
    }

    inline constexpr VkRenderingAttachmentInfo CreateVkRenderingAttachmentInfo() noexcept {
        VkRenderingAttachmentInfo result {};
        result.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        return result;
    }

    inline constexpr VkDescriptorSetLayoutBinding CreateVkDescriptorSetLayoutBinding() noexcept {
        return {};
    }

    inline constexpr VkPipelineColorBlendAttachmentState CreateVkPipelineColorBlendAttachmentState() noexcept {
        return {};
    }

    inline constexpr VkPipelineVertexInputStateCreateInfo CreateVkPipelineVertexInputStateCreateInfo() noexcept {
        VkPipelineVertexInputStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineInputAssemblyStateCreateInfo CreateVkPipelineInputAssemblyStateCreateInfo() noexcept {
        VkPipelineInputAssemblyStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineViewportStateCreateInfo CreateVkPipelineViewportStateCreateInfo() noexcept {
        VkPipelineViewportStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineRasterizationStateCreateInfo CreateVkPipelineRasterizationStateCreateInfo() noexcept {
        VkPipelineRasterizationStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineMultisampleStateCreateInfo CreateVkPipelineMultisampleStateCreateInfo() noexcept {
        VkPipelineMultisampleStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineDepthStencilStateCreateInfo CreateVkPipelineDepthStencilStateCreateInfo() noexcept {
        VkPipelineDepthStencilStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineColorBlendStateCreateInfo CreateVkPipelineColorBlendStateCreateInfo() noexcept {
        VkPipelineColorBlendStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineDynamicStateCreateInfo CreateVkPipelineDynamicStateCreateInfo() noexcept {
        VkPipelineDynamicStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineLayoutCreateInfo CreateVkPipelineLayoutCreateInfo() noexcept {
        VkPipelineLayoutCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkGraphicsPipelineCreateInfo CreateVkGraphicsPipelineCreateInfo() noexcept {
        VkGraphicsPipelineCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineRenderingCreateInfo CreateVkPipelineRenderingCreateInfo() noexcept {
        VkPipelineRenderingCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkShaderModuleCreateInfo CreateVkShaderModuleCreateInfo() noexcept {
        VkShaderModuleCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        return result;
    }

    inline constexpr VkPipelineShaderStageCreateInfo CreateVkPipelineShaderStageCreateInfo() noexcept {
        VkPipelineShaderStageCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkSubmitInfo CreateVkSubmitInfo() noexcept {
        VkSubmitInfo result{};
        result.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPresentInfoKHR CreateVkPresentInfoKHR() noexcept {
        VkPresentInfoKHR result{};
        result.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkCommandPoolCreateInfo CreateVkCommandPoolCreateInfo() noexcept {
        VkCommandPoolCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

}
