#pragma once

#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include <vulkan/vulkan_core.h>

namespace WVkPipelineHelper {
    /**
     * helper function to config VkPipelineShaderStageCreateInfo.
     */
    inline void RenderPlane_UpdateVkPipelineShaderStageCreateInfo(
        VkPipelineShaderStageCreateInfo in_data[2],
        const VkShaderModule & in_shader_module
        ) {

        // VkShaderStageFlagBits
        in_data[0] = WVulkan::VkStructs::CreateVkPipelineShaderStageCreateInfo();
        in_data[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        in_data[0].module=in_shader_module;
        in_data[0].pName="vsMain";

        in_data[1] = WVulkan::VkStructs::CreateVkPipelineShaderStageCreateInfo();
        in_data[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        in_data[1].module=in_shader_module;
        in_data[1].pName="fsMain";

    }

    inline void RenderPlane_UpdateVertexInputAttributeDescriptor(
        VkVertexInputAttributeDescription in_data[2]
        ) {
        in_data[0].binding=0;
        in_data[0].location=0;
        // in_data[0].format=VK_FORMAT_R32G32B32_SFLOAT; // RG?
        in_data[0].format=VK_FORMAT_R32G32_SFLOAT;
        in_data[0].offset=0;  // position

        in_data[1].binding=0;
        in_data[1].location=1;
        in_data[1].format = VK_FORMAT_R32G32_SFLOAT;
        in_data[1].offset = sizeof(float) * 2; // uv
    }

    inline VkVertexInputBindingDescription RenderPlane_VertBindingDescrpt() {
        VkVertexInputBindingDescription result{};
        result.binding = 0;
        result.stride = sizeof(float) * 2 + sizeof(float) * 2;  // position + uv
        result.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return result;
    }

    inline VkPipelineVertexInputStateCreateInfo RenderPlane_VkPipelineVertexInputStateCreateInfo(
        const VkVertexInputBindingDescription & in_binding_desc,
        VkVertexInputAttributeDescription * in_attr_desc_data,
        const std::uint32_t & in_attr_desc_count
        )
    {
        
        VkPipelineVertexInputStateCreateInfo vertex_input_info =
            WVulkan::VkStructs::CreateVkPipelineVertexInputStateCreateInfo();
        vertex_input_info.vertexBindingDescriptionCount=1;
        vertex_input_info.pVertexBindingDescriptions = &in_binding_desc;
        vertex_input_info.vertexAttributeDescriptionCount=in_attr_desc_count;
        vertex_input_info.pVertexAttributeDescriptions=in_attr_desc_data;

        return vertex_input_info;
    }

    inline VkPipelineInputAssemblyStateCreateInfo RenderPlane_VkPipelineInputAssemblyStateCreateInfo() {
        VkPipelineInputAssemblyStateCreateInfo input_assembly =
            WVulkan::VkStructs::CreateVkPipelineInputAssemblyStateCreateInfo();
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;
        
        return input_assembly;
    }

    inline VkPipelineViewportStateCreateInfo RenderPlane_VkPipelineViewportStateCreateInfo()
    {
        VkPipelineViewportStateCreateInfo viewport_state =
            WVulkan::VkStructs::CreateVkPipelineViewportStateCreateInfo();
        viewport_state.viewportCount=1;
        viewport_state.scissorCount=1;
        
        return viewport_state;
    }

    inline VkPipelineRasterizationStateCreateInfo RenderPlane_VkPipelineRasterizationStateCreateInfo() {
        VkPipelineRasterizationStateCreateInfo rasterizer =
            WVulkan::VkStructs::CreateVkPipelineRasterizationStateCreateInfo();
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        // because of y-flip in the projection matrix
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        
        return rasterizer;
    }

    inline VkPipelineMultisampleStateCreateInfo RenderPlane_VkPipelineMultisampleStateCreateInfo() {
        VkPipelineMultisampleStateCreateInfo multisampling =
            WVulkan::VkStructs::CreateVkPipelineMultisampleStateCreateInfo();
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;  // no msaa
        
        return multisampling;        
    }

    inline VkPipelineDepthStencilStateCreateInfo RenderPlane_VkPipelineDepthStencilStateCreateInfo() {
        VkPipelineDepthStencilStateCreateInfo depth_stencil =
            WVulkan::VkStructs::CreateVkPipelineDepthStencilStateCreateInfo();
        depth_stencil.depthTestEnable = VK_FALSE;
        depth_stencil.depthWriteEnable = VK_FALSE;
        
        return depth_stencil;
    }

    inline VkPipelineColorBlendAttachmentState RenderPlane_VkPipelineColorBlendAttachmentState()
    {
        VkPipelineColorBlendAttachmentState color_blend_attachment{};
        color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        
        return color_blend_attachment;
    }

    inline VkPipelineColorBlendStateCreateInfo RenderPlane_VkPipelineColorBlendStateCreateInfo(
        const VkPipelineColorBlendAttachmentState & in_color_blend_attachment
        )
    {
        VkPipelineColorBlendStateCreateInfo color_blending =
            WVulkan::VkStructs::CreateVkPipelineColorBlendStateCreateInfo();
        color_blending.logicOpEnable = VK_FALSE;
        color_blending.logicOp = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &in_color_blend_attachment;
        color_blending.blendConstants[0] = 0.f;
        color_blending.blendConstants[1] = 0.f;
        color_blending.blendConstants[2] = 0.f;
        color_blending.blendConstants[3] = 0.f;

        return color_blending;
    }

    inline VkPipelineDynamicStateCreateInfo RenderPlane_VkPipelineDynamicStateCreateInfo(
        std::vector<VkDynamicState> & out_dyn_states
        )
    {
        out_dyn_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamic_state =
            WVulkan::VkStructs::CreateVkPipelineDynamicStateCreateInfo();
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(out_dyn_states.size());
        dynamic_state.pDynamicStates = out_dyn_states.data();

        return dynamic_state;
    }

    inline VkPipelineLayout RenderPlane_VkPipelineLayout(
        const VkDevice & in_device,
        const VkDescriptorSetLayout & in_desc_lay
        ) {

        VkPipelineLayout result;
        
        VkPipelineLayoutCreateInfo pipeline_layout_info =
            WVulkan::VkStructs::CreateVkPipelineLayoutCreateInfo();

        // Only one desc set, in the future I could need more (some global parameters).

        pipeline_layout_info.setLayoutCount = 1;         // slayouts.size();
        pipeline_layout_info.pSetLayouts = &in_desc_lay; // slayouts.data();

        WVulkan::ExecVkProcChecked(vkCreatePipelineLayout,
                                   "Failed to create pipeline layout!",
                                   in_device,
                                   &pipeline_layout_info,
                                   nullptr,
                                   &result);

        return result;

    }

    inline VkGraphicsPipelineCreateInfo RenderPlane_VkGraphicsPipelineCreateInfo(
        const std::uint32_t & in_shader_stages_count,
        const VkPipelineShaderStageCreateInfo * in_shader_stages,
        const VkPipelineVertexInputStateCreateInfo * in_vertex_input_info,
        const VkPipelineInputAssemblyStateCreateInfo * in_input_assembly,
        const VkPipelineViewportStateCreateInfo * in_viewport_state,
        const VkPipelineRasterizationStateCreateInfo * in_rasterizer,
        const VkPipelineMultisampleStateCreateInfo * in_multisampling,
        const VkPipelineDepthStencilStateCreateInfo * in_depth_stencil,
        const VkPipelineColorBlendStateCreateInfo * in_color_blending,
        const VkPipelineDynamicStateCreateInfo * in_dynamic_state,
        VkPipelineLayout in_pipeline_layout
        )
    {
        VkGraphicsPipelineCreateInfo pipeline_info =
            WVulkan::VkStructs::CreateVkGraphicsPipelineCreateInfo();
        pipeline_info.stageCount = in_shader_stages_count;
        pipeline_info.pStages = in_shader_stages;
        pipeline_info.pVertexInputState = in_vertex_input_info;
        pipeline_info.pInputAssemblyState = in_input_assembly;
        pipeline_info.pViewportState = in_viewport_state;
        pipeline_info.pRasterizationState = in_rasterizer;
        pipeline_info.pMultisampleState = in_multisampling;
        pipeline_info.pDepthStencilState = in_depth_stencil;
        pipeline_info.pColorBlendState = in_color_blending;
        pipeline_info.pDynamicState = in_dynamic_state;
        pipeline_info.layout = in_pipeline_layout;

        pipeline_info.renderPass = VK_NULL_HANDLE; // in_render_pass_info.render_pass;
        pipeline_info.subpass = 0;                 // out_pipeline_info.subpass;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

        return pipeline_info;
    }

    inline VkPipeline RenderPlane_VkPipeline(
        VkFormat * in_color_attachment_format,
        std::uint32_t in_color_attachment_count,
        VkGraphicsPipelineCreateInfo in_graphics_pipeline_create_info,
        const VkDevice & in_device
        )
    {
        VkPipelineRenderingCreateInfo rendering_info =
            WVulkan::VkStructs::CreateVkPipelineRenderingCreateInfo();
        rendering_info.colorAttachmentCount = in_color_attachment_count;
        rendering_info.pColorAttachmentFormats = in_color_attachment_format;

        in_graphics_pipeline_create_info.pNext = &rendering_info;

        VkPipeline result;

        WVulkan::ExecVkProcChecked(
            vkCreateGraphicsPipelines,
            "Failed to create graphics pipeline!",
            in_device,
            VK_NULL_HANDLE,
            1,
            &in_graphics_pipeline_create_info,
            nullptr,
            &result
            );

        return result;
    }

}
