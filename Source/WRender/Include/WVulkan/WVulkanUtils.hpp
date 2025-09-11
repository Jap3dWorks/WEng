#pragma once

#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <vulkan/vulkan_core.h>
#include <array>
#include <stdexcept>

namespace WVulkanUtils {
    template<std::uint32_t N>
    VkRenderPass CreateRenderPass(const std::array<VkAttachmentDescription, N> & in_attachment_descriptors,
                                  const VkSubpassDescription & in_subpass_descriptor,
                                  const VkDevice & in_device)
    {

        std::array<VkSubpassDependency, 2> dependencies{};
    
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;  // first subpass
        // End of previous commands
        dependencies[0].srcStageMask =
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_NONE_KHR;
        // Read/write from/to depth
        dependencies[0].dstStageMask =
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].dstAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        // Write to attachment
        dependencies[0].dstStageMask |=
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        dependencies[1].srcSubpass=0;
        dependencies[1].dstSubpass=VK_SUBPASS_EXTERNAL;
        // End of write to attachment
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        // Attachment later read
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = static_cast<uint32_t>(in_attachment_descriptors.size());
        render_pass_info.pAttachments = in_attachment_descriptors.data();
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &in_subpass_descriptor;
        render_pass_info.dependencyCount = static_cast<std::uint32_t>(dependencies.size());
        render_pass_info.pDependencies = dependencies.data();

        VkRenderPass result;

        if (vkCreateRenderPass(in_device,
                               &render_pass_info,
                               nullptr,
                               &result) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create offscreen render pass!");
        }

        return result;
    }

    inline VkShaderModule CreateShaderModule(
        const VkDevice & in_device,
        const std::uint32_t * in_code,
        const std::size_t& in_code_size
    )
    {
        VkShaderModule result;

        VkShaderModuleCreateInfo shader_module_create_info{};
        shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_info.codeSize = in_code_size;
        shader_module_create_info.pCode = in_code;

        if (vkCreateShaderModule(
                in_device, 
                &shader_module_create_info, 
                nullptr, 
                &result
                ) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module!");
        }

        return result;
    }

    inline VkShaderModule CreateShaderModule(
        const VkDevice & in_device,
        const char * in_code,
        const std::size_t& in_code_size
        ) {
        return CreateShaderModule(in_device,
                                  reinterpret_cast<const std::uint32_t *>(in_code),
                                  in_code_size);
    }

    inline void CopyVkBuffer(
        const VkDevice & device,
        const VkCommandPool & command_pool,
        const VkQueue & graphics_queue,
        const VkBuffer & src_buffer,
        const VkBuffer & dst_buffer,
        const VkDeviceSize & size
        ) {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = command_pool;
        alloc_info.commandBufferCount = 1;
        alloc_info.pNext = nullptr;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);

        VkBufferCopy copy_region{};
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphics_queue);

        vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
    }

}
