#pragma once

#include "WVulkan/Vk/WVkShader.hpp"
#include "WVulkan/Vk/WVkPipeline.hpp"
#include "WVulkan/RAII/UBOManager/DynamicUBOManager.hpp"
#include "WVulkan/RAII/DescriptorPool.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WVulkan/RAII/Pipelines/DescriptorBindings.hpp"
#include "WAssets/RenderPipeline.hpp"
#include "WAssets/RenderPipelineParams.hpp"

#include <vulkan/vulkan_core.h>
#include <unordered_set>

namespace wvk::raii::pipelines::gbuffer_lib {

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

    template<std::uint8_t FramesInFlight>
    inline auto CreateModelUboDescriptorSet(
        VkDevice device,
        wcr::wid::WEngId model_ubo_id,
        std::uint32_t model_ubo_binding,
        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> & ubo_man,
        VkDescriptorPool descriptor_pool,
        VkDescriptorSetLayout model_ubo_layout
        ) {

        ubo_man.Add<FramesInFlight>(
            sizeof(wct::render::ModelUBO),
            model_ubo_id
            );

        std::array<VkDescriptorSet, FramesInFlight> result;

        for(std::uint32_t f=0; f<FramesInFlight; ++f) {
                    
            WVkUBO ubo = ubo_man
                .GetUBO<FramesInFlight>(
                    sizeof(wct::render::ModelUBO),
                    f
                    );

            VkDescriptorSet descriptor_set;
            VkDescriptorSetAllocateInfo alloc_info =
                wvk::types::VkDescriptorSetAllocateInfo();

            alloc_info.descriptorPool = descriptor_pool;
            alloc_info.descriptorSetCount = 1;
            alloc_info.pSetLayouts = &model_ubo_layout;

            wvk::vulkan::ExecVkProcChecked(
                vkAllocateDescriptorSets,
                "Failed to allocate descriptor sets!",
                device,
                &alloc_info,
                &descriptor_set
                );

            VkWriteDescriptorSet write_ds;

            VkDescriptorBufferInfo buffer_info = {
                .buffer=ubo.buffer,
                .offset=0,
                .range=sizeof(wct::render::ModelUBO)
            };

            VkWriteDescriptorSet ubo_write = wvk::types::VkWriteDescriptorSet();
            ubo_write.dstBinding = model_ubo_binding;
            ubo_write.dstSet = descriptor_set;
            ubo_write.dstArrayElement = 0;
            ubo_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            ubo_write.descriptorCount = 1;
            ubo_write.pBufferInfo = &buffer_info;
            
            vkUpdateDescriptorSets(
                device,
                1,
                &ubo_write,
                0,
                nullptr
                );

            result[f]=descriptor_set;
        }

        return result;
    }


    template<std::uint8_t FramesInFlight>
    struct Binding {
        wcr::wid::WTypeAssetIndexId mesh_id;

        std::uint32_t model_ubo_offset;
        std::array<VkDescriptorSet, FramesInFlight> model_ubo_descriptor_set;

        std::vector<std::uint32_t> param_ubo_offsets;
        
        std::array<VkDescriptorSet, FramesInFlight> param_descripor_set;
    };

    template<std::uint8_t FramesInFlight>
    struct DescriptorCollection {
        
        wvk::raii::DescriptorPool<
            8 * FramesInFlight,
            0,
            30 * FramesInFlight,
            38> descriptor_pool{};

        std::array<VkDescriptorSet, FramesInFlight> model_ubo_desc_set{VK_NULL_HANDLE};

        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> ubo_manager{};

        std::unordered_map<wcr::wid::WAssetId,
                           std::array<VkDescriptorSet, FramesInFlight>> param_descriptors;

        std::unordered_map<
            wcr::wid::WAssetId::IdType,
            TSparseSet<Binding<FramesInFlight>>> pipeline_bindings{};

        void CreateBindingSet(
            wcr::wid::WEngId binding_set_id,
            wcr::wid::WEngId model_ubo_id,
            VkDescriptorSetLayout model_ubo_layout,
            std::uint8_t model_ubo_binding,
            was::RenderPipeline asset_pipeline,
            was::RenderPipelineParams asset_params,
            VkDescriptorSetLayout param_ds_layout,
            wvk::raii::AssetRenderData & asset_data,
            wcr::wid::WTypeAssetIndexId in_mesh_asset_id
            ) {
            
            EnsureModelUboDescriptorSet(
                model_ubo_id,
                model_ubo_layout,
                model_ubo_binding
                );

            auto param_dynamic_offsets = EnsureParamDescriptorSet(
                binding_set_id,
                asset_pipeline,
                asset_params,
                asset_data,
                param_ds_layout
                );

            auto model_ubo_offset = ubo_manager
                .GetOffset<FramesInFlight>(
                    sizeof(wct::render::ModelUBO),
                    binding_set_id);

            auto param_descriptor_sets = param_descriptors[asset_params.Get_asset_id()];

            wvk::raii::pipelines::gbuffer_lib::Binding binding{
                .mesh_id=in_mesh_asset_id,
                .model_ubo_offset = model_ubo_offset,
                .model_ubo_descriptor_set = model_ubo_desc_set,
                .param_ubo_offsets=std::move(param_dynamic_offsets),
                .param_descriptor_set = param_descriptor_sets
            };

            pipeline_bindings[
                asset_pipeline.Get_asset_id().GetId()
                ].Insert(binding_set_id.GetId(), std::move(binding));            
        }

        void EnsureModelUboDescriptorSet(
            wcr::wid::WEngId model_ubo_id,
            VkDescriptorSetLayout model_ubo_layout,
            std::uint8_t model_ubo_binding
            ) {
            
            VkDescriptorSet model_ubo_descriptor;

            if(model_ubo_desc_set[0] == VK_NULL_HANDLE) {

                auto descriptors =
                    wvk::raii::pipelines::gbuffer_lib::
                    CreateModelUboDescriptorSet(
                        descriptor_pool.Creator().device,
                        model_ubo_id,
                        model_ubo_binding,
                        ubo_manager,
                        *descriptor_pool,
                        model_ubo_layout
                        );

                model_ubo_desc_set = descriptors;
            }
        }

        std::vector<std::uint32_t> EnsureParamDescriptorSet(
            wcr::wid::WEngId binding_set_id,
            was::RenderPipeline const & asset_pipeline,
            was::RenderPipelineParams const & asset_params,
            wvk::raii::AssetRenderData & asset_data,
            VkDescriptorSetLayout param_layout
            ) {

            auto ubo_params = wvk::raii::pipelines::desc_bindings
                ::CollectUBOBindings<FramesInFlight>(
                    binding_set_id,
                    asset_pipeline,
                    asset_params,
                    ubo_manager
                    );

            if (!param_descriptors.contains(
                    asset_params.Get_asset_id())) {
                
                auto texture_params = wvk::raii::pipelines::desc_bindings
                    ::CollectTextureBindings(
                        asset_params,
                        asset_data
                        );

                auto descriptors =wvk::raii::pipelines::desc_bindings
                    ::CreateParamsDescriptorSet(
                        descriptor_pool.Creator().device,
                        param_layout,
                        descriptor_pool,
                        texture_params,
                        ubo_params
                        );

                param_descriptors[asset_params.Get_asset_id()] =
                    std::move(descriptors);
            }

            std::sort(ubo_params, [](auto a, auto b)
                {
                    return a.binding < b.binding;
                }
                );

            std::vector<std::uint32_t> dynamic_offsets;

            std::ranges::transform(ubo_params,
                                   std::back_inserter(dynamic_offsets),
                                   [](auto b) {return b.dynamic_offset; });

            return dynamic_offsets;
        }       

        
    };



};


