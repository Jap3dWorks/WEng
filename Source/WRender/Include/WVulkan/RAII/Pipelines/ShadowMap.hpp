#pragma once

#include "WVulkan/Vk/WVkTypes.hpp"
#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/RAII/DescriptorPool.hpp"
#include "WVulkan/RAII/DescriptorSetLayout.hpp"
#include "WVulkan/RAII/Pipeline.hpp"
#include "WVulkan/RAII/PipelineLayout.hpp"
#include "WRender/WShader.hpp"
#include "WVulkan/Vk/WVkShader.hpp"
#include "WCoreTypes/WGeometry.hpp"

#include <string_view>
#include <vulkan/vulkan_core.h>
#include <span>

namespace wvk::raii::pipelines {
    
    template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
    class ShadowMap {

    public:

        static inline constexpr std::array const VERTEX_INPUT_ATTRIBUTE_DESCRIPTION {
            VkVertexInputAttributeDescription{
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(wct::geometry::WVertex, position)
            },
                VkVertexInputAttributeDescription{
                .location = 1,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32_SFLOAT,
                    .offset = offsetof(wct::geometry::WVertex, tex_coords),
                    }
        };

        static inline constexpr std::array const VERTEX_INPUT_BINDING_DESCRIPTION {
            VkVertexInputBindingDescription{
                .binding=0,
                .stride=sizeof(wct::geometry::WVertex),
                .inputRate=VK_VERTEX_INPUT_RATE_VERTEX
            }  
        };

    public:

        static inline constexpr std::uint8_t MODEL_UBO_BINDING{0};

        static inline constexpr std::string_view SHADER_PATH
            {"/Content/Shaders/WRender_shadowmap.shdw.spv"};

    public:

        ShadowMap() = default;
        ShadowMap(ShadowMap const &) = delete;
        ShadowMap(ShadowMap&&) = default;
        ShadowMap& operator=(ShadowMap const &) = delete;
        ShadowMap& operator=(ShadowMap&&) = default;
        ~ShadowMap() = default;

        ShadowMap(
            VkDevice device,
            std::string_view shader_path,
            VkDescriptorSetLayout global_layout
            ) :
            // descriptor_pool_({device}),
            descset_lay_(
                {device},
                std::array{
                    VkDescriptorSetLayoutBinding{
                        .binding=0,
                        .descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .descriptorCount=1,
                        .stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT,
                        .pImmutableSamplers=VK_NULL_HANDLE
                    }
                }),
            pipeline_layout_(
                {device},
                std::array{
                    global_layout,
                    *descset_lay_
                }
                )
            {
                InitializePipeline(device);
            }


        WNODISCARD
        VkPipelineLayout GetPipelineLayout() const {
            return pipeline_layout_.Value();
        }

        WNODISCARD
        VkPipeline GetPipeline() const {
            return *pipeline_;
        }

    private:

        void InitializePipeline(VkDevice device) {

            auto shadercode = wrd::shader::ReadShader(
                wstr::SystemPath(SHADER_PATH)
                );

            std::array shader_stages_info {
                WVkShaderStageInfo{
                    .code{std::span<std::uint8_t>{shadercode}},
                    .type=wct::render::EShaderStageFlag::Vertex,
                    .entry_point{"vsMain"}
                },
                WVkShaderStageInfo{
                    .code{std::span<std::uint8_t>{shadercode}},
                    .type=wct::render::EShaderStageFlag::Fragment,
                    .entry_point{"fsMain"}
                }
            };

            auto[shader_stages, shader_modules] =
                wvk::shader::CreateShaderModules<shader_stages_info.size()>(
                    device,
                    shader_stages_info
                    );

            VkPipelineVertexInputStateCreateInfo vertex_input_info =
                wvk::types::VkPipelineVertexInputStateCreateInfo();

            vertex_input_info.vertexBindingDescriptionCount =
                static_cast<uint32_t>(
                    VERTEX_INPUT_BINDING_DESCRIPTION.size()
                    );
            vertex_input_info.vertexAttributeDescriptionCount =
                static_cast<uint32_t>(
                    VERTEX_INPUT_ATTRIBUTE_DESCRIPTION.size()
                    );
            vertex_input_info.pVertexBindingDescriptions =
                VERTEX_INPUT_BINDING_DESCRIPTION.data();
            vertex_input_info.pVertexAttributeDescriptions =
                VERTEX_INPUT_ATTRIBUTE_DESCRIPTION.data();

            VkPipelineInputAssemblyStateCreateInfo input_assembly = 
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
             // because of y-flip in the projection matrix
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;

            VkPipelineMultisampleStateCreateInfo multisampling =
                wvk::types::VkPipelineMultisampleStateCreateInfo();
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

            VkPipelineDepthStencilStateCreateInfo depth_stencil =
                wvk::types::VkPipelineDepthStencilStateCreateInfo();
            depth_stencil.depthTestEnable = VK_TRUE;
            depth_stencil.depthWriteEnable = VK_TRUE;
            depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
            depth_stencil.depthBoundsTestEnable = VK_FALSE;
            depth_stencil.stencilTestEnable = VK_FALSE;

            std::array<VkDynamicState,2> dynamic_states;
            dynamic_states = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };

            VkPipelineDynamicStateCreateInfo dynamic_state_create_info =
                wvk::types::VkPipelineDynamicStateCreateInfo();
            dynamic_state_create_info.dynamicStateCount =
                static_cast<uint32_t>(dynamic_states.size());
            dynamic_state_create_info.pDynamicStates = dynamic_states.data();

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
            pipeline_create_info.pColorBlendState = VK_NULL_HANDLE;
            pipeline_create_info.pDynamicState = &dynamic_state_create_info;
    
            pipeline_create_info.renderPass = VK_NULL_HANDLE;
            pipeline_create_info.subpass = 0;
            pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

            // Dynamic Rendering

            VkPipelineRenderingCreateInfo rendering_info =
                wvk::types::VkPipelineRenderingCreateInfo();
            rendering_info.colorAttachmentCount = 0;
            rendering_info.pColorAttachmentFormats = VK_NULL_HANDLE; 
            rendering_info.depthAttachmentFormat = WVK_GBUFFER_RENDER_DEPTH_FORMAT;
            rendering_info.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
            pipeline_create_info.layout = *pipeline_layout_;

            pipeline_create_info.pNext = &rendering_info;

            VkPipeline vk_pipeline;
        
            wvk::vulkan::ExecVkProcChecked(
                vkCreateGraphicsPipelines,
                "Failed to create graphics pipeline!",
                device,
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info,
                nullptr,
                &vk_pipeline
                );

            pipeline_ = wvk::raii::PipelineWrapper(
                {device}, vk_pipeline
                );

            for (auto& shader_module : shader_modules)
            {
                vkDestroyShaderModule(
                    device, 
                    shader_module,
                    nullptr
                    );
            }
        }

    private:

        wvk::raii::DescriptorSetLayout<1> descset_lay_{};

        wvk::raii::PipelineLayout<1>  pipeline_layout_{};
        wvk::raii::PipelineWrapper pipeline_{};


    };
}
