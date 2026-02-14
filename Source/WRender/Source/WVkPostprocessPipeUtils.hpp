#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
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

        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(
            wvertex_stage_info.binding_descriptors.size());
        vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(
            wvertex_stage_info.attribute_descriptors.size());
        vertex_input_info.pVertexBindingDescriptions =
            wvertex_stage_info.binding_descriptors.data();
        vertex_input_info.pVertexAttributeDescriptions =
            wvertex_stage_info.attribute_descriptors.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        // because of y-flip in the projection matrix
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //in_device.msaa_samples;

        VkPipelineDepthStencilStateCreateInfo depth_stencil;
        depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil.depthTestEnable = VK_TRUE;
        depth_stencil.depthWriteEnable = VK_TRUE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depth_stencil.depthBoundsTestEnable = VK_FALSE;
        depth_stencil.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState color_blend_attachment{};
        color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo color_blend_create_info{};
        color_blend_create_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_create_info.logicOpEnable = VK_FALSE;
        color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
        color_blend_create_info.attachmentCount = 1;
        color_blend_create_info.pAttachments = &color_blend_attachment;
        color_blend_create_info.blendConstants[0] = 0.f;
        color_blend_create_info.blendConstants[1] = 0.f;
        color_blend_create_info.blendConstants[2] = 0.f;
        color_blend_create_info.blendConstants[3] = 0.f;

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        std::vector<VkDynamicState> dynamic_states;
        dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        dynamic_state = {};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state.pDynamicStates = dynamic_states.data();

        VkPipelineLayoutCreateInfo pipeline_layout_info;
        pipeline_layout_info = {};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = in_desc_lay.size();
        pipeline_layout_info.pSetLayouts = in_desc_lay.data();

        VkGraphicsPipelineCreateInfo pipeline_create_info{};
        pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_create_info.stageCount = static_cast<uint32_t>(shader_stages.size());
        pipeline_create_info.pStages = shader_stages.data();
        pipeline_create_info.pVertexInputState = &vertex_input_info;
        pipeline_create_info.pInputAssemblyState = &input_assembly;
        pipeline_create_info.pViewportState = &viewport_state;
        pipeline_create_info.pRasterizationState = &rasterizer;
        pipeline_create_info.pMultisampleState = &multisampling;
        pipeline_create_info.pDepthStencilState = &depth_stencil;
        pipeline_create_info.pColorBlendState = &color_blend_create_info;
        pipeline_create_info.pDynamicState = &dynamic_state;
    
        pipeline_create_info.renderPass = VK_NULL_HANDLE;
        pipeline_create_info.subpass = 0;
        pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

        // VkFormat color_format = VK_FORMAT_B8G8R8A8_SRGB; // TODO: 16 bit colors
        VkFormat color_format = WENG_VK_POSTPROCESS_RENDER_COLOR_FORMAT;
        VkFormat depth_format = VK_FORMAT_D32_SFLOAT;
        // TODO check, depth pass is not required here

        VkPipelineRenderingCreateInfo rendering_info{};
        rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachmentFormats = &color_format;
        rendering_info.depthAttachmentFormat = depth_format;
        rendering_info.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        pipeline_create_info.pNext = &rendering_info;

        // Disable depth testing

        depth_stencil.depthTestEnable = VK_FALSE;
        depth_stencil.depthWriteEnable = VK_FALSE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;

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
        pool_sizes[0].descriptorCount = 32; // bytes
        
        pool_sizes[1]={};
        pool_sizes[1].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[1].descriptorCount = 10;

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes = pool_sizes.data();
        pool_info.maxSets = 32;

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
