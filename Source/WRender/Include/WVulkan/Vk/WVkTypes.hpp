#pragma once

#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/Vk/WVulkan.hpp"

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
    inline VkShaderStageFlagBits ToShaderStageFlagBits(const wct::render::EShaderStageFlag & type) {
        switch (type)
        {
        case wct::render::EShaderStageFlag::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case wct::render::EShaderStageFlag::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case wct::render::EShaderStageFlag::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            throw std::runtime_error("Invalid shader type!");
        }
    }

    inline VkShaderStageFlags ToVkShaderStageFlag(const wct::render::EShaderStageFlag & in_pipe_flags) {

        VkShaderStageFlags result{0};
        for(const auto & f : wct::render::SHADER_STAGE_FLAGS_LIST) {
            
            if ((f & in_pipe_flags) == wct::render::EShaderStageFlag::None)
                continue; 

            VkShaderStageFlags n{0};
            
            switch(f) {
            case wct::render::EShaderStageFlag::Vertex:
                n = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case wct::render::EShaderStageFlag::Fragment:
                n = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case wct::render::EShaderStageFlag::Compute:
                n = VK_SHADER_STAGE_COMPUTE_BIT;
                break;
            default:
                continue;
            }

            result |= n;
        }

        return result;
    }

    inline constexpr VkApplicationInfo VkApplicationInfo() noexcept {
        ::VkApplicationInfo result{};
        result.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkInstanceCreateInfo VkInstanceCreateInfo() noexcept {
        ::VkInstanceCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkDebugUtilsMessengerCreateInfoEXT VkDebugUtilsMessengerCreateInfoEXT() noexcept {
        ::VkDebugUtilsMessengerCreateInfoEXT result{};
        result.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkSwapchainCreateInfoKHR VkSwapchainCreateInfoKHR() noexcept {
        ::VkSwapchainCreateInfoKHR result{};
        result.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkDescriptorSetAllocateInfo VkDescriptorSetAllocateInfo() noexcept {
        return {
            .sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext=VK_NULL_HANDLE
        };
    }

    inline constexpr VkWriteDescriptorSet VkWriteDescriptorSet() noexcept {
        return {
            .sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext=VK_NULL_HANDLE,
            .pImageInfo=VK_NULL_HANDLE,
            .pBufferInfo=VK_NULL_HANDLE
        };
    }

    inline constexpr VkDescriptorImageInfo VkDescriptorImageInfo() noexcept {
        return {};
    }

    inline constexpr VkDescriptorPoolCreateInfo VkDescriptorPoolCreateInfo() noexcept {
        ::VkDescriptorPoolCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        return result;
    }

    inline constexpr VkRenderingInfo VkRenderingInfo()  noexcept {
        ::VkRenderingInfo result{};
        result.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        return result;
    }

    inline constexpr VkRenderingAttachmentInfo VkRenderingAttachmentInfo() noexcept {
        ::VkRenderingAttachmentInfo result {};
        result.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        return result;
    }

    inline constexpr VkDescriptorSetLayoutBinding VkDescriptorSetLayoutBinding() noexcept {
        return {};
    }

    inline constexpr VkDescriptorSetLayoutCreateInfo VkDescriptorSetLayoutCreateInfo() noexcept {
        return {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr
        };
    }

    inline constexpr VkPipelineColorBlendAttachmentState VkPipelineColorBlendAttachmentState() noexcept {
        return {};
    }

    inline constexpr VkPipelineVertexInputStateCreateInfo VkPipelineVertexInputStateCreateInfo() noexcept {
        ::VkPipelineVertexInputStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineInputAssemblyStateCreateInfo VkPipelineInputAssemblyStateCreateInfo() noexcept {
        ::VkPipelineInputAssemblyStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineViewportStateCreateInfo VkPipelineViewportStateCreateInfo() noexcept {
        ::VkPipelineViewportStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineRasterizationStateCreateInfo VkPipelineRasterizationStateCreateInfo() noexcept {
        ::VkPipelineRasterizationStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineMultisampleStateCreateInfo VkPipelineMultisampleStateCreateInfo() noexcept {
        ::VkPipelineMultisampleStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineDepthStencilStateCreateInfo VkPipelineDepthStencilStateCreateInfo() noexcept {
        ::VkPipelineDepthStencilStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineColorBlendStateCreateInfo VkPipelineColorBlendStateCreateInfo() noexcept {
        ::VkPipelineColorBlendStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineDynamicStateCreateInfo VkPipelineDynamicStateCreateInfo() noexcept {
        ::VkPipelineDynamicStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineLayoutCreateInfo VkPipelineLayoutCreateInfo() noexcept {
        ::VkPipelineLayoutCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkGraphicsPipelineCreateInfo VkGraphicsPipelineCreateInfo() noexcept {
        ::VkGraphicsPipelineCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPipelineRenderingCreateInfo VkPipelineRenderingCreateInfo() noexcept {
        ::VkPipelineRenderingCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkShaderModuleCreateInfo VkShaderModuleCreateInfo() noexcept {
        ::VkShaderModuleCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        return result;
    }

    inline constexpr VkPipelineShaderStageCreateInfo VkPipelineShaderStageCreateInfo() noexcept {
        return {
            .sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext=VK_NULL_HANDLE
        };
    }

    inline constexpr VkSubmitInfo VkSubmitInfo() noexcept {
        ::VkSubmitInfo result{};
        result.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkPresentInfoKHR VkPresentInfoKHR() noexcept {
        ::VkPresentInfoKHR result{};
        result.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkCommandPoolCreateInfo VkCommandPoolCreateInfo() noexcept {
        ::VkCommandPoolCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        result.pNext = VK_NULL_HANDLE;
        return result;
    }

    inline constexpr VkSamplerCreateInfo VkSamplerCreateInfo() noexcept {
        return {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO
        };
    }

    inline constexpr VkSemaphoreCreateInfo VkSemaphoreCreateInfo() noexcept {
        return {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = VK_NULL_HANDLE            
        };
    }

    inline constexpr VkFenceCreateInfo VkFenceCreateInfo() noexcept {
        return {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = VK_NULL_HANDLE
        };
    }

}
