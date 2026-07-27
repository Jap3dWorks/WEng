#pragma once

#include "WCoreTypes/WGeometry.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WVulkan/RAII/WVkAttachmentsPostprocessRAII.hpp"
#include "WVulkan/RAII/WVkAttachmentsTonemappingRAII.hpp"
#include "WVulkan/RAII/WVkAttachmentsGBuffersRAII.hpp"
#include "WVulkan/RAII/WVkGlobalDescriptorsRAII.hpp"
#include "WVulkan/RAII/WVkLightingPipelineRAII.hpp"
#include "WVulkan/RAII/WVkPostprocessPipelinesRAII.hpp"
#include "WVulkan/RAII/WVkTonemappingPipelineRAII.hpp"
#include "WVulkan/RAII/WVkGBufferPipelinesRAII.hpp"
#include "WVulkan/RAII/WVkSwapchainRAII.hpp"
#include "WVulkan/RAII/WVkSwapchainPipelineRAII.hpp"
#include "WVulkan/RAII/ShadowMapAttachments.hpp"
#include "WVulkan/RAII/ShadowMapPipeline.hpp"

#include "WVkRender/RenderUtils.hpp"
#include "WVkRender/RenderCommands.hpp"

#include <vulkan/vulkan_core.h>
#include <cstdint>

namespace wvk::render::rec_cmd_bffr {

    template<std::uint8_t FramesInFlight>
    inline void GBuffers(
        VkDevice device,
        VkCommandBuffer command_buffer,
        std::uint32_t frame_index,
        WVkAttachmentsGBuffersRAII<FramesInFlight> & attachments,
        WVkGBufferPipelinesRAII<FramesInFlight> & pipelines,
        wvk::raii::AssetRenderData const & asset_render_data,
        WVkGlobalDescriptorsRAII<FramesInFlight> const & global_descriptors
        ) {
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
        
            pipelines.ResetDescriptorPool(pipeline_id, frame_index);

            const WVkRenderPipeline & render_pipeline =
                pipelines.Pipeline(pipeline_id);

            vkCmdBindPipeline(command_buffer,
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              render_pipeline.pipeline);

            wvk::render::RndCmd_SetViewportAndScissor(
                command_buffer,
                attachments.Extent()
                );

            for (auto & bid : pipelines.IterBindings(pipeline_id)) {

                auto& binding = pipelines.GetBinding(bid);

                // Create descriptor
                VkDescriptorSet descriptorset =
                    wvk::render::CreateDescriptorSet(
                        device,
                        pipelines.DescriptorPool(pipeline_id, frame_index),
                        pipelines.DescriptorSetLayout(pipeline_id).descset_layout,
                        frame_index,
                        binding.ubos,
                        binding.textures
                        );

                auto& mesh_info =
                    asset_render_data.StaticMeshInfo(
                        binding.mesh_asset_id
                        );

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

                std::array<VkDescriptorSet, 2> descsets =
                    {
                        global_descriptors.DescriptorSet(frame_index),
                        descriptorset
                    };

                vkCmdBindDescriptorSets(command_buffer,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        render_pipeline.pipeline_layout,
                                        0,
                                        static_cast<std::uint32_t>(descsets.size()),
                                        descsets.data(),
                                        0,
                                        nullptr);

                vkCmdDrawIndexed(command_buffer,
                                 mesh_info.index_count,
                                 1,
                                 0,
                                 0,
                                 0);
            }
        }
    
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
    }

    template<std::uint8_t FramesInFlight>
    inline void ShadowMap(
        VkDevice device,
        VkCommandBuffer command_buffer,
        std::uint32_t frame_index,
        wvk::raii::ShadowMapAttachments<FramesInFlight> & attachments,
        wvk::raii::ShadowMapPipeline<FramesInFlight> & pipeline,
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
        WVkPostprocessPipelinesRAII & pipelines,
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

        // TODO Descriptors are being recreated each frame.
        //  is it required? can I preserve the descriptors between frames?
        pipelines.ResetDescriptorPools(frame_index);

        // Render each postprocess shader
        std::uint32_t idx=0;
        for(auto pbindingid : pipelines.BindingOrderIterator()) {

            auto ppcess_binding = pipelines.GetBinding(pbindingid);

            WVkRenderPipeline ppcess_pipeline =
                pipelines.Pipeline(ppcess_binding.pipeline_id);
            WVkDescriptorSetLayoutInfo ppcess_dsetlay =
                pipelines.DescriptorSetLayout(ppcess_binding.pipeline_id);
            VkDescriptorPool ppcess_dpool =
                pipelines.DescriptorPool(ppcess_binding.pipeline_id, frame_index);

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
                ppcess_pipeline.pipeline
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

            // TODO is it required to be recreated each frame?
            VkDescriptorSet pp_descriptor = wvk::render::CreateDescriptorSet(
                device,
                ppcess_dpool,
                ppcess_dsetlay.descset_layout,
                frame_index,
                ppcess_binding.ubos,
                ppcess_binding.textures
                );

            // const WVkMesh & render_plane = render_plane_.RenderPlane();

            wvk::render::RndCmd_PostprocessDrawCommands(
                device, command_buffer,
                render_plane.vertex_buffer, render_plane.index_buffer,
                render_plane.index_count,
                ppcess_pipeline.pipeline_layout,
                ppcess_pipeline.pipeline,
                std::array<VkDescriptorSet,3>{
                    global_descriptors.DescriptorSet(frame_index),
                    pp_descriptor,
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
