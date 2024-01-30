#pragma once

#include "WCore.h"
#include "WRenderCore.h"
#include <string>
#include <vector>
#include "vulkan/vulkan.h"
#include <stdexcept>


enum class WShaderType
{
    Vertex,
    Fragment,
    // Geometry,
    Compute,
    // TessellationControl,
    // TessellationEvaluation
};

struct WShaderStage
{
    WId id;
    std::vector<char> code;
    WShaderType type;
    std::string entry_point;
    VkShaderModule vk_shader_module=nullptr;

    VkVertexInputBindingDescription vk_vertex_input_binding_description{};
    std::vector<VkVertexInputAttributeDescription> vk_vertex_input_attribute_descriptions{};
    // inputs and descriptors
};

namespace WVulkan
{
    void CreateShaderModule(const WDevice& device, WShaderStage& out_shader_info)
    {
        VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
        ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        ShaderModuleCreateInfo.codeSize = out_shader_info.code.size();
        ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(
            out_shader_info.code.data()
        );

        if (vkCreateShaderModule(
            device.vk_device, 
            &ShaderModuleCreateInfo, 
            nullptr, 
            &out_shader_info.vk_shader_module
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module!");
        }
    }

    VkShaderStageFlagBits ToShaderStageFlagBits(const WShaderType& type)
    {
        switch (type)
        {
        case WShaderType::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case WShaderType::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case WShaderType::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            throw std::runtime_error("Invalid shader type!");
        }
    }
}