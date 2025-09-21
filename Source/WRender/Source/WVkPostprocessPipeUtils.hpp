#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkanUtils.hpp"
#include "WShaderUtils.hpp"

#include <cstddef>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

namespace WVkPostprocessPipeUtils {

    struct WVkPostprocessVertex {
        glm::vec2 position;
        glm::vec2 tex_coords;
    };

    inline WVkShaderStageInfo BuildPostprocessShaderStageInfo(
        const char * in_shader_file_path,
        const char * in_entry_point,
        EShaderStageFlag in_shader_type
        ) {
        WVkShaderStageInfo result;

        result.code = WShaderUtils::ReadShader(in_shader_file_path);
        result.entry_point = in_entry_point;
        result.type = in_shader_type;

        if (in_shader_type == EShaderStageFlag::Vertex) {
            result.attribute_descriptors.resize(2);

            result.attribute_descriptors[0].binding=0;
            result.attribute_descriptors[0].location=0;
            result.attribute_descriptors[0].format = VK_FORMAT_R32G32_SFLOAT;
            result.attribute_descriptors[0].offset = 0;

            result.attribute_descriptors[1].binding=0;
            result.attribute_descriptors[1].location=1;
            result.attribute_descriptors[1].format=VK_FORMAT_R32G32_SFLOAT;
            result.attribute_descriptors[1].offset= offsetof(WVkPostprocessVertex, tex_coords);

            result.binding_descriptors.resize(1);
            result.binding_descriptors[0].binding=0;
            result.binding_descriptors[0].stride=sizeof(WVkPostprocessVertex);
            result.binding_descriptors[0].inputRate=VK_VERTEX_INPUT_RATE_VERTEX;
        }

        return result;

    }

    inline void CreatePostprocessPipeline(WVkRenderPipelineInfo & out_pipeline_info,
                                          const WVkDeviceInfo & in_device,
                                          const std::vector<VkDescriptorSetLayout> & in_desc_lay,
                                          const std::vector<WVkShaderStageInfo> & in_shader_stage_infos) {

        WVkShaderStageInfo wvertex_stage_info;
        std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
    
        std::vector<VkShaderModule> shader_modules =
            WVulkanUtils::CreateShaderModules(
                wvertex_stage_info, shader_stages, in_device.vk_device, in_shader_stage_infos
                );

        VkPipelineVertexInputStateCreateInfo vertex_input_info;
        VkPipelineInputAssemblyStateCreateInfo input_assembly;
        VkPipelineViewportStateCreateInfo viewport_state;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineDepthStencilStateCreateInfo depth_stencil;
        VkPipelineColorBlendAttachmentState color_blend_attachment;
        VkPipelineColorBlendStateCreateInfo color_blend_create_info;
        std::vector<VkDynamicState> dynamic_states;
        VkPipelineDynamicStateCreateInfo dynamic_state;
        VkPipelineLayoutCreateInfo pipeline_layout_info;
        VkFormat color_format;
        VkFormat depth_format;
        VkPipelineRenderingCreateInfo rendering_info;
        VkGraphicsPipelineCreateInfo pipeline_create_info;

        WVulkanUtils::InitializeDefaultPipelineStructs(
            wvertex_stage_info, shader_stages, in_desc_lay,
            vertex_input_info, input_assembly, viewport_state, rasterizer,
            multisampling, depth_stencil, color_blend_attachment, color_blend_create_info,
            dynamic_states, dynamic_state,
            color_format, depth_format, rendering_info,
            pipeline_layout_info, pipeline_create_info
            );

        // Disable depth testing

        depth_stencil.depthTestEnable = VK_FALSE;
        depth_stencil.depthWriteEnable = VK_FALSE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;

        // pipeline_create_info.pDepthStencilState=VK_NULL_HANDLE;

        if (vkCreatePipelineLayout(
                in_device.vk_device,
                &pipeline_layout_info,
                nullptr,
                &out_pipeline_info.pipeline_layout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        pipeline_create_info.layout = out_pipeline_info.pipeline_layout;

        if (vkCreateGraphicsPipelines(
                in_device.vk_device,
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info,
                nullptr,
                &out_pipeline_info.pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }

        for (auto& shader_module : shader_modules)
        {
            vkDestroyShaderModule(
                in_device.vk_device, 
                shader_module,
                nullptr
                );
        }
    }

    inline void CreateDescSetPool(
        WVkDescriptorPoolInfo & out_descriptor_pool_info,
        const WVkDeviceInfo & in_device
        ) {
        std::array<VkDescriptorPoolSize, 2> pool_sizes;

        pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount = 5;

        pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[1].descriptorCount = 10;

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes = pool_sizes.data();
        
        pool_info.maxSets = 15;

        if (vkCreateDescriptorPool(
                in_device.vk_device,
                &pool_info,
                nullptr,
                &out_descriptor_pool_info.descriptor_pool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
    }

    inline void UpdateDSL_DefaultGlobalBindings(WVkDescriptorSetLayoutInfo & out_dsl) {
        VkDescriptorSetLayoutBinding rbinding1{};

        rbinding1.binding=0;
        rbinding1.descriptorCount = 1;
        rbinding1.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        rbinding1.pImmutableSamplers = nullptr;
        rbinding1.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        out_dsl.bindings = {
            rbinding1
        };

    }

    inline void CreateGlobalResourcesDescPool(
        WVkDescriptorPoolInfo & out_descriptor_pool_info,
        const WVkDeviceInfo & in_device
        ) {

        std::array<VkDescriptorPoolSize, 2> pool_sizes;

        pool_sizes[0]={};
        pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount = 10;
        
        pool_sizes[1]={};
        pool_sizes[1].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[1].descriptorCount = 10;

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes = pool_sizes.data();
        pool_info.maxSets = static_cast<uint32_t>(35);

        if (vkCreateDescriptorPool(
                in_device.vk_device,
                &pool_info,
                nullptr,
                &out_descriptor_pool_info.descriptor_pool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool!");
        }

    }



}
