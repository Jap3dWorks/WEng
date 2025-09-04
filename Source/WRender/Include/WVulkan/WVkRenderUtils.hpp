#pragma once

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>

namespace WVkRenderUtils {
    void RndCmd_TransitionRenderImageLayout(
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
}

