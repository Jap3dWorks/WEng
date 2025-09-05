#pragma once

#include "WVulkan/WVkRenderStructs.hpp"
#include "WVulkan/WVulkan.hpp"

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

namespace WVkRenderUtils {

    inline void RndCmd_BeginOffscreenRendering(
        const VkCommandBuffer & in_command_buffer,
        const VkImageView & in_color_view,
        const VkImageView & in_depth_view,
        const VkExtent2D & in_extent
        ) {
        std::array<VkClearValue, 2> clear_values;
        clear_values[0].color={{0.5, 0.5, 0.5, 1.f}};
        clear_values[1].depthStencil = {1.f, 0};

        // Color Attachment
        VkRenderingAttachmentInfo color_attachment;
        color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment.imageView = in_color_view;
        color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.clearValue = clear_values[0];

        // Depth Attachment
        VkRenderingAttachmentInfo depth_attachment;
        depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depth_attachment.imageView = in_depth_view;
        depth_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
         // If later i need it, should change depth storeOp (I think)
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.clearValue = clear_values[1];

        VkRenderingInfo rendering_info;
        rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        rendering_info.renderArea = {{0,0}, in_extent};
        rendering_info.layerCount = 1;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = &color_attachment;
        rendering_info.pDepthAttachment = &depth_attachment;

        vkCmdBeginRendering(
            in_command_buffer,
            &rendering_info
            );
    }

    inline void RndCmd_BeginPostprocessRendering(
        const VkCommandBuffer & in_command_buffer,
        const VkImageView & in_color_view,
        const VkExtent2D & in_extent
        ) {

        // Color Attachment
        VkRenderingAttachmentInfo color_attachment;
        color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment.imageView = in_color_view;
        color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.clearValue = {{0.5, 0.5, 0.5, 1.f}};

        VkRenderingInfo rendering_info;
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

    inline void RndCmd_BeginOffscreenRendering(
        const VkCommandBuffer & in_command_buffer,
        const VkImageView & in_color_view,
        const VkExtent2D & in_extent
        ) {
        std::array<VkClearValue,1> clear_values;
        clear_values[0] = {{0.5, 0.5, 0.5, 1.f}};

        // Color Attachment
        VkRenderingAttachmentInfo color_attachment;
        color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment.imageView = in_color_view;
        color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.clearValue = clear_values[0];

        VkRenderingInfo rendering_info;
        rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
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

    inline void RndCmd_SetViewportAndScissor(
        const VkCommandBuffer & in_command_buffer,
        const VkExtent2D & in_extent
        ) {
        VkViewport viewport{};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = static_cast<float>(in_extent.width);
        viewport.height = static_cast<float>(in_extent.height);
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;
        vkCmdSetViewport(
            in_command_buffer,
            0, 1,
            &viewport
            );

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = in_extent;
        vkCmdSetScissor(
            in_command_buffer,
            0, 1,
            &scissor
            );
    }

    inline void RndCmd_TransitionRenderImageLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_image,
        const VkImageLayout & old_layout,
        const VkImageLayout & new_layout,
        const VkAccessFlags2 & srcAccessMask,
        const VkAccessFlags2 & dstAccessMask,
        const VkPipelineStageFlags2 & srcStageMask,
        const VkPipelineStageFlags2 & dstStageMask
        ) {
        VkImageMemoryBarrier2 barrier;
        barrier.srcStageMask = srcStageMask;
        barrier.srcAccessMask = srcAccessMask;
        barrier.dstStageMask = dstStageMask;
        barrier.dstAccessMask = dstAccessMask;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = in_image;
        barrier.subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        VkDependencyInfo dependencyInfo;
        dependencyInfo.dependencyFlags = {};
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &barrier;

        vkCmdPipelineBarrier2(
            in_command_buffer,
            &dependencyInfo
            );
    }

    inline void BeginRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer
        )
    {
        vkResetCommandBuffer(in_command_buffer, 0);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0;
        begin_info.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(
                in_command_buffer,
                &begin_info
                ) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

    }

    inline void EndRenderCommandBuffer(
        const VkCommandBuffer & in_command_buffer
        )
    {
        if(vkEndCommandBuffer(in_command_buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer!");
        }

    }

    inline VkDescriptorSet CreateGraphicsDescriptor(
        const VkDevice & in_device,
        const VkDescriptorPool & in_desc_pool,
        const VkDescriptorSetLayout & in_desc_lay,
        const WVkDescriptorSetUBOBinding & ubo_binding,
        const std::vector<WVkDescriptorSetTextureBinding> & in_textures_binding

        ) {
        
        VkDescriptorSet descriptor_set;
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = in_desc_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &in_desc_lay;

        if (vkAllocateDescriptorSets(
                in_device,
                &alloc_info,
                &descriptor_set
                ) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }    

        std::vector<VkWriteDescriptorSet> write_ds{in_textures_binding.size() + 1};

        WVulkan::UpdateWriteDescriptorSet_UBO(
            write_ds[0],
            ubo_binding.binding,
            ubo_binding.buffer_info,
            descriptor_set
            );

        for(std::uint32_t i=0; i<in_textures_binding.size(); i++) {
            WVulkan::UpdateWriteDescriptorSet_Texture(
                write_ds[i+1],
                in_textures_binding[i].binding,
                in_textures_binding[i].image_info,
                descriptor_set
                );
        }

        vkUpdateDescriptorSets(
            in_device,
            static_cast<std::uint32_t>(write_ds.size()),
            write_ds.data(),
            0,
            nullptr
            );

        return descriptor_set;
    }
}

