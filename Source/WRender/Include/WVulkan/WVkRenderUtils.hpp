#pragma once

#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WCore/WConcepts.hpp"

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <concepts>

namespace WVkRenderUtils {

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

    // TODO: Move to GraphicsPipelineDb?
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

    // TODO: Move to PostprocessPipelineDb?
    inline VkDescriptorSet CreatePostprocessDescriptor(
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

    inline VkDescriptorSet CreateInputRenderDescriptor(
        const VkDevice & in_device,
        const VkDescriptorPool & in_desc_pool,
        const VkDescriptorSetLayout & in_desc_lay,
        const VkImageView & in_input_render_view,
        const VkSampler & in_input_render_sampler
        ) {
        VkDescriptorSet descriptor_set{};
        VkDescriptorSetAllocateInfo alloc_info{};
        
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = in_desc_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &in_desc_lay;

        if (vkAllocateDescriptorSets(
                in_device,
                &alloc_info,
                &descriptor_set
                ) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }

        std::array<VkWriteDescriptorSet, 1> write_ds;

        VkDescriptorImageInfo image_info{};
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = in_input_render_view;
        image_info.sampler = in_input_render_sampler;

        write_ds[0] = {};
        write_ds[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_ds[0].dstBinding = 0; // only 1 binding in this descriptor
        write_ds[0].dstSet = descriptor_set;
        write_ds[0].dstArrayElement=0;
        write_ds[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write_ds[0].descriptorCount=1;
        write_ds[0].pImageInfo = &image_info;
        write_ds[0].pNext = VK_NULL_HANDLE;

        vkUpdateDescriptorSets(
            in_device,
            static_cast<std::uint32_t>(write_ds.size()),
            write_ds.data(),
            0,
            nullptr
            );

        return descriptor_set;
    }

    template<typename T>
    inline std::vector<T> CreateSyncSemaphore(const std::size_t & in_images,
                                              const VkDevice & in_device) {
        std::vector<T> result(in_images);

        VkSemaphoreCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        create_info.pNext = VK_NULL_HANDLE;

        for(std::size_t i=0; i<in_images; i++) {
            T itm{};
            
            if (vkCreateSemaphore(in_device, &create_info, nullptr, &itm.image_available)
                != VK_SUCCESS) {
                throw std::runtime_error("Failed Creating a Semaphore!");
            }

            if (vkCreateSemaphore(in_device, &create_info, nullptr, &itm.render_finished)
               != VK_SUCCESS) {
                throw std::runtime_error("Failed creating a Semaphore!");
            }

            result[i] = itm;
        }

        return result;
    }

    template<std::size_t N>
    inline std::array<VkFence, N> CreateSyncFences(const VkDevice & in_device) {
        VkFenceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        create_info.pNext = VK_NULL_HANDLE;

        std::array<VkFence, N> result;

        for(std::size_t i=0; i<N; i++) {
            VkFence itm;
            if (vkCreateFence(
                    in_device,
                    &create_info,
                    nullptr,
                    &itm
                    ) != VK_SUCCESS) {
                throw std::runtime_error("Failed creating a Fence!");
            }

            result[i] = itm;
        }

        return result;
    }

    template<CIterable<WVkOffscreenRenderStruct> T>
    inline void CreateOffscreenRender(T & in_offscreen_structs,
                                      const WVkDeviceInfo & in_device_info,
                                      const std::uint32_t & in_width,
                                      const std::uint32_t & in_height,
                                      const VkFormat & in_format) {
        for(auto& offrnd : in_offscreen_structs) {
            
            offrnd.extent = {in_width, in_height};
            offrnd.color.extent = {in_width, in_height};
            
            WVulkan::CreateRenderColorResource(
                offrnd.color.image,
                offrnd.color.memory,
                offrnd.color.view,
                in_format,
                in_device_info,
                offrnd.color.extent
                );

            offrnd.depth.extent = {in_width, in_height};
            WVulkan::CreateRenderDepthResource(
                offrnd.depth.image,
                offrnd.depth.memory,
                offrnd.depth.view,
                in_device_info,
                offrnd.depth.extent
                );
        }
    }

    template<typename T>
    inline void DestroySyncSemaphores(std::vector<T> & out_semaphores, const VkDevice & in_device) {
        for(auto & smph : out_semaphores) {
            vkDestroySemaphore(
                in_device,
                smph.image_available,
                nullptr
                );
            smph.image_available = VK_NULL_HANDLE;

            vkDestroySemaphore(
                in_device,
                smph.render_finished,
                nullptr
                );
            smph.render_finished = VK_NULL_HANDLE;
        }
    }

    template<std::size_t N>
    inline void DestroySyncFences(std::array<VkFence, N> & out_fences, const VkDevice & in_device) {
        for(auto & fnc : out_fences) {
            vkDestroyFence(
                in_device,
                fnc,
                nullptr
                );

            fnc = VK_NULL_HANDLE;
        }
    }

    template<CIterable<WVkOffscreenRenderStruct> T>
    void DestroyOffscreenRender(const T & in_offscreen_structs,
                                const WVkDeviceInfo & in_device_info) {
        
        for(auto& offrnd : in_offscreen_structs) {
            
            vkDestroyImageView(in_device_info.vk_device,
                               offrnd.color.view,
                               nullptr);

            vkDestroyImage(in_device_info.vk_device,
                           offrnd.color.image,
                           nullptr);

            vkFreeMemory(in_device_info.vk_device,
                         offrnd.color.memory,
                         nullptr);

            vkDestroyImageView(in_device_info.vk_device,
                               offrnd.depth.view,
                               nullptr);

            vkDestroyImage(in_device_info.vk_device,
                           offrnd.depth.image,
                           nullptr);

            vkFreeMemory(in_device_info.vk_device,
                         offrnd.depth.memory,
                         nullptr);

        }
    }

    template<CIterable<WVkPostprocessRenderStruct> T>
    void CreatePostprocessRender(T & postprocess_structs,
                                 const WVkDeviceInfo & in_device_info,
                                 const std::uint32_t & in_width,
                                 const std::uint32_t & in_height,
                                 const VkFormat & in_format) {
        for (auto & pstrnd : postprocess_structs) {
            pstrnd.extent = {in_width, in_height};
            pstrnd.color.extent = {in_width, in_height};

            WVulkan::CreateRenderColorResource(
            pstrnd.color.image,
            pstrnd.color.memory,
            pstrnd.color.view,
            in_format,
            in_device_info,
            pstrnd.color.extent
            );
            
        }
    }

    template<CIterable<WVkPostprocessRenderStruct> T>
    void DestroyPostprocessRender(const T & postprocess_render,
                                  const WVkDeviceInfo & in_device_info) {
        for (auto & pstrnd : postprocess_render) {
            vkDestroyImageView(
                in_device_info.vk_device,
                pstrnd.color.view,
                nullptr
                );

            vkDestroyImage(
                in_device_info.vk_device,
                pstrnd.color.image,
                nullptr
                );

            vkFreeMemory(
                in_device_info.vk_device,
                pstrnd.color.memory,
                nullptr
                );
        }
    }

    inline void RndCmd_BeginOffscreenRendering(
        const VkCommandBuffer & in_command_buffer,
        const VkImageView & in_color_view,
        const VkImageView & in_depth_view,
        const VkExtent2D & in_extent
        ) {
        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color={{0.5, 0.5, 0.5, 1.f}};
        clear_values[1].depthStencil = {1.f, 0};

        // Color Attachment
        VkRenderingAttachmentInfo color_attachment{};
        color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment.imageView = in_color_view;
        color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.clearValue = clear_values[0];

        // Depth Attachment
        VkRenderingAttachmentInfo depth_attachment{};
        depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depth_attachment.imageView = in_depth_view;
        depth_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
         // If later i need it, should change depth storeOp (I think)
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.clearValue = clear_values[1];

        VkRenderingInfo rendering_info{};
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
        VkClearValue clear_value = {0.5, 0.5, 0.5, 1.f};

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
    
    inline void RndCmd_BeginSwapchainRendering(
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

    template<typename TDcrpLst>
    inline void RndCmd_PostprocessDrawCommands(
        const VkDevice & in_device,
        const VkCommandBuffer & in_command_buffer,
        const VkBuffer & in_vertex_buffer,
        const VkBuffer & in_index_buffer,
        const std::uint32_t & in_index_count,
        const VkPipelineLayout & in_pipeline_layout,
        const VkPipeline & in_pipeline,
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
                                0,
                                nullptr);

        vkCmdDrawIndexed(in_command_buffer,
                         in_index_count,
                         1,0,0,0);
    }

}

