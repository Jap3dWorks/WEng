#pragma once

// #include "WCoreTypes/WRenderTypes.hpp"
// #include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkTypes.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include <vulkan/vulkan_core.h>

/**
 * Functions to make easiest build a render plane.
 * Render Plane is a plane with a rendered texture.
 * Used in steps like postprocess.
 */
namespace wvk::render_plane {

    inline std::array<float, 16> RenderPlaneVertex() noexcept {
        return {
            -1.f, -1.f, 0.f, 0.f,
            1.f, -1.f, 1.f, 0.f,
            1.f, 1.f, 1.f, 1.f,
            -1.f, 1.f, 0.f, 1.f
        };
    }

    inline std::array<std::uint32_t, 6> RenderPlaneIndexes() noexcept {
        return { 2,1,0,0,3,2 };
    }

    WNODISCARD inline VkSampler CreateRenderPlaneSampler(const VkDevice & in_device) {
        VkSampler result;
        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.anisotropyEnable = VK_FALSE;
        sampler_info.maxAnisotropy = 0;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;

        if(vkCreateSampler(in_device,
                           &sampler_info,
                           nullptr,
                           &result) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sampler!");
        }

        return result;
    }

    /**
     * helper function to config VkPipelineShaderStageCreateInfo.
     */
    inline void UpdateVkPipelineShaderStageCreateInfo(
        VkPipelineShaderStageCreateInfo in_data[2],
        const VkShaderModule & in_shader_module
        ) {

        // VkShaderStageFlagBits
        in_data[0] = wvk::types::VkPipelineShaderStageCreateInfo();
        in_data[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        in_data[0].module=in_shader_module;
        in_data[0].pName="vsMain";

        in_data[1] = wvk::types::VkPipelineShaderStageCreateInfo();
        in_data[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        in_data[1].module=in_shader_module;
        in_data[1].pName="fsMain";
    }

    inline void UpdateVertexInputAttributeDescriptor(
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

    inline VkVertexInputBindingDescription VertBindingDescrpt() {
        VkVertexInputBindingDescription result{};
        result.binding = 0;
        result.stride = sizeof(float) * 2 + sizeof(float) * 2;  // position + uv
        result.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return result;
    }

    inline VkPipelineVertexInputStateCreateInfo VkPipelineVertexInputStateCreateInfo(
        const VkVertexInputBindingDescription & in_binding_desc,
        VkVertexInputAttributeDescription * in_attr_desc_data,
        const std::uint32_t & in_attr_desc_count
        )
    {
        
        ::VkPipelineVertexInputStateCreateInfo vertex_input_info =
            wvk::types::CreateVkPipelineVertexInputStateCreateInfo();
        vertex_input_info.vertexBindingDescriptionCount=1;
        vertex_input_info.pVertexBindingDescriptions = &in_binding_desc;
        vertex_input_info.vertexAttributeDescriptionCount=in_attr_desc_count;
        vertex_input_info.pVertexAttributeDescriptions=in_attr_desc_data;

        return vertex_input_info;
    }

    inline VkPipelineInputAssemblyStateCreateInfo VkPipelineInputAssemblyStateCreateInfo() {
        ::VkPipelineInputAssemblyStateCreateInfo input_assembly =
            wvk::types::CreateVkPipelineInputAssemblyStateCreateInfo();
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;
        
        return input_assembly;
    }

    inline VkPipelineViewportStateCreateInfo VkPipelineViewportStateCreateInfo()
    {
        ::VkPipelineViewportStateCreateInfo viewport_state =
            wvk::types::CreateVkPipelineViewportStateCreateInfo();
        viewport_state.viewportCount=1;
        viewport_state.scissorCount=1;
        
        return viewport_state;
    }

    inline VkPipelineRasterizationStateCreateInfo VkPipelineRasterizationStateCreateInfo() {
        ::VkPipelineRasterizationStateCreateInfo rasterizer =
            wvk::types::CreateVkPipelineRasterizationStateCreateInfo();
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

    inline VkPipelineMultisampleStateCreateInfo VkPipelineMultisampleStateCreateInfo() {
        ::VkPipelineMultisampleStateCreateInfo multisampling =
            wvk::types::CreateVkPipelineMultisampleStateCreateInfo();
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;  // no msaa
        
        return multisampling;        
    }

    inline VkPipelineDepthStencilStateCreateInfo VkPipelineDepthStencilStateCreateInfo() {
        ::VkPipelineDepthStencilStateCreateInfo depth_stencil =
            wvk::types::CreateVkPipelineDepthStencilStateCreateInfo();
        depth_stencil.depthTestEnable = VK_FALSE;
        depth_stencil.depthWriteEnable = VK_FALSE;
        
        return depth_stencil;
    }

    inline VkPipelineColorBlendAttachmentState VkPipelineColorBlendAttachmentState()
    {
        ::VkPipelineColorBlendAttachmentState color_blend_attachment{};
        color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        
        return color_blend_attachment;
    }

    inline VkPipelineColorBlendStateCreateInfo VkPipelineColorBlendStateCreateInfo(
        const struct VkPipelineColorBlendAttachmentState & in_color_blend_attachment
        )
    {
        ::VkPipelineColorBlendStateCreateInfo color_blending =
            wvk::types::CreateVkPipelineColorBlendStateCreateInfo();
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

    inline VkPipelineDynamicStateCreateInfo VkPipelineDynamicStateCreateInfo(
        std::vector<VkDynamicState> & out_dyn_states
        )
    {
        out_dyn_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        ::VkPipelineDynamicStateCreateInfo dynamic_state =
            wvk::types::CreateVkPipelineDynamicStateCreateInfo();
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(out_dyn_states.size());
        dynamic_state.pDynamicStates = out_dyn_states.data();

        return dynamic_state;
    }

    inline VkPipelineLayout VkPipelineLayout(
        const VkDevice & in_device,
        const VkDescriptorSetLayout & in_desc_lay
        ) {

        ::VkPipelineLayout result;
        
        VkPipelineLayoutCreateInfo pipeline_layout_info =
            wvk::types::CreateVkPipelineLayoutCreateInfo();

        pipeline_layout_info.setLayoutCount = 1;         // slayouts.size();
        pipeline_layout_info.pSetLayouts = &in_desc_lay; // slayouts.data();

        wvk::vulkan::ExecVkProcChecked(vkCreatePipelineLayout,
                                   "Failed to create pipeline layout!",
                                   in_device,
                                   &pipeline_layout_info,
                                   nullptr,
                                   &result);

        return result;
    }

    inline VkGraphicsPipelineCreateInfo VkGraphicsPipelineCreateInfo(
        const std::uint32_t & in_shader_stages_count,
        const VkPipelineShaderStageCreateInfo * in_shader_stages,
        const ::VkPipelineVertexInputStateCreateInfo * in_vertex_input_info,
        const ::VkPipelineInputAssemblyStateCreateInfo * in_input_assembly,
        const ::VkPipelineViewportStateCreateInfo * in_viewport_state,
        const ::VkPipelineRasterizationStateCreateInfo * in_rasterizer,
        const ::VkPipelineMultisampleStateCreateInfo * in_multisampling,
        const ::VkPipelineDepthStencilStateCreateInfo * in_depth_stencil,
        const ::VkPipelineColorBlendStateCreateInfo * in_color_blending,
        const ::VkPipelineDynamicStateCreateInfo * in_dynamic_state,
        ::VkPipelineLayout in_pipeline_layout
        )
    {
        ::VkGraphicsPipelineCreateInfo pipeline_info =
            wvk::types::CreateVkGraphicsPipelineCreateInfo();
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

    inline VkPipeline VkPipeline(
        VkFormat * in_color_attachment_format,
        std::uint32_t in_color_attachment_count,
        ::VkGraphicsPipelineCreateInfo in_graphics_pipeline_create_info,
        const VkDevice & in_device
        )
    {
        VkPipelineRenderingCreateInfo rendering_info =
            wvk::types::CreateVkPipelineRenderingCreateInfo();
        rendering_info.colorAttachmentCount = in_color_attachment_count;
        rendering_info.pColorAttachmentFormats = in_color_attachment_format;

        in_graphics_pipeline_create_info.pNext = &rendering_info;

        ::VkPipeline result;

        wvk::vulkan::ExecVkProcChecked(
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
