#pragma once

#include "WCore/WStringUtils.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkanUtils.hpp"
#include "WShaderUtils.hpp"

#include <glm/fwd.hpp>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>
#include <glm/glm.hpp>

#include <stdexcept>

template<std::uint32_t ImagesInFlight>
class WVkSwapChainResources {

    WVkSwapChainResources();

    void Initialize() {

        InitializeDescriptorLayout();

        InitializePipeline();
        
    }

    void Destroy() {
        
    }

private:

    void InitializeDescriptorLayout() {
        VkDescriptorSetLayoutBinding sampler_binding;
        sampler_binding.binding = 0;
        sampler_binding.descriptorCount = 1;
        sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_binding.pImmutableSamplers = nullptr;
        sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo layout_info;
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 1;
        layout_info.pBindings=&sampler_binding;

        if (vkCreateDescriptorSetLayout(
                vk_device_,
                &layout_info,
                nullptr,
                &descriptor_layout_
                )) {
            throw std::runtime_error("Failed to create Descriptor Set Layout!");
        }

    }

    void InitializePipeline() {
        // shader modules
        std::vector<char> shadercode = WShaderUtils::ReadShader(shader_path);

        VkShaderModule shader_module = WVulkanUtils::CreateShaderModule(
            vk_device_,
            shadercode.data(),
            shadercode.size()
            );

        //
        std::array<VkPipelineShaderStageCreateInfo,2> shader_stages;
        // VkPipelineShaderStageCreateInfo vert_shader_stage;
        
        shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[0].pNext = nullptr;
        shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages[0].pNext=VK_NULL_HANDLE;
        shader_stages[0].module=shader_module;
        shader_stages[0].pName="vsMain";

        shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[1].pNext = nullptr;
        shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages[1].pNext = VK_NULL_HANDLE;
        shader_stages[1].module=shader_module;
        shader_stages[1].pName="fsMain";

        std::array<VkVertexInputAttributeDescription, 2> attr_desc;
        attr_desc[0].binding=0;
        attr_desc[0].location=0;
        attr_desc[0].format=VK_FORMAT_R32G32B32_SFLOAT;
        attr_desc[0].offset=0;  // position

        attr_desc[1].binding=0;
        attr_desc[1].location=1;
        attr_desc[1].format = VK_FORMAT_R32G32_SFLOAT;
        attr_desc[1].offset = sizeof(glm::vec3); // uv

        VkVertexInputBindingDescription binding_desc;
        binding_desc.binding=0;
        binding_desc.stride = sizeof(glm::vec3) + sizeof(glm::vec2);
        binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkPipelineVertexInputStateCreateInfo vertex_input_info;
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount=1;
        vertex_input_info.pVertexBindingDescriptions = &binding_desc;
        vertex_input_info.vertexAttributeDescriptionCount=static_cast<std::uint32_t>(
            attr_desc.size()
            );
        vertex_input_info.pVertexAttributeDescriptions=attr_desc.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly;
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewport_state;
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount=1;
        viewport_state.scissorCount=1;

        VkPipelineRasterizationStateCreateInfo Rasterizer{};
        Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        Rasterizer.depthClampEnable = VK_FALSE;
        Rasterizer.rasterizerDiscardEnable = VK_FALSE;
        Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        Rasterizer.lineWidth = 1.0f;
        Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // because of y-flip in the projection matrix
        Rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo Multisampling{};
        Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        Multisampling.sampleShadingEnable = VK_FALSE;
        Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;  // no msaa

        VkPipelineDepthStencilStateCreateInfo DepthStencil{};
        DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        DepthStencil.depthTestEnable = VK_FALSE;
        DepthStencil.depthWriteEnable = VK_FALSE;
        // DepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        // DepthStencil.depthBoundsTestEnable = VK_FALSE;
        // DepthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState ColorBlendAttachment{};
        ColorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        ColorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo ColorBlending{};
        ColorBlending.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        ColorBlending.logicOpEnable = VK_FALSE;
        ColorBlending.logicOp = VK_LOGIC_OP_COPY;
        ColorBlending.attachmentCount = 1;
        ColorBlending.pAttachments = &ColorBlendAttachment;
        ColorBlending.blendConstants[0] = 0.f;
        ColorBlending.blendConstants[1] = 0.f;
        ColorBlending.blendConstants[2] = 0.f;
        ColorBlending.blendConstants[3] = 0.f;

        std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state.pDynamicStates = dynamic_states.data();

        // std::vector<VkDescriptorSetLayout> slayouts;
        // slayouts.reserve(in_descriptor_set_layout_infos.size());
        // for(auto & v : in_descriptor_set_layout_infos) {
        //     slayouts.push_back(v.descriptor_set_layout);
        // }

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 1; // slayouts.size();
        pipeline_layout_info.pSetLayouts = &descriptor_layout_; // slayouts.data();

        if (vkCreatePipelineLayout(
                vk_device_,
                &pipeline_layout_info,
                nullptr,
                &pipeline_layout_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
        pipeline_info.pStages = shader_stages.data();
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &Rasterizer;
        pipeline_info.pMultisampleState = &Multisampling;
        pipeline_info.pDepthStencilState = &DepthStencil;
        pipeline_info.pColorBlendState = &ColorBlending;
        pipeline_info.pDynamicState = &dynamic_state;
        pipeline_info.layout = pipeline_layout_;

        pipeline_info.renderPass = VK_NULL_HANDLE; // in_render_pass_info.render_pass;
        pipeline_info.subpass = 0;                 // out_pipeline_info.subpass;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

        // Dynamic rendering info

        VkFormat color_format = VK_FORMAT_R8G8B8A8_UNORM;
        VkFormat depth_format = VK_FORMAT_D32_SFLOAT;

        VkPipelineRenderingCreateInfo rendering_info{};
        rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachmentFormats = &color_format;
        rendering_info.depthAttachmentFormat = depth_format;
        rendering_info.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        pipeline_info.pNext = &rendering_info;

            if (vkCreateGraphicsPipelines(
            vk_device_,
            VK_NULL_HANDLE,
            1,
            &pipeline_info,
            nullptr,
            &pipeline_) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create graphics pipeline!");
            }

            vkDestroyShaderModule(vk_device_, shader_module, nullptr);
    }

    VkPipeline pipeline_{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};

    VkDescriptorSetLayout descriptor_layout_{VK_NULL_HANDLE};

    std::array<VkDescriptorPool, ImagesInFlight> descriptor_pool_;

    const char * shader_path{"/Shaders/WRender_DrawInSwapChain.graphic.spv"};

    VkDevice vk_device_;

};
