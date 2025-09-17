#pragma once

#include "WVulkan/WVulkanStructs.hpp"

#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <vulkan/vulkan_core.h>
#include <array>
#include <stdexcept>
#include <vector>

namespace WVulkanUtils {

    /**
     * @brief Enum representing the shader stage flags for Vulkan.
     */
    inline VkShaderStageFlagBits ToShaderStageFlagBits(const EShaderType & type) {
        switch (type)
        {
        case EShaderType::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case EShaderType::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case EShaderType::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            throw std::runtime_error("Invalid shader type!");
        }
    }

    template<std::uint32_t N>
    VkRenderPass CreateRenderPass(const std::array<VkAttachmentDescription, N> & in_attachment_descriptors,
                                  const VkSubpassDescription & in_subpass_descriptor,
                                  const VkDevice & in_device)
    {

        std::array<VkSubpassDependency, 2> dependencies{};
    
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;  // first subpass
        // End of previous commands
        dependencies[0].srcStageMask =
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_NONE_KHR;
        // Read/write from/to depth
        dependencies[0].dstStageMask =
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].dstAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        // Write to attachment
        dependencies[0].dstStageMask |=
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        dependencies[1].srcSubpass=0;
        dependencies[1].dstSubpass=VK_SUBPASS_EXTERNAL;
        // End of write to attachment
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        // Attachment later read
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = static_cast<uint32_t>(in_attachment_descriptors.size());
        render_pass_info.pAttachments = in_attachment_descriptors.data();
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &in_subpass_descriptor;
        render_pass_info.dependencyCount = static_cast<std::uint32_t>(dependencies.size());
        render_pass_info.pDependencies = dependencies.data();

        VkRenderPass result;

        if (vkCreateRenderPass(in_device,
                               &render_pass_info,
                               nullptr,
                               &result) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create offscreen render pass!");
        }

        return result;
    }

    inline VkShaderModule CreateShaderModule(
        const VkDevice & in_device,
        const std::uint32_t * in_code,
        const std::size_t& in_code_size
    )
    {
        VkShaderModule result;

        VkShaderModuleCreateInfo shader_module_create_info{};
        shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_info.codeSize = in_code_size;
        shader_module_create_info.pCode = in_code;

        if (vkCreateShaderModule(
                in_device, 
                &shader_module_create_info, 
                nullptr, 
                &result
                ) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module!");
        }

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
            out_shader_stages[i].stage = ToShaderStageFlagBits(
                stage_infos[i].type
                );

            if (stage_infos[i].type == EShaderType::Vertex)
            {
                vertex_shader_stage = &stage_infos[i];
            }

            shader_modules[i] = WVulkanUtils::CreateShaderModule(in_device,
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

    inline void CopyVkBuffer(
        const VkDevice & device,
        const VkCommandPool & command_pool,
        const VkQueue & graphics_queue,
        const VkBuffer & src_buffer,
        const VkBuffer & dst_buffer,
        const VkDeviceSize & size
        ) {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = command_pool;
        alloc_info.commandBufferCount = 1;
        alloc_info.pNext = nullptr;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);

        VkBufferCopy copy_region{};
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphics_queue);

        vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
    }

    inline void InitializeDefaultPipelineStructs(
        const WVkShaderStageInfo & in_wvertex_stage_info,
        const std::vector<VkPipelineShaderStageCreateInfo> & in_shader_stages,
        const std::vector<VkDescriptorSetLayout> & in_descset_layouts,
        VkPipelineVertexInputStateCreateInfo & out_vertex_input_info,
        VkPipelineInputAssemblyStateCreateInfo & out_input_assembly,
        VkPipelineViewportStateCreateInfo & out_viewport_state,
        VkPipelineRasterizationStateCreateInfo & out_rasterizer,
        VkPipelineMultisampleStateCreateInfo & out_multisampling,
        VkPipelineDepthStencilStateCreateInfo & out_depth_stencil,
        VkPipelineColorBlendAttachmentState & out_color_blend_attachment,
        VkPipelineColorBlendStateCreateInfo & out_color_blend_create_info,
        std::vector<VkDynamicState> & out_dynamic_states,
        VkPipelineDynamicStateCreateInfo & out_dynamic_state,
        VkFormat & out_color_format,
        VkFormat & out_depth_format,
        VkPipelineRenderingCreateInfo & out_rendering_info,
        VkPipelineLayoutCreateInfo & out_pipeline_layout_info,
        VkGraphicsPipelineCreateInfo & out_create_pipeline_info
        ) {
        // VkPipelineVertexInputStateCreateInfo vertex_input_info{};

        out_vertex_input_info = {};
        out_vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        out_vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(
            in_wvertex_stage_info.binding_descriptors.size());
        out_vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(
            in_wvertex_stage_info.attribute_descriptors.size());
        out_vertex_input_info.pVertexBindingDescriptions =
            in_wvertex_stage_info.binding_descriptors.data();
        out_vertex_input_info.pVertexAttributeDescriptions =
            in_wvertex_stage_info.attribute_descriptors.data();

        out_input_assembly = {};
        out_input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        out_input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        out_input_assembly.primitiveRestartEnable = VK_FALSE;

        out_viewport_state = {};
        out_viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        out_viewport_state.viewportCount = 1;
        out_viewport_state.scissorCount = 1;

        out_rasterizer={};
        out_rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        out_rasterizer.depthClampEnable = VK_FALSE;
        out_rasterizer.rasterizerDiscardEnable = VK_FALSE;
        out_rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        out_rasterizer.lineWidth = 1.0f;
        out_rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        out_rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // because of y-flip in the projection matrix
        out_rasterizer.depthBiasEnable = VK_FALSE;

        out_multisampling = {};
        out_multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        out_multisampling.sampleShadingEnable = VK_FALSE;
        out_multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //in_device.msaa_samples;

        out_depth_stencil = {};
        out_depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        out_depth_stencil.depthTestEnable = VK_TRUE;
        out_depth_stencil.depthWriteEnable = VK_TRUE;
        out_depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
        out_depth_stencil.depthBoundsTestEnable = VK_FALSE;
        out_depth_stencil.stencilTestEnable = VK_FALSE;

        // TODO gbuffers attachments
        out_color_blend_attachment = {};
        out_color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        out_color_blend_attachment.blendEnable = VK_FALSE;

        out_color_blend_create_info={};
        out_color_blend_create_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        out_color_blend_create_info.logicOpEnable = VK_FALSE;
        out_color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
        out_color_blend_create_info.attachmentCount = 1;
        out_color_blend_create_info.pAttachments = &out_color_blend_attachment;
        out_color_blend_create_info.blendConstants[0] = 0.f;
        out_color_blend_create_info.blendConstants[1] = 0.f;
        out_color_blend_create_info.blendConstants[2] = 0.f;
        out_color_blend_create_info.blendConstants[3] = 0.f;

        out_dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        out_dynamic_state = {};
        out_dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        out_dynamic_state.dynamicStateCount = static_cast<uint32_t>(out_dynamic_states.size());
        out_dynamic_state.pDynamicStates = out_dynamic_states.data();

        out_pipeline_layout_info = {};
        out_pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        out_pipeline_layout_info.setLayoutCount = in_descset_layouts.size();
        out_pipeline_layout_info.pSetLayouts = in_descset_layouts.data();

        out_create_pipeline_info = {};
        out_create_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        out_create_pipeline_info.stageCount = static_cast<uint32_t>(in_shader_stages.size());
        out_create_pipeline_info.pStages = in_shader_stages.data();
        out_create_pipeline_info.pVertexInputState = &out_vertex_input_info;
        out_create_pipeline_info.pInputAssemblyState = &out_input_assembly;
        out_create_pipeline_info.pViewportState = &out_viewport_state;
        out_create_pipeline_info.pRasterizationState = &out_rasterizer;
        out_create_pipeline_info.pMultisampleState = &out_multisampling;
        out_create_pipeline_info.pDepthStencilState = &out_depth_stencil;
        out_create_pipeline_info.pColorBlendState = &out_color_blend_create_info;
        out_create_pipeline_info.pDynamicState = &out_dynamic_state;
    
        out_create_pipeline_info.renderPass = VK_NULL_HANDLE;
        out_create_pipeline_info.subpass = 0;
        out_create_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

        // Dynamic Rendering
        // TODO gBuffers attachments (other config function)

        out_color_format=VK_FORMAT_B8G8R8A8_SRGB;
        out_depth_format = VK_FORMAT_D32_SFLOAT;

        out_rendering_info={};
        out_rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        out_rendering_info.colorAttachmentCount = 1;
        out_rendering_info.pColorAttachmentFormats = &out_color_format;
        out_rendering_info.depthAttachmentFormat = out_depth_format;
        out_rendering_info.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        // --

        out_create_pipeline_info.pNext = &out_rendering_info;

    }

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

}
