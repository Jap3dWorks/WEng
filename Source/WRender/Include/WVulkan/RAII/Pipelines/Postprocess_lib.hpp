#pragma once

#include "WCore/WCore.hpp"
#include "WAssets/RenderPipeline.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/Vk/WVkShader.hpp"
#include "WVulkan/Vk/WVkRenderPlane.hpp"

#include <vulkan/vulkan_core.h>

namespace wvk::raii::pipelines::postprocess_lib {

    inline auto CreatePipeline(
        VkDevice in_device,
        std::vector<VkDescriptorSetLayout> const & desc_layouts,
        std::vector<WVkShaderStageInfo> const & shader_stage_infos
        ) {

        VkPipeline render_pipeline;
        VkPipelineLayout pipeline_layout;

        auto [shader_stages, shader_modules] = wvk::shader::CreateShaderModules(
            in_device, shader_stage_infos
            );

        auto vertex_input_info = wvk::types::VkPipelineVertexInputStateCreateInfo();
        
        vertex_input_info.vertexBindingDescriptionCount =
            static_cast<uint32_t>(
                wvk::render_plane::VERTEX_INPUT_BINDING_DESCRIPTION.size()
                );
        vertex_input_info.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(
                wvk::render_plane::VERTEX_INPUT_ATTRIBUTE_DESCRIPTION.size()
                );
        vertex_input_info.pVertexBindingDescriptions =
            wvk::render_plane::VERTEX_INPUT_BINDING_DESCRIPTION.data();
        vertex_input_info.pVertexAttributeDescriptions =
            wvk::render_plane::VERTEX_INPUT_ATTRIBUTE_DESCRIPTION.data();

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
        pipeline_layout_info.setLayoutCount = desc_layouts.size();
        pipeline_layout_info.pSetLayouts = desc_layouts.data();

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

        VkFormat color_format = WVK_POSTPROCESS_RENDER_COLOR_FORMAT;

        VkPipelineRenderingCreateInfo rendering_info{};
        rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachmentFormats = &color_format;
        rendering_info.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        pipeline_create_info.pNext = &rendering_info;

        // Disable depth testing

        depth_stencil.depthTestEnable = VK_FALSE;
        depth_stencil.depthWriteEnable = VK_FALSE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;

        if (vkCreatePipelineLayout(
                in_device,
                &pipeline_layout_info,
                nullptr,
                &pipeline_layout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        pipeline_create_info.layout = pipeline_layout;

        if (vkCreateGraphicsPipelines(
                in_device,
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info,
                nullptr,
                &render_pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }

        for (auto& shader_module : shader_modules)
        {
            vkDestroyShaderModule(
                in_device, 
                shader_module,
                nullptr
                );
        }

        return std::tuple{render_pipeline, pipeline_layout};

    }

}
