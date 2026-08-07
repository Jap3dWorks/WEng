#pragma once

#include "WCoreTypes/WGeometry.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WVulkan/RAII/Pipelines/GBuffer.hpp"
#include "WVulkan/RAII/WVkAttachmentsPostprocessRAII.hpp"
#include "WVulkan/RAII/WVkAttachmentsTonemappingRAII.hpp"
#include "WVulkan/RAII/WVkAttachmentsGBuffersRAII.hpp"
#include "WVulkan/RAII/WVkGlobalDescriptorsRAII.hpp"
#include "WVulkan/RAII/WVkLightingPipelineRAII.hpp"
// #include "WVulkan/RAII/Pipelines/Postprocess.hpp"
#include "WVulkan/RAII/Pipelines/Postprocess.hpp"
#include "WVulkan/RAII/WVkTonemappingPipelineRAII.hpp"
#include "WVulkan/RAII/Pipelines/GBuffer.hpp"
#include "WVulkan/RAII/WVkSwapchainRAII.hpp"
#include "WVulkan/RAII/WVkSwapchainPipelineRAII.hpp"
#include "WVulkan/RAII/ShadowMapAttachments.hpp"
#include "WVulkan/RAII/ShadowMapPipeline.hpp"

#include "WVkRender/RenderUtils.hpp"
#include "WVkRender/RenderCommands.hpp"
#include "WVulkan/WVulkanStructs.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>
#include <cstdint>

namespace wvk::render::rec_cmd_bffr {

    struct ShadowMapBindingInfo {
        WVkMesh mesh_info;
        // WVkDescUBOInfo model_ubo;
    };

    template<std::uint8_t FramesInFlight>
    inline auto GBuffers(
        VkDevice device,
        VkCommandBuffer command_buffer,
        std::uint32_t frame_index,
        WVkAttachmentsGBuffersRAII<FramesInFlight> & attachments,
        wvk::raii::pipelines::GBuffer<FramesInFlight> & pipelines,
        wvk::raii::AssetRenderData const & asset_render_data,
        WVkGlobalDescriptorsRAII<FramesInFlight> const & global_descriptors
        ) {

        std::vector<std::optional<ShadowMapBindingInfo>> shadow_map_bindings{};
        // shadow_map_bindings.reserve(pipelines.GetBindingsCount());

        // auto collect_shadow_map_binding =
        //     [frame_index, &pipelines]
        //     (
        //         WVkMesh const & mesh,
        //         WVkPipelineBinding<FramesInFlight> const & binding
        //         )
        //     -> std::optional<ShadowMapBindingInfo> {
        //     for(auto & ubo_dt : binding.ubos) {
        //         if(ubo_dt.binding == pipelines.MODEL_UBO_BINDING) {
        //             return ShadowMapBindingInfo{
        //                 .mesh_info=mesh,
        //                 .model_ubo=ubo_dt.ubo_desc[frame_index]
        //             };
        //         } 
        //     }
            
        //     return std::nullopt;
        // };

        wvk::render::RndCmd_TransitionGBufferWriteLayout(
            command_buffer,
            attachments.Albedo(frame_index).Image(),
            attachments.Emission(frame_index).Image(),
            attachments.Normal(frame_index).Image(),
            attachments.ORM(frame_index).Image(),
            attachments.Depth(frame_index).Image(),
            attachments.Extra01(frame_index).Image()
            );

        wvk::render::RndCmd_BeginGBuffersRendering(
            command_buffer,
            attachments.Albedo(frame_index).View(),
            attachments.Emission(frame_index).View(),
            attachments.Normal(frame_index).View(),
            attachments.ORM(frame_index).View(),
            attachments.Depth(frame_index).View(),
            attachments.Extra01(frame_index).View(),
            attachments.Extent()
            );

        for(auto pipeline_id : pipelines.IterPipelines()) {
        
            std::tuple<VkPipeline,VkPipelineLayout> pipeline__layout =
                pipelines.GetPipeline(pipeline_id);

            vkCmdBindPipeline(command_buffer,
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              std::get<0>(pipeline__layout));
                              // render_pipeline.pipeline);

            wvk::render::RndCmd_SetViewportAndScissor(
                command_buffer,
                attachments.Extent()
                );

            for (auto coll_id : pipelines.IterCollections()) {

                for (auto & binding : pipelines.Bindings(coll_id, pipeline_id)) {

                    auto& mesh_info =
                        asset_render_data.StaticMeshInfo(
                            binding.renderable_asset_id.AsAssetIndexId()
                            );

                    // shadow_map_bindings.push_back(
                    //     collect_shadow_map_binding(
                    //         mesh_info, binding
                    //         ));

                    VkBuffer vertex_buffers[] = {mesh_info.vertex_buffer};
                    VkDeviceSize offsets[] = {0};

                    vkCmdBindVertexBuffers(
                        command_buffer,
                        0,
                        1,
                        vertex_buffers,
                        offsets
                        );

                    vkCmdBindIndexBuffer(
                        command_buffer,
                        mesh_info.index_buffer,
                        0,
                        VK_INDEX_TYPE_UINT32
                        );

                    auto ubo_binding = pipelines.GetUboBinding(
                        coll_id, pipeline_id, binding.binding_set_id
                        );

                    std::array descsets =
                        {
                            global_descriptors.DescriptorSet(frame_index),
                            binding.descriptor_set.at(frame_index),
                            ubo_binding.descriptor_set.at(frame_index)
                        };

                    std::vector<std::uint32_t> dynamic_offsets =
                        binding.dynamic_offsets;

                    dynamic_offsets.push_back(ubo_binding.dynamic_offsets[0]);

                    vkCmdBindDescriptorSets(command_buffer,
                                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            std::get<1>(pipeline__layout),
                                            0,
                                            static_cast<std::uint32_t>(descsets.size()),
                                            descsets.data(),
                                            dynamic_offsets.size(),
                                            dynamic_offsets.data());

                    vkCmdDrawIndexed(command_buffer,
                                     mesh_info.index_count,
                                     1,
                                     0,
                                     0,
                                     0);
                }
            }
        }

        // TODO can Shadow map be in parallel?
        vkCmdEndRendering(command_buffer);

        wvk::render::RndCmd_TransitionGBufferReadLayout(
            command_buffer,
            attachments.Albedo(frame_index).Image(),
            attachments.Emission(frame_index).Image(),
            attachments.Normal(frame_index).Image(),
            attachments.ORM(frame_index).Image(),
            attachments.Depth(frame_index).Image(),
            attachments.Extra01(frame_index).Image()
            );

        return shadow_map_bindings;
    }

    template<std::uint8_t FramesInFlight>
    inline void ShadowMap(
        VkDevice device,
        VkCommandBuffer command_buffer,
        std::uint32_t frame_index,
        wvk::raii::ShadowMapAttachments<FramesInFlight> & attachments,
        wvk::raii::ShadowMapPipeline<FramesInFlight> & pipeline,
        std::vector<std::optional<ShadowMapBindingInfo>> const & pipeline_bindings,
        WVkGlobalDescriptorsRAII<FramesInFlight> const & global_descriptors
        ) {

        wvk::render::rcmd::ShadowMap::AttachmentTransitionWriteLayout(
            command_buffer,
            attachments.ShadowMap(frame_index)
            );

        // beginRendering
        wvk::render::rcmd::ShadowMap::BeginRendering(
            command_buffer,
            attachments.ShadowMap(frame_index),
            attachments.Extent()
            );

        // Bind pipeline
        // Render if for each GBuffer opaque geometry.
        // Bind Pipeline
        vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline.Pipeline()
            );

        wvk::render::rcmd::SetViewportAndScissor(
            command_buffer,
            attachments.Extent()
            );

        pipeline.ResetDescriptorPool(frame_index);

        for(auto binding  : pipeline_bindings) {
            if (!binding) continue;

            VkDescriptorSet descriptorset;
            //     =
            //     wvk::render::rcmd::ShadowMap::CreateDescriptorSet(
            //         device,
            //         pipeline.MODEL_UBO_BINDING,
            //         binding->model_ubo.desc_buffer
            //         );

            VkBuffer vertex_buffers[] = {binding->mesh_info.vertex_buffer};
            VkDeviceSize offsets[] = {0};

            vkCmdBindVertexBuffers(
                command_buffer,
                0,
                1,
                vertex_buffers,
                offsets
                );

            vkCmdBindIndexBuffer(
                command_buffer,
                binding->mesh_info.index_buffer,
                0,
                VK_INDEX_TYPE_UINT32
                );

            std::array<VkDescriptorSet, 2> descsets =
                {
                    global_descriptors.DescriptorSet(frame_index),  // key light info
                    descriptorset
                };

            vkCmdBindDescriptorSets(command_buffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipeline.GetPipelineLayout(),
                                    0,
                                    static_cast<std::uint32_t>(descsets.size()),
                                    descsets.data(),
                                    0,
                                    nullptr);

            vkCmdDrawIndexed(command_buffer,
                             binding->mesh_info.index_count,
                             1,
                             0,
                             0,
                             0);
        }

        vkCmdEndRendering(command_buffer);

        wvk::render::rcmd::ShadowMap::AttachmentTransitionReadLayout(
            command_buffer,
            attachments.Depth(frame_index).Image()
            );

    }

    template<std::uint8_t FramesInFlight>
    inline void Lighting(
        VkDevice device,
        VkCommandBuffer in_command_buffer,
        std::uint32_t in_frame_index,
        WVkAttachmentsLightingRAII<FramesInFlight> & attachments,
        WVkLightingPipelineRAII<FramesInFlight> & pipelines,
        WVkAttachmentsGBuffersRAII<FramesInFlight> const & gbuffer_attachments,
        WVkGlobalDescriptorsRAII<FramesInFlight> const & global_descriptors,
        WVkMesh const & render_plane,
        VkSampler plane_sampler
        ) {
        wvk::render::RndCmd_TransitionLightingWriteLayout(
            in_command_buffer,
            attachments.Color(in_frame_index).Image()
            );

        wvk::render::RndCmd_BeginLightingRendering(
            in_command_buffer,
            attachments.Color(in_frame_index).View(),
            attachments.Extent()
            );

        pipelines.ResetDescriptorPool(in_frame_index);

        // Bind Pipeline
        vkCmdBindPipeline(
            in_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelines.Pipeline()
            );

        wvk::render::RndCmd_SetViewportAndScissor(
            in_command_buffer,
            attachments.Extent()
            );

        // DescriptorSet
        // TODO do not recreate each frame, create descriptorsSets only once.
        VkDescriptorSet descriptorset = wvk::render::CreateLightingRenderDescriptor(
            device,
            pipelines.DescriptorPool(in_frame_index),
            pipelines.DescriptorSetLayout(),
            plane_sampler,
            // render_plane_.Sampler(),
            gbuffer_attachments.Albedo(in_frame_index).View(),
            gbuffer_attachments.Emission(in_frame_index).View(),
            gbuffer_attachments.Normal(in_frame_index).View(),
            gbuffer_attachments.ORM(in_frame_index).View(),
            gbuffer_attachments.Depth(in_frame_index).View(),
            gbuffer_attachments.Extra01(in_frame_index).View()
            );

        // Draw Commands
        // const WVkMesh & rplane = render_plane_.RenderPlane();
    
        VkBuffer vertex_buffers[] = {render_plane.vertex_buffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(
            in_command_buffer,
            0,
            1,
            vertex_buffers,
            offsets
            );

        vkCmdBindIndexBuffer(
            in_command_buffer,
            render_plane.index_buffer,
            0,
            VK_INDEX_TYPE_UINT32
            );

        std::array<VkDescriptorSet,2> descsets = {
            global_descriptors.DescriptorSet(in_frame_index),
            descriptorset
        };

        vkCmdBindDescriptorSets(in_command_buffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelines.PipelineLayout(),
                                0,
                                static_cast<std::uint32_t>(descsets.size()),
                                descsets.data(),
                                0,
                                nullptr);

        vkCmdDrawIndexed(in_command_buffer,
                         render_plane.index_count,
                         1,
                         0,
                         0,
                         0);

        vkCmdEndRendering(in_command_buffer);
    
        wvk::render::RndCmd_TransitionLightingReadLayout(
            in_command_buffer,
            attachments.Color(in_frame_index).Image()
            );
    }

    
    template<std::uint8_t FramesInFlight>
    inline VkImageView Postprocess(
        VkDevice device,
        VkCommandBuffer command_buffer,
        std::uint32_t frame_index,
        WVkAttachmentsPostprocessRAII<FramesInFlight> & attachments,
        wvk::raii::pipelines::Postprocess<FramesInFlight> & pipelines,
        WVkAttachmentsLightingRAII<FramesInFlight> const & lighting_attachments,
        WVkAttachmentsGBuffersRAII<FramesInFlight> const & gbuffer_attachments,
        WVkPostprocessGlobalDescriptorRAII<FramesInFlight> & ppcss_global_descriptors,
        WVkGlobalDescriptorsRAII<FramesInFlight> & global_descriptors,
        WVkMesh const & render_plane,
        VkSampler plane_sampler
        ){
        VkImageView input_view = lighting_attachments.Color(frame_index).View();
        VkImage input_img = lighting_attachments.Color(frame_index).Image();
    
        VkImageView dst_view = attachments.Color(frame_index).View();
        VkImage dst_img = attachments.Color(frame_index).Image();
    
        std::array<VkImageView, 2> pp_views = {input_view, dst_view};
        std::array<VkImage, 2> pp_images = {input_img, dst_img};

        // Render each postprocess shader
        std::uint32_t idx=0;
        for(auto pbindingid : pipelines.BindingsOrder()) {

            auto ppcess_binding = pipelines.GetBinding(pbindingid);

            std::tuple<VkPipeline, VkPipelineLayout> pipeline__layout =
                pipelines.GetPipeline(ppcess_binding.pipeline_id);

            // render into layout
            wvk::render::RndCmd_TransitionRenderImageLayout(
                command_buffer,
                dst_img,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                {},
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                );

            wvk::render::RndCmd_BeginPostprocessRendering(
                command_buffer,
                dst_view,
                attachments.Extent()
                );

            vkCmdBindPipeline(
                command_buffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                std::get<0>(pipeline__layout)
                );

            wvk::render::RndCmd_SetViewportAndScissor(
                command_buffer,
                attachments.Extent()
                );

            ppcss_global_descriptors.UpdateDescriptorBinding(
                ppcss_global_descriptors.PREV_BINDING,
                frame_index,
                {
                    .sampler=plane_sampler,
                    .imageView=input_view,
                    .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                }
                );

            wvk::render::RndCmd_PostprocessDrawCommands(
                device,
                command_buffer,
                render_plane.vertex_buffer,
                render_plane.index_buffer,
                render_plane.index_count,
                std::get<1>(pipeline__layout),
                std::get<0>(pipeline__layout),
                std::array<VkDescriptorSet,3> {
                    global_descriptors.DescriptorSet(frame_index),
                    ppcess_binding.descriptor_set[frame_index],
                    ppcss_global_descriptors.DescriptorSet(frame_index)
                }
                );

            vkCmdEndRendering(command_buffer);

            idx++;

            input_view = pp_views[idx % 2];
            input_img = pp_images[idx % 2];
            dst_view = pp_views[(idx + 1) % 2];
            dst_img = pp_images[(idx + 1) % 2];

            // render from layout
            wvk::render::RndCmd_TransitionRenderImageLayout(
                command_buffer,
                input_img,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_ACCESS_SHADER_READ_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
                );
        }

        return input_view;
    }

    template<std::uint8_t FramesInFlight>
    inline VkImageView Tonemapping (
        VkDevice device,
        VkCommandBuffer in_command_buffer,
        std::uint32_t in_frame_index,
        WVkAttachmentsTonemappingRAII<FramesInFlight> & attachments,
        WVkTonemappingPipelineRAII<FramesInFlight> & pipelines,
        VkImageView input_image_view,
        WVkMesh const & render_plane,
        VkSampler plane_sampler
        ) {
        wvk::render::RndCmd_TransitionTonemappingWriteLayout(
            in_command_buffer,
            attachments.Color(in_frame_index).Image()
            );

        wvk::render::RndCmd_BeginTonemappingRendering(
            in_command_buffer,
            attachments.Color(in_frame_index).View(),
            attachments.Extent()
            );

        pipelines.ResetDescriptorPool(in_frame_index);

        VkPipeline pipeline = pipelines.Pipeline();

        vkCmdBindPipeline(
            in_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline
            );

        wvk::render::RndCmd_SetViewportAndScissor(
            in_command_buffer,
            attachments.Extent()
            );

        VkDescriptorSet descriptorset =
            wvk::render::CreateTonemappingDescriptor(
                device,
                pipelines.DescriptorPool(in_frame_index),
                pipelines.DescriptorSetLayout(),
                plane_sampler,
                input_image_view
                );

        VkDeviceSize offsets = 0;

        wvk::render::TonemappingBindings(
            in_command_buffer,
            render_plane.vertex_buffer,
            render_plane.index_buffer,
            offsets,
            descriptorset,
            pipelines.PipelineLayout()
            );

        vkCmdDrawIndexed(in_command_buffer,
                         render_plane.index_count,
                         1,
                         0,
                         0,
                         0);

        vkCmdEndRendering(in_command_buffer);

        wvk::render::RndCmd_TransitionTonemappingReadLayout(
            in_command_buffer,
            attachments.Color(in_frame_index).Image()
            );

        return attachments.Color(in_frame_index).View();
    }

    template<std::uint8_t FramesInFlight>
    inline void SwapChain(
        VkDevice device,
        VkCommandBuffer in_command_buffer,
        std::uint32_t in_frame_index,
        std::uint32_t in_image_index,
        WVkSwapchainRAII const & swap_chain,
        WVkSwapchainPipelineRAII<FramesInFlight> & pipeline,
        VkImageView input_img_view,
        WVkMesh const & render_plane,
        VkSampler plane_sampler
        ) {
        
        VkImage swapchain_image = swap_chain.Images()[in_image_index];
        VkImageView swapchain_imageview = swap_chain.Views()[in_image_index];

        // swap chain image layout to render into it
        wvk::render::RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            swapchain_image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        wvk::render::RndCmd_BeginSwapchainRendering(
            in_command_buffer,
            swapchain_imageview,
            swapchain_imageview,
            swap_chain.Extent()
            );

        pipeline.ResetDescriptorPool(in_frame_index);

        VkDescriptorSetLayout dslay = pipeline.DescriptorSetLayout();

        vkCmdBindPipeline(
            in_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline.Pipeline()
            );

        wvk::render::RndCmd_SetViewportAndScissor(
            in_command_buffer,
            swap_chain.Extent()
            );

        VkDescriptorSet descriptor = wvk::render::CreateInputRenderDescriptor(
            device,
            pipeline.DescriptorPool(in_frame_index),
            dslay,
            input_img_view,
            plane_sampler
            );

        VkDeviceSize offsets=0;

        vkCmdBindVertexBuffers(in_command_buffer,
                               0,
                               1,
                               &render_plane.vertex_buffer,
                               &offsets);

        vkCmdBindIndexBuffer(in_command_buffer,
                             render_plane.index_buffer,
                             0,
                             VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(in_command_buffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline.PipelineLayout(),
                                0,
                                1,
                                &descriptor,
                                0,
                                nullptr);

        vkCmdDrawIndexed(in_command_buffer,
                         render_plane.index_count,
                         1,
                         0,0,0);

        vkCmdEndRendering(in_command_buffer);

        // Prepare swapchain images for present
        wvk::render::RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            swapchain_image,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            {},
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
            );
    }
}
