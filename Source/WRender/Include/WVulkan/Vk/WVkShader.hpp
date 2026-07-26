#pragma once

#include "WCore/TVisitor.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"

#include <vulkan/vulkan_core.h>
#include <cstdint>

namespace wvk::shader {
    
    inline VkShaderModule CreateShaderModule(
        VkDevice in_device,
        std::uint8_t const * in_code,
        std::size_t in_code_size
        )
    {
        VkShaderModule result;

        VkShaderModuleCreateInfo shader_module_create_info =
            wvk::types::VkShaderModuleCreateInfo();
        shader_module_create_info.codeSize = in_code_size;
        shader_module_create_info.pCode = reinterpret_cast<std::uint32_t const *>(in_code);

        wvk::vulkan::ExecVkProcChecked(
            vkCreateShaderModule,
            "Failed to create shader module!",
            in_device, 
            &shader_module_create_info, 
            nullptr, 
            &result
            );

        return result;
    }

    /**
     * Returns a tuple with shader stages and shader modules,
     * Caller needs remember to delete the shader modules.
     */
    inline auto CreateShaderModules(
        const VkDevice & in_device,
        const std::vector<WVkShaderStageInfo> & stage_infos) {

        auto code_data = [](auto const & itm) -> std::uint8_t const * { return itm.data(); };
        auto code_size = [](auto const & itm) -> std::uint32_t {return itm.size(); };

        std::vector<VkPipelineShaderStageCreateInfo> shader_stages{};
        shader_stages.resize(stage_infos.size());
        
        std::vector<VkShaderModule> shader_modules(stage_infos.size(), VK_NULL_HANDLE);

        for (uint32_t i = 0; i < stage_infos.size(); i++)
        {
            shader_stages[i] = {};
            shader_stages[i].pNext = VK_NULL_HANDLE;
            shader_stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shader_stages[i].stage = wvk::types::ToShaderStageFlagBits(
                stage_infos[i].type
                );

            shader_modules[i] = CreateShaderModule(in_device,
                                                   std::visit(code_data, stage_infos[i].code),
                                                   std::visit(code_size, stage_infos[i].code));

            shader_stages[i].module = shader_modules[i];
            shader_stages[i].pName = stage_infos[i].entry_point.c_str();
        }

        return std::tuple{std::move(shader_stages), std::move(shader_modules)};
    }

    template<std::uint8_t ShaderStages>
    inline auto CreateShaderModules(
        VkDevice device,
        std::array<WVkShaderStageInfo, ShaderStages> const & stage_infos
        ) {
        auto code_data = [](auto const & itm) -> std::uint8_t const * { return itm.data(); };
        auto code_size = [](auto const & itm) -> std::uint32_t {return itm.size(); };

        std::array<VkPipelineShaderStageCreateInfo, ShaderStages> shader_stages{};
        
        std::array<VkShaderModule, ShaderStages> shader_modules{VK_NULL_HANDLE};

        for (uint32_t i = 0; i < stage_infos.size(); i++)
        {
            shader_stages[i] = {};
            shader_stages[i].pNext = VK_NULL_HANDLE;
            shader_stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shader_stages[i].stage = wvk::types::ToShaderStageFlagBits(
                stage_infos[i].type
                );

            shader_modules[i] = CreateShaderModule(device,
                                                   std::visit(code_data, stage_infos[i].code),
                                                   std::visit(code_size, stage_infos[i].code));

            shader_stages[i].module = shader_modules[i];
            shader_stages[i].pName = stage_infos[i].entry_point.c_str();
        }

        return std::tuple{std::move(shader_stages), std::move(shader_modules)};
        
    }
}
