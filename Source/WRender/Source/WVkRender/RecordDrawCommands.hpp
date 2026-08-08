#pragma once

#include "WCoreTypes/WGeometry.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WVulkan/RAII/Pipelines/GBuffer.hpp"
#include "WVulkan/RAII/WVkAttachmentsPostprocessRAII.hpp"
#include "WVulkan/RAII/WVkAttachmentsTonemappingRAII.hpp"
#include "WVulkan/RAII/WVkAttachmentsGBuffersRAII.hpp"
#include "WVulkan/RAII/WVkGlobalDescriptorsRAII.hpp"
#include "WVulkan/RAII/Pipelines/Lighting.hpp"
#include "WVulkan/RAII/Pipelines/Postprocess.hpp"
#include "WVulkan/RAII/WVkTonemappingPipelineRAII.hpp"
#include "WVulkan/RAII/Pipelines/GBuffer.hpp"
#include "WVulkan/RAII/WVkSwapchainRAII.hpp"
#include "WVulkan/RAII/WVkSwapchainPipelineRAII.hpp"
#include "WVulkan/RAII/Attachments/ShadowMap.hpp"
#include "WVulkan/RAII/Pipelines/ShadowMap.hpp"

#include "WVkRender/RenderUtils.hpp"
#include "WVkRender/RenderCommands.hpp"
#include "WVulkan/WVulkanStructs.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>
#include <cstdint>

namespace wvk::render::rec_cmd_bffr {

    using ShadowMapBindingInfo =
        std::unordered_map<
            wcr::wid::WEngId,                                        // pipeline_id
            std::unordered_map<
                std::size_t,                                         // collection_id
                std::tuple <
                    VkDescriptorSet,                                 // descriporSet
                    std::vector<std::tuple<WVkMesh, std::uint32_t>>  // mesh offset
                    >>>;

    template<std::uint8_t FramesInFlight>
    inline auto GBuffers (
        VkDevice device,
        VkCommandBuffer command_buffer,
        std::uint32_t frame_index,
        WVkAttachmentsGBuffersRAII<FramesInFlight> & attachments,
        wvk::raii::pipelines::GBuffer<FramesInFlight> & pipelines,
        wvk::raii::AssetRenderData const & asset_render_data,
        WVkGlobalDescriptorsRAII<FramesInFlight> const & global_descriptors
        ) {

        ShadowMapBindingInfo shadow_map_binding_info{};

        auto collect_shadow_map_descriptor =
            [&shadow_map_binding_info] (
                wcr::wid::WEngId pipeline_id,
                std::size_t collection_id,
                VkDescriptorSet descriptor
                ) {
                if (!shadow_map_binding_info.contains(pipeline_id)) {
                    shadow_map_binding_info[pipeline_id]= {};
                    if (!shadow_map_binding_info[pipeline_id].contains(collection_id)) {
                        shadow_map_binding_info[pipeline_id] [collection_id]={};
                    }
                }
                shadow_map_binding_info[pipeline_id]
                    [collection_id] = std::tuple{
                    descriptor,
                    std::vector<std::tuple<WVkMesh, std::uint32_t>>{}};
            };

        auto collect_shadow_map_binding =
            [&shadow_map_binding_info] (
                wcr::wid::WEngId pipeline_id,
                std::size_t collection_id,
                WVkMesh const & mesh,
                std::uint32_t offset
                ) {
                std::get<1>(shadow_map_binding_info[pipeline_id][collection_id]).push_back(
                    std::tuple{mesh, offset}
                    );

                return std::nullopt;
            };

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

            wvk::render::RndCmd_SetViewportAndScissor(
                command_buffer,
                attachments.Extent()
                );

            for (auto coll_id : pipelines.IterCollections()) {

                collect_shadow_map_descriptor(
                    pipeline_id,
                    coll_id,
                    pipelines.GetModelUboDescriptors(coll_id)[frame_index]
                    );

                for (auto & binding : pipelines.Bindings(coll_id, pipeline_id)) {

                    auto& mesh_info =
                        asset_render_data.StaticMeshInfo(
                            binding.renderable_asset_id.AsAssetIndexId()
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

                    auto model_ubo_binding = pipelines.GetModelUboBinding(
                        coll_id, pipeline_id, binding.binding_set_id
                        );

                    collect_shadow_map_binding(
                        pipeline_id,
                        coll_id,
                        mesh_info,
                        model_ubo_binding.dynamic_offsets[0]
                        );

                    std::array descsets =
                        {
                            global_descriptors.DescriptorSet(frame_index),
                            binding.descriptor_set.at(frame_index),
                            model_ubo_binding.descriptor_set.at(frame_index)
                        };

                    std::vector<std::uint32_t> dynamic_offsets =
                        binding.dynamic_offsets;

                    dynamic_offsets.push_back(model_ubo_binding.dynamic_offsets[0]);

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

        return shadow_map_binding_info;
    }

    template<std::uint8_t FramesInFlight>
    inline void ShadowMap (
        VkDevice device,
        VkCommandBuffer command_buffer,
        std::uint32_t frame_index,
        wvk::raii::attachments::ShadowMap<FramesInFlight> & shadowmap_attachments,
        wvk::raii::pipelines::ShadowMap<FramesInFlight> & shadowmap_pipeline,
        ShadowMapBindingInfo const & pipeline_bindings,
        WVkGlobalDescriptorsRAII<FramesInFlight> const & global_descriptors
        ) {

        wvk::render::rcmd::ShadowMap::AttachmentTransitionWriteLayout(
            command_buffer,
            shadowmap_attachments.GetDepth(frame_index).Image()
            );

        // beginRendering
        wvk::render::rcmd::ShadowMap::BeginRendering(
            command_buffer,
            shadowmap_attachments.GetDepth(frame_index).View(),
            shadowmap_attachments.GetExtent()
            );

        // Bind pipeline
        // Render if for each GBuffer opaque geometry.
        // Bind Pipeline
        vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            shadowmap_pipeline.GetPipeline()
            );

        wvk::render::rcmd::SetViewportAndScissor(
            command_buffer,
            shadowmap_attachments.GetExtent()
            );

        for(auto & pipeline__collection  : pipeline_bindings) {
            
            for (auto & coll__data : pipeline__collection.second) {

                VkDescriptorSet geometry_descriptorset =
                    std::get<0>(coll__data.second);

                for (auto & bind : std::get<1>(coll__data.second)) {
                
                    VkBuffer vertex_buffers[] = {std::get<0>(bind).vertex_buffer};
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
                        std::get<0>(bind).index_buffer,
                        0,
                        VK_INDEX_TYPE_UINT32
                        );

                    std::array<VkDescriptorSet, 2> descsets =
                        {
                            // key light info
                            global_descriptors.DescriptorSet(frame_index),
                            // camera light ubo,
                            geometry_descriptorset
                        };

                    vkCmdBindDescriptorSets(command_buffer,
                                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            shadowmap_pipeline.GetPipelineLayout(),
                                            0,
                                            static_cast<std::uint32_t>(descsets.size()),
                                            descsets.data(),
                                            1,
                                            &std::get<1>(bind));

                    vkCmdDrawIndexed(command_buffer,
                                     std::get<0>(bind).index_count,
                                     1,
                                     0,
                                     0,
                                     0);
                }
            }
        }

        vkCmdEndRendering(command_buffer);

        wvk::render::rcmd::ShadowMap::AttachmentTransitionReadLayout(
            command_buffer,
            shadowmap_attachments.GetDepth(frame_index).Image()
            );

    }

    template<std::uint8_t FramesInFlight>
    inline void Lighting(
        VkDevice device,
        VkCommandBuffer in_command_buffer,
        std::uint32_t in_frame_index,
        WVkAttachmentsLightingRAII<FramesInFlight> & attachments,
        wvk::raii::pipelines::Lighting<FramesInFlight> & pipelines,
        WVkAttachmentsGBuffersRAII<FramesInFlight> const & gbuffer_attachments,
        wvk::raii::attachments::ShadowMap<FramesInFlight> const & shadow_attachments,
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
        VkDescriptorSet descriptorset = wvk::render::rcmd::Lighting::CreateDescriptor(
            device,
            pipelines.DescriptorPool(in_frame_index),
            pipelines.DescriptorSetLayout(),
            plane_sampler,
            gbuffer_attachments.Albedo(in_frame_index).View(),
            gbuffer_attachments.Emission(in_frame_index).View(),
            gbuffer_attachments.Normal(in_frame_index).View(),
            gbuffer_attachments.ORM(in_frame_index).View(),
            gbuffer_attachments.Depth(in_frame_index).View(),
            gbuffer_attachments.Extra01(in_frame_index).View(),
            shadow_attachments.GetDepth(in_frame_index).View()
            );

        // Draw Commands
        VkBuffer vertex_buffers[] = {
            render_plane.vertex_buffer
        };
        VkDeviceSize offsets[] = { 0 };

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

        std::array descsets = {
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
