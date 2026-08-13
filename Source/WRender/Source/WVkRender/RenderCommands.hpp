#pragma once

#include "WVulkan/Vk/WVkDescriptor.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"

#include <utility>
#include <vulkan/vulkan_core.h>

namespace wvk::render::rcmd {

    // TODO bring back render command functions from ./RenderUtils.hpp 

    inline
    void TransitionImageLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_image,
        const VkImageLayout & in_old_layout,
        const VkImageLayout & in_new_layout,
        const VkAccessFlags2 & in_src_access_mask,
        const VkAccessFlags2 & in_dst_access_mask,
        const VkPipelineStageFlags2 & in_src_stage_mask,
        const VkPipelineStageFlags2 & in_dst_stage_mask,
        const VkImageAspectFlags & in_img_aspect=VK_IMAGE_ASPECT_COLOR_BIT
        ) {
        
        VkImageMemoryBarrier2 barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barrier.srcStageMask = in_src_stage_mask;
        barrier.srcAccessMask = in_src_access_mask;
        barrier.dstStageMask = in_dst_stage_mask;
        barrier.dstAccessMask = in_dst_access_mask;
        barrier.oldLayout = in_old_layout;
        barrier.newLayout = in_new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = in_image;
        barrier.subresourceRange = {
            .aspectMask = in_img_aspect,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        VkDependencyInfo dependency_info{};
        dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependency_info.dependencyFlags = {};
        dependency_info.imageMemoryBarrierCount = 1;
        dependency_info.pImageMemoryBarriers = &barrier;
        dependency_info.pNext=VK_NULL_HANDLE;

        vkCmdPipelineBarrier2(
            in_command_buffer,
            &dependency_info
            );
    }

    inline
    void SetViewportAndScissor(
        VkCommandBuffer command_buffer,
        VkExtent2D extent2d
        ) {
        VkViewport viewport{};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = static_cast<float>(extent2d.width);
        viewport.height = static_cast<float>(extent2d.height);
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;
        vkCmdSetViewport(
            command_buffer,
            0, 1,
            &viewport
            );

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent2d;
        vkCmdSetScissor(
            command_buffer,
            0, 1,
            &scissor
            );
    }

    // inline
    // void AttachmentTransitionReadLayout(
    //     VkCommandBuffer command_buffer,
    //     VkImage depth_image
    //     ) {
    // }

}

namespace wvk::render::rcmd::GBuffer {
    
    inline void AttachmentTransitionWriteLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_albedo,
        const VkImage & in_emission,
        const VkImage & in_normal,
        const VkImage & in_orm,
        const VkImage & in_depth,
        const VkImage & in_extra01
        ) {
        // Image Layouts
        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_albedo,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_emission,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_normal,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_orm,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_extra01,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_depth,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT
            );
    }

    inline void BeginRendering(
        const VkCommandBuffer & in_command_buffer,
        const VkImageView & in_albedo_view,
        const VkImageView & in_emission_view,
        const VkImageView & in_normal_view,
        const VkImageView & in_orm_view,
        const VkImageView & in_depth_view,
        const VkImageView & in_extra01_view,
        const VkExtent2D & in_extent
        ) {

        // std::array<VkRenderingAttachmentInfo, WENG_VK_GBUFFERS_COUNT - 1> color_attachments;

        auto albedo_attach = wvk::types::VkRenderingAttachmentInfo();
        albedo_attach.imageView = in_albedo_view;
        albedo_attach.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        albedo_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        albedo_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        // TODO WENG_ALBEDO_CLEAR_VALUE
        albedo_attach.clearValue = {0.18, 0.18, 0.18, 1.f};

        // emission attachment
        auto emission_attach = wvk::types::VkRenderingAttachmentInfo();
        emission_attach.imageView = in_emission_view;
        emission_attach.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        emission_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        emission_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        emission_attach.clearValue = {0.f, 0.f, 0.f, 1.f};

        // Normal Attachment
        auto normal_attach = wvk::types::VkRenderingAttachmentInfo();
        normal_attach.imageView = in_normal_view;
        normal_attach.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        normal_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        normal_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        normal_attach.clearValue = {0.f, 0.f, 0.f, 1.f};

        // orm attachment
        auto orm_attach = wvk::types::VkRenderingAttachmentInfo();
        orm_attach.imageView = in_orm_view;
        orm_attach.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        orm_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        orm_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        orm_attach.clearValue = {0.f, 0.f, 0.f, 1.f};

        // extra01 attachment
        auto extra01_attach = wvk::types::VkRenderingAttachmentInfo();
        extra01_attach.imageView = in_extra01_view;
        extra01_attach.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        extra01_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        extra01_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        extra01_attach.clearValue = {0.f, 0.f, 0.f, 1.f};

        std::array color_attachments = {
            std::move(albedo_attach),
            std::move(emission_attach),
            std::move(normal_attach),
            std::move(orm_attach),
            std::move(extra01_attach)
        };

        // Depth Attachment
        VkRenderingAttachmentInfo depth_attachment =
            wvk::types::VkRenderingAttachmentInfo();
        depth_attachment.imageView = in_depth_view;
        depth_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment.clearValue = {1.f, 0.f};

        VkRenderingInfo rendering_info =
            wvk::types::VkRenderingInfo();
        rendering_info.renderArea = {{0,0}, in_extent};
        rendering_info.layerCount = 1;
        rendering_info.colorAttachmentCount = color_attachments.size();
        rendering_info.pColorAttachments = color_attachments.data();
        rendering_info.pDepthAttachment = &depth_attachment;

        vkCmdBeginRendering(
            in_command_buffer,
            &rendering_info
            );
    }    


    inline void AttachmentTransitionReadLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_albedo,
        const VkImage & in_emission,
        const VkImage & in_normal,
        const VkImage & in_orm,
        const VkImage & in_depth,
        const VkImage & in_extra01
        ) {

        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_albedo,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );

        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_emission,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );

        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_normal,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );

        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_orm,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );

        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_depth,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT
            );

        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_extra01,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );
    }


}

namespace wvk::render::rcmd::shadowmap {

    inline
    void AttachmentTransitionWriteLayout(
        VkCommandBuffer command_buffer,
        VkImage depth_image
        ) {
        wvk::render::rcmd::TransitionImageLayout(
            command_buffer,
            depth_image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT
            );        
    }

    inline
    void AttachmentTransitionReadLayout(
        VkCommandBuffer command_buffer,
        VkImage depth_image
        )
    {
        wvk::render::rcmd::TransitionImageLayout(
            command_buffer,
            depth_image,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT
            );
    }

    inline
    void BeginRendering(
        VkCommandBuffer command_buffer,
        VkImageView depth_view,
        VkExtent2D in_extent
        ){

        VkRenderingAttachmentInfo depth_attachment =
            wvk::types::VkRenderingAttachmentInfo();
        depth_attachment.imageView = depth_view;
        depth_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment.clearValue = {1.f, 0.f};


        VkRenderingInfo rendering_info =
            wvk::types::VkRenderingInfo();
        rendering_info.renderArea = {{0,0}, in_extent};
        rendering_info.layerCount = 1;
        rendering_info.colorAttachmentCount = 0;
        rendering_info.pColorAttachments = VK_NULL_HANDLE;
        rendering_info.pDepthAttachment = &depth_attachment;

        vkCmdBeginRendering(
            command_buffer,
            &rendering_info
            );
    }

    inline VkDescriptorSet CreateDescriptorSet(
        VkDevice device,
        std::uint32_t model_ubo_binding,
        VkDescriptorBufferInfo model_ubo
        ) {

        VkDescriptorSet result;

        VkWriteDescriptorSet write_ds = wvk::types::VkWriteDescriptorSet();

        write_ds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_ds.dstBinding = model_ubo_binding;
        write_ds.dstSet = result;
        write_ds.dstArrayElement = 0;
        write_ds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        write_ds.descriptorCount = 1;
        write_ds.pBufferInfo = &model_ubo;
        write_ds.pImageInfo = VK_NULL_HANDLE;
        write_ds.pNext = VK_NULL_HANDLE;

        vkUpdateDescriptorSets(
            device,
            1,
            &write_ds,
            0,
            nullptr
            );

        return result;
    }
}
    
namespace wvk::render::rcmd::lighting {

    inline void AttachmentTransitionWriteLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_color
        ) {
        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_color,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );
    }

    inline void BeginRendering(
        const VkCommandBuffer & in_command_buffer,
        const VkImageView & in_color_view,
        const VkExtent2D & in_extent
        ) {

        // Color Attachment
        VkRenderingAttachmentInfo color_attachment{};
        color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment.imageView = in_color_view;
        color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.clearValue = {0.5, 0.5, 0.5, 1.f};

        VkRenderingInfo rendering_info{};
        rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        rendering_info.renderArea = {{0,0}, in_extent};
        rendering_info.layerCount = 1;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = &color_attachment;
        rendering_info.pDepthAttachment = VK_NULL_HANDLE;
        rendering_info.pStencilAttachment = VK_NULL_HANDLE;

        vkCmdBeginRendering(
            in_command_buffer,
            &rendering_info
            );
    }
    

    inline VkDescriptorSet CreateDescriptor(
        VkDevice vk_device,
        VkDescriptorPool desc_pool,
        VkDescriptorSetLayout desc_lay,
        VkSampler sampler,
        VkImageView albedo_view,
        VkImageView emission_view,
        VkImageView normal_view,
        VkImageView orm_view,
        VkImageView depth_view,
        VkImageView extra01_view,
        VkImageView shadow_view
        ) {

        VkDescriptorSet descriptor_set{};

        VkDescriptorSetAllocateInfo alloc_info = wvk::types::VkDescriptorSetAllocateInfo();
        
        alloc_info.descriptorPool = desc_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &desc_lay;

        if (vkAllocateDescriptorSets(
                vk_device,
                &alloc_info,
                &descriptor_set
                ) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }

        auto create_image_info =
            [sampler]
            (auto view, bool is_depth=false) constexpr
            -> VkDescriptorImageInfo
            {
                auto result = wvk::types::VkDescriptorImageInfo();
                result.imageLayout = (is_depth)
                    ? VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL
                    : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                result.imageView = view;
                result.sampler = sampler;

                return result;
            };
        
        auto create_write_ds =
            [descriptor_set]
            (std::uint32_t binding, VkDescriptorImageInfo & image_info) constexpr
            -> VkWriteDescriptorSet
            {
                auto write_ds = wvk::types::VkWriteDescriptorSet();
                write_ds.dstBinding = binding;
                write_ds.dstSet = descriptor_set;
                write_ds.dstArrayElement=0;
                write_ds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write_ds.descriptorCount=1;
                write_ds.pImageInfo = &image_info;
                write_ds.pNext = VK_NULL_HANDLE;

                return write_ds;
            };
            
        std::array image_infos {
            create_image_info(albedo_view),
            create_image_info(emission_view),
            create_image_info(normal_view),
            create_image_info(orm_view),
            create_image_info(depth_view, true),
            create_image_info(extra01_view),
            create_image_info(shadow_view, true)
        };

        std::array<VkWriteDescriptorSet, image_infos.size()> write_ds;

        auto add_write_element =
            [&write_ds, &create_write_ds, &image_infos]
            <std::size_t ... Idx>
            (std::index_sequence<Idx...> seq) constexpr {
            ((write_ds[Idx]=create_write_ds(Idx, image_infos[Idx])), ...);
        };

        add_write_element(std::make_index_sequence<image_infos.size()>{});

        vkUpdateDescriptorSets(
            vk_device,
            static_cast<std::uint32_t>(write_ds.size()),
            write_ds.data(),
            0,
            nullptr
            );

        return descriptor_set;
    }

    inline void AttachmentTransitionReadLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_color
        ) {
        wvk::render::rcmd::TransitionImageLayout(
            in_command_buffer,
            in_color,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );
    }
}

namespace wvk::render::rcmd::postprocess {

    inline void AttachmentTransitionWriteLayout(
        VkCommandBuffer command_buffer,
        VkImage image
        ) {
        wvk::render::rcmd::TransitionImageLayout(
            command_buffer,
            image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );
    }

    inline void BeginRendering(
        const VkCommandBuffer & in_command_buffer,
        const VkImageView & in_color_view,
        const VkExtent2D & in_extent
        ) {
        // Color Attachment
        VkRenderingAttachmentInfo color_attachment{};
        color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment.imageView = in_color_view;
        color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.clearValue = {0.5, 0.5, 0.5, 1.f};

        VkRenderingInfo rendering_info{};
        rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        rendering_info.renderArea = {{0,0}, in_extent};
        rendering_info.layerCount = 1;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = &color_attachment;
        rendering_info.pDepthAttachment = VK_NULL_HANDLE;
        rendering_info.pStencilAttachment = VK_NULL_HANDLE;

        vkCmdBeginRendering(
            in_command_buffer,
            &rendering_info
            );
    }

    template<typename TDcrpLst>
    inline void DrawCommands(
        VkDevice in_device,
        VkCommandBuffer in_command_buffer,
        VkBuffer in_vertex_buffer,
        VkBuffer in_index_buffer,
        std::uint32_t in_index_count,
        VkPipelineLayout in_pipeline_layout,
        VkPipeline in_pipeline,
        TDcrpLst && in_descriptors
        ) {
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(in_command_buffer,
                               0,
                               1,
                               &in_vertex_buffer,
                               offsets);

        vkCmdBindIndexBuffer(in_command_buffer,
                             in_index_buffer,
                             0,
                             VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(in_command_buffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                in_pipeline_layout,
                                0,
                                static_cast<std::uint32_t>(std::forward<TDcrpLst>(in_descriptors).size()),
                                std::forward<TDcrpLst>(in_descriptors).data(),
                                0, nullptr);

        vkCmdDrawIndexed(in_command_buffer,
                         in_index_count,
                         1,0,0,0);
    }

    inline void AttachmentTransitionReadLayout(
        VkCommandBuffer command_buffer,
        VkImage image
        ) {
        wvk::render::rcmd::TransitionImageLayout(
            command_buffer,
            image,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );
    }
}

namespace wvk::render::rcmd::tonemapping {

    inline void AttachmentTransitionWriteLayout(
        VkCommandBuffer command_buffer,
        VkImage image
        ) {
        wvk::render::rcmd::TransitionImageLayout(
            command_buffer,
            image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );
    }

    inline void BeginRendering(
        const VkCommandBuffer & in_command_buffer,
        const VkImageView & in_color_view,
        const VkExtent2D & in_extent
        )
    {
        VkRenderingAttachmentInfo color_attachment{};
        color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment.imageView = in_color_view;
        color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.clearValue = {0.5, 0.5, 0.5, 1.f};

        VkRenderingInfo rendering_info{};
        rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        rendering_info.renderArea = {{0,0}, in_extent};
        rendering_info.layerCount = 1;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = &color_attachment;
        rendering_info.pDepthAttachment = VK_NULL_HANDLE;
        rendering_info.pStencilAttachment = VK_NULL_HANDLE;

        vkCmdBeginRendering(
            in_command_buffer,
            &rendering_info
            );        
    }

    inline void AttachmentTransitionReadLayout(
        VkCommandBuffer command_buffer,
        VkImage image
        ) {
        wvk::render::rcmd::TransitionImageLayout(
            command_buffer,
            image,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );
    }
}

namespace wvk::render::rcmd::swapchain {
    inline void AttachmentTransitionWriteLayout(
        VkCommandBuffer command_buffer,
        VkImage image
        ) {
        wvk::render::rcmd::TransitionImageLayout(
            command_buffer,
            image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT            
            );
    }

    inline void BeginRendering(
        const VkCommandBuffer & in_command_buffer,
        const VkImageView & in_color_view,
        const VkImageView & in_resolve_view,
        const VkExtent2D & in_extent
        ) {

        VkClearValue clear_value = {0.5, 0.5, 0.5, 1.f};

        // Color Attachment
        VkRenderingAttachmentInfo color_attachment{};
        color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment.imageView = in_color_view;
        color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.clearValue = clear_value;
        color_attachment.resolveImageView = in_resolve_view;
        color_attachment.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkRenderingInfo rendering_info{};
        rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        rendering_info.renderArea = {{0,0}, in_extent};
        rendering_info.layerCount = 1;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = &color_attachment;

        vkCmdBeginRendering(
            in_command_buffer,
            &rendering_info
            );
    }

    inline void AttachmentTransitionReadLayout(
        VkCommandBuffer command_buffer,
        VkImage image
        ) {
        wvk::render::rcmd::TransitionImageLayout(
            command_buffer,
            image,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            {},
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
            );        
    }

}
