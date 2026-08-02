#pragma once

#include "WVulkan/Vk/WVkShader.hpp"
#include "WVulkan/Vk/WVkPipeline.hpp"
#include "WVulkan/RAII/UBOManager/DynamicUBOManager.hpp"
#include "WVulkan/RAII/DescriptorPool.hpp"

#include <vulkan/vulkan_core.h>

namespace wvk::raii::pipelines::gbuffer_lib {

    template<std::uint8_t FramesInFlight>
    struct Binding {
        wcr::wid::WTypeAssetIndexId mesh_id;

        std::uint32_t model_ubo_offset;
        std::array<VkDescriptorSet, FramesInFlight> model_ubo_descriptor_set;

        std::uint32_t param_ubo_offset;
        std::array<VkDescriptorSet, FramesInFlight> param_descripor_set;
    };

    template<std::uint8_t FramesInFlight>
    struct Collection {
        wvk::raii::DescriptorPool<10,10,70,90> descriptor_pool{};
        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> ubo_manager{};
    };

    inline auto CreatePipeline(
        const VkDevice & in_device,
        const std::vector<VkDescriptorSetLayout> & in_desc_layouts,
        const std::vector<WVkShaderStageInfo> & in_shader_stage_infos
        ) {

        VkPipeline render_pipeline;
        VkPipelineLayout pipeline_layout;

        auto [shader_stages, shader_modules] = wvk::shader::CreateShaderModules(
            in_device,
            in_shader_stage_infos
            );

        VkPipelineVertexInputStateCreateInfo vertex_input_info =
            wvk::types::VkPipelineVertexInputStateCreateInfo();

        vertex_input_info.vertexBindingDescriptionCount =
            static_cast<uint32_t>(
                wvk::pipeline::GEO_VERTEX_INPUT_BINDING_DESCRIPTION.size()
                );
        vertex_input_info.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(
                wvk::pipeline::GEO_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION.size()
                );
        vertex_input_info.pVertexBindingDescriptions =
            wvk::pipeline::GEO_VERTEX_INPUT_BINDING_DESCRIPTION.data();
        vertex_input_info.pVertexAttributeDescriptions =
            wvk::pipeline::GEO_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly;
        input_assembly = 
            wvk::types::VkPipelineInputAssemblyStateCreateInfo();
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewport_state =
            wvk::types::VkPipelineViewportStateCreateInfo();
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer =
            wvk::types::VkPipelineRasterizationStateCreateInfo();
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // because of y-flip in the projection matrix
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling =
            wvk::types::VkPipelineMultisampleStateCreateInfo();
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // TODO: Use weng::vk:vkstructs
        VkPipelineDepthStencilStateCreateInfo depth_stencil =
            wvk::types::VkPipelineDepthStencilStateCreateInfo();
        depth_stencil.depthTestEnable = VK_TRUE;
        depth_stencil.depthWriteEnable = VK_TRUE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depth_stencil.depthBoundsTestEnable = VK_FALSE;
        depth_stencil.stencilTestEnable = VK_FALSE;

        // dynamic rendering color formats
        std::array<VkFormat, WVK_GBUFFERS_COUNT-1> color_formats  {
            WVK_GBUFFER_RENDER_COLOR_FORMAT,      // albedo
            WVK_GBUFFER_RENDER_EMISSION_FORMAT,   // emission
            WVK_GBUFFER_RENDER_NORMAL_FORMAT,     // normal
            WVK_GBUFFER_RENDER_ORM_FORMAT,       // metallic roughness AO
            WVK_GBUFFER_RENDER_EXTRA01_FORMAT     // extra 01
        };

        std::array<VkPipelineColorBlendAttachmentState, WVK_GBUFFERS_COUNT-1>
            color_blend_attachments;
        
        for(auto & cblend_attch : color_blend_attachments) {
            cblend_attch = wvk::types::VkPipelineColorBlendAttachmentState();
            cblend_attch.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
            
            // Overrides the color
            cblend_attch.blendEnable = VK_FALSE;  
        }
        
        VkPipelineColorBlendStateCreateInfo color_blend_create_info =
            wvk::types::VkPipelineColorBlendStateCreateInfo();
        color_blend_create_info.logicOpEnable = VK_FALSE;
        color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
        color_blend_create_info.attachmentCount = color_blend_attachments.size();
        color_blend_create_info.pAttachments = color_blend_attachments.data();
        color_blend_create_info.blendConstants[0] = 0.f;
        color_blend_create_info.blendConstants[1] = 0.f;
        color_blend_create_info.blendConstants[2] = 0.f;
        color_blend_create_info.blendConstants[3] = 0.f;

        std::array<VkDynamicState,2> dynamic_states;
        dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamic_state_create_info =
            wvk::types::VkPipelineDynamicStateCreateInfo();
        dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state_create_info.pDynamicStates = dynamic_states.data();

        VkPipelineLayoutCreateInfo pipeline_layout_info =
            wvk::types::VkPipelineLayoutCreateInfo();
        pipeline_layout_info.setLayoutCount = in_desc_layouts.size();
        pipeline_layout_info.pSetLayouts = in_desc_layouts.data();

        VkGraphicsPipelineCreateInfo pipeline_create_info =
            wvk::types::VkGraphicsPipelineCreateInfo();
        pipeline_create_info.stageCount = static_cast<uint32_t>(shader_stages.size());
        pipeline_create_info.pStages = shader_stages.data();
        pipeline_create_info.pVertexInputState = &vertex_input_info;
        pipeline_create_info.pInputAssemblyState = &input_assembly;
        pipeline_create_info.pViewportState = &viewport_state;
        pipeline_create_info.pRasterizationState = &rasterizer;
        pipeline_create_info.pMultisampleState = &multisampling;
        pipeline_create_info.pDepthStencilState = &depth_stencil;
        pipeline_create_info.pColorBlendState = &color_blend_create_info; // 
        pipeline_create_info.pDynamicState = &dynamic_state_create_info;
    
        pipeline_create_info.renderPass = VK_NULL_HANDLE;
        pipeline_create_info.subpass = 0;
        pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

        // Dynamic Rendering

        VkPipelineRenderingCreateInfo rendering_info =
            wvk::types::VkPipelineRenderingCreateInfo();
        rendering_info.colorAttachmentCount = color_formats.size();
        rendering_info.pColorAttachmentFormats = color_formats.data();
        rendering_info.depthAttachmentFormat = WVK_GBUFFER_RENDER_DEPTH_FORMAT;
        rendering_info.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        pipeline_create_info.pNext = &rendering_info;

        wvk::vulkan::ExecVkProcChecked(
            vkCreatePipelineLayout,
            "Failed to create pipeline layout!",
            in_device,
            &pipeline_layout_info,
            nullptr,
            &pipeline_layout
            );

        pipeline_create_info.layout = pipeline_layout;
        
        wvk::vulkan::ExecVkProcChecked(
            vkCreateGraphicsPipelines,
            "Failed to create graphics pipeline!",
            in_device,
            VK_NULL_HANDLE,
            1,
            &pipeline_create_info,
            nullptr,
            &render_pipeline
            );

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

    
};


