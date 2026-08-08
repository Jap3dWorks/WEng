#pragma once

#include "WVulkan/Vk/WVkDescriptor.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"

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

    inline
    void AttachmentTransitionReadLayout(
        VkCommandBuffer command_buffer,
        VkImage depth_image
        ) {
        
    }

}

namespace wvk::render::rcmd::GBuffer {}

namespace wvk::render::rcmd::ShadowMap {

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
        rendering_info.pColorAttachments = nullptr;
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
    
namespace wvk::render::rcmd::Lighting {

    inline VkDescriptorSet CreateDescriptor(
        VkDevice vk_device,
        VkDescriptorPool in_desc_pool,
        VkDescriptorSetLayout in_desc_lay,
        VkSampler in_sampler,
        VkImageView in_albedo_view,
        VkImageView in_emission_view,
        VkImageView in_normal_view,
        VkImageView in_orm_view,
        VkImageView in_depth_view,
        VkImageView in_extra01_view,
        VkImageView in_shadow_view
        // , shadow image view
        ) {

        VkDescriptorSet descriptor_set{};
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = in_desc_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &in_desc_lay;

        if (vkAllocateDescriptorSets(
                vk_device,
                &alloc_info,
                &descriptor_set
                ) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }

        std::array<VkWriteDescriptorSet, WVK_GBUFFERS_COUNT> write_ds;
        std::array<VkDescriptorImageInfo, WVK_GBUFFERS_COUNT> image_infos;

        std::uint32_t idx=0;
        for (const VkImageView & vw : {in_albedo_view,
                                       in_emission_view,
                                       in_normal_view,
                                       in_orm_view,
                                       in_depth_view,
                                       in_extra01_view
            }) {

            image_infos[idx] = wvk::types::VkDescriptorImageInfo();
            image_infos[idx].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_infos[idx].imageView = vw;
            image_infos[idx].sampler = in_sampler;

            write_ds[idx] = wvk::types::VkWriteDescriptorSet();
            write_ds[idx].dstBinding = idx;
            write_ds[idx].dstSet = descriptor_set;
            write_ds[idx].dstArrayElement=0;
            write_ds[idx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write_ds[idx].descriptorCount=1;
            write_ds[idx].pImageInfo = &image_infos[idx];
            write_ds[idx].pNext = VK_NULL_HANDLE;

            idx++;
        }

        // The depth image layout
        image_infos[4].imageLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;

        vkUpdateDescriptorSets(
            vk_device,
            static_cast<std::uint32_t>(write_ds.size()),
            write_ds.data(),
            0,
            nullptr
            );

        return descriptor_set;
    }

    
}

