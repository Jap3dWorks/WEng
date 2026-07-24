#pragma once

#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"

#include <vulkan/vulkan_core.h>
#include <cstdint>

namespace wvk::shader {
    
    inline VkShaderModule CreateShaderModule(
        const VkDevice & in_device,
        const std::uint32_t * in_code,
        const std::size_t& in_code_size
        )
    {
        VkShaderModule result;

        VkShaderModuleCreateInfo shader_module_create_info =
            wvk::types::VkShaderModuleCreateInfo();
        shader_module_create_info.codeSize = in_code_size;
        shader_module_create_info.pCode = in_code;

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

    inline VkShaderModule CreateShaderModule(
        const VkDevice & in_device,
        const char * in_code,
        const std::size_t& in_code_size
        ) {
        return CreateShaderModule(in_device,
                                  reinterpret_cast<const std::uint32_t *>(in_code),
                                  in_code_size);
    }

    inline std::vector<VkShaderModule> CreateShaderModules(
        WVkShaderStageInfo & out_vertex_stage,
        std::vector<VkPipelineShaderStageCreateInfo> & out_shader_stages,
        const VkDevice & in_device,
        const std::vector<WVkShaderStageInfo> & stage_infos) {

        out_shader_stages.clear();
        out_shader_stages.resize(stage_infos.size());
        
        std::vector<VkShaderModule> shader_modules(stage_infos.size(), VK_NULL_HANDLE);

        const WVkShaderStageInfo * vertex_shader_stage = nullptr;

        for (uint32_t i = 0; i < stage_infos.size(); i++)
        {
            out_shader_stages[i] = {};
            out_shader_stages[i].pNext = VK_NULL_HANDLE;
            out_shader_stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            out_shader_stages[i].stage = wvk::types::ToShaderStageFlagBits(
                stage_infos[i].type
                );

            if (stage_infos[i].type == wct::render::EShaderStageFlag::Vertex)
            {
                vertex_shader_stage = &stage_infos[i];
            }

            shader_modules[i] = CreateShaderModule(in_device,
                                                   stage_infos[i].code.data(),
                                                   stage_infos[i].code.size());

            out_shader_stages[i].module = shader_modules[i];
            out_shader_stages[i].pName = stage_infos[i].entry_point.c_str();
        }

        if (vertex_shader_stage == nullptr)
        {
            throw std::runtime_error("Vertex shader stage not found!");
        }

        out_vertex_stage = *vertex_shader_stage;

        return shader_modules;
    }


}
