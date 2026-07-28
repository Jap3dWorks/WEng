#pragma once

#include "WVulkan/Vk/WVkDescriptor.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"

#include <vulkan/vulkan_core.h>

namespace wvk::render::rcmd {
    
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
        // TODO
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

    VkDescriptorSet CreateDescriptorSet(
        VkDevice device,
        std::uint32_t model_ubo_binding,
        VkDescriptorBufferInfo model_ubo
        ) {

        VkDescriptorSet result;

        VkWriteDescriptorSet write_ds = wvk::types::VkWriteDescriptorSet();
        wvk::descriptor::UpdateWriteDescriptorSet_UBO(
            write_ds,
            model_ubo_binding,
            &model_ubo,
            result
            );

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
    
namespace wvk::render::rcmd::Lighting {}

