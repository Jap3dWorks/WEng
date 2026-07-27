#pragma once

#include "WVulkan/Vk/WVkRender.hpp"

#include <vulkan/vulkan_core.h>
#include <cstdint>

namespace wvk::render::rec_cmd {

    void GBuffers(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index
        ) {
        wvk::render::RndCmd_TransitionGBufferWriteLayout(
            in_command_buffer,
            gbuffers_attachments_.Albedo(in_frame_index).Image(),
            gbuffers_attachments_.Emission(in_frame_index).Image(),
            gbuffers_attachments_.Normal(in_frame_index).Image(),
            gbuffers_attachments_.ORM(in_frame_index).Image(),
            gbuffers_attachments_.Depth(in_frame_index).Image(),
            gbuffers_attachments_.Extra01(in_frame_index).Image()
            );

        wvk::render::RndCmd_BeginGBuffersRendering(
            in_command_buffer,
            gbuffers_attachments_.Albedo(in_frame_index).View(),
            gbuffers_attachments_.Emission(in_frame_index).View(),
            gbuffers_attachments_.Normal(in_frame_index).View(),
            gbuffers_attachments_.ORM(in_frame_index).View(),
            gbuffers_attachments_.Depth(in_frame_index).View(),
            gbuffers_attachments_.Extra01(in_frame_index).View(),
            gbuffers_attachments_.Extent()
            );

        for(auto pipeline_id : gbuffers_pipelines_.IterPipelines()) {
        
            gbuffers_pipelines_.ResetDescriptorPool(pipeline_id, frame_index_);

            const WVkRenderPipeline & render_pipeline =
                gbuffers_pipelines_.Pipeline(pipeline_id);

            vkCmdBindPipeline(render_command_buffers_[in_frame_index],
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              render_pipeline.pipeline);

            wvk::render::RndCmd_SetViewportAndScissor(
                in_command_buffer,
                gbuffers_attachments_.Extent()
                );

            for (auto & bid : gbuffers_pipelines_.IterBindings(pipeline_id)) {

                auto& binding = gbuffers_pipelines_.GetBinding(bid);

                // Create descriptor
                VkDescriptorSet descriptorset =
                    wvk::render::CreateDescriptorSet(
                        device_.Device(),
                        gbuffers_pipelines_.DescriptorPool(pipeline_id, in_frame_index),
                        gbuffers_pipelines_.DescriptorSetLayout(pipeline_id).descset_layout,
                        frame_index_,
                        binding.ubos,
                        binding.textures
                        );

                auto& mesh_info =
                    asset_render_data_.StaticMeshInfo(
                        binding.mesh_asset_id
                        );

                VkBuffer vertex_buffers[] = {mesh_info.vertex_buffer};
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
                    mesh_info.index_buffer,
                    0,
                    VK_INDEX_TYPE_UINT32
                    );

                std::array<VkDescriptorSet, 2> descsets =
                    {
                        global_descriptors_.DescriptorSet(frame_index_),
                        descriptorset
                    };

                vkCmdBindDescriptorSets(in_command_buffer,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        render_pipeline.pipeline_layout,
                                        0,
                                        static_cast<std::uint32_t>(descsets.size()),
                                        descsets.data(),
                                        0,
                                        nullptr);

                vkCmdDrawIndexed(in_command_buffer,
                                 mesh_info.index_count,
                                 1,
                                 0,
                                 0,
                                 0);
            }
        }
    
        vkCmdEndRendering(in_command_buffer);

        wvk::render::RndCmd_TransitionGBufferReadLayout(
            in_command_buffer,
            gbuffers_attachments_.Albedo(in_frame_index).Image(),
            gbuffers_attachments_.Emission(in_frame_index).Image(),
            gbuffers_attachments_.Normal(in_frame_index).Image(),
            gbuffers_attachments_.ORM(in_frame_index).Image(),
            gbuffers_attachments_.Depth(in_frame_index).Image(),
            gbuffers_attachments_.Extra01(in_frame_index).Image()
            );
    }

    void RecordLightingRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index
        );

    void RecordPostprocessRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index,
        const std::uint32_t & in_image_index
        );

    void RecordTonemappingRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index,
        const std::uint32_t & in_image_index
        );

    void RecordSwapChainRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer,
        const std::uint32_t & in_frame_index,
        const std::uint32_t & in_image_index
        );

}
