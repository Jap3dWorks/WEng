#pragma once

#include "WVulkan/WVkRAII/WVkAttachmentsGBuffersRAII.hpp"
#include "WVulkan/WVkRAII/WVkAttachmentsOffscreenRAII.hpp"
#include "WVulkan/WVkRAII/WVkGBufferPipelinesRAII.hpp"
#include "WVulkan/WVkRAII/WVkPostprocessGlobalDescriptorRAII.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkDescriptor.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include "WVulkan/WVk/WVkImage.hpp"
#include "WCore/WConcepts.hpp"

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

namespace wvk::render {

    inline WVkRenderDebugInfo CreateWVkRenderDebugInfo(bool in_enable_validation_layers) {
        return {
            .enable_validation_layers = in_enable_validation_layers,
            .validation_layers = {"VK_LAYER_KHRONOS_validation"},
            .debug_callback = VK_NULL_HANDLE,
            .debug_messenger = VK_NULL_HANDLE
        };
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

    template<std::uint8_t FramesInFlight>
    inline VkDescriptorSet CreateDescriptorSet(
        const VkDevice & in_device,
        const VkDescriptorPool & in_desc_pool,
        const VkDescriptorSetLayout & in_desc_lay,
        const std::uint32_t & in_frame_index,
        std::vector<WVkDescSetUBOBinding<FramesInFlight>> const & ubo_binding,
        std::vector<WVkDescSetTextureBinding> const & in_textures_binding
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

        std::vector<VkWriteDescriptorSet> write_ds{};
        write_ds.reserve(ubo_binding.size() + in_textures_binding.size());

        for(auto & ubo_desc : ubo_binding) {
            write_ds.push_back({});

            wvk::descriptor::UpdateWriteDescriptorSet_UBO(
                write_ds.back(),
                ubo_desc.binding,
                &(ubo_desc.ubo_desc[in_frame_index].desc_buffer),
                descriptor_set
                );
        }

        for (auto & texbnd : in_textures_binding) {
            write_ds.push_back({});
            
            wvk::descriptor::UpdateWriteDescriptorSet_Texture(
                write_ds.back(),
                texbnd.binding,
                texbnd.image_info,
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

    inline VkDescriptorSet CreateOffscreenRenderDescriptor(
        const VkDevice & vk_device,
        const VkDescriptorPool & in_desc_pool,
        const VkDescriptorSetLayout & in_desc_lay,
        const VkSampler & in_sampler,
        const VkImageView & in_albedo_view,
        const VkImageView & in_emission_view,
        const VkImageView & in_normal_view,
        const VkImageView & in_ws_position_view,
        const VkImageView & in_mrAO_view,
        const VkImageView & in_depth_view,
        const VkImageView & in_extra01_view
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

        std::array<VkWriteDescriptorSet, WENG_VK_GBUFFERS_COUNT> write_ds;
        std::array<VkDescriptorImageInfo, WENG_VK_GBUFFERS_COUNT> image_infos;

        std::uint32_t idx=0;
        for (const VkImageView & vw : {in_albedo_view,
                                       in_emission_view,
                                       in_normal_view,
                                       in_ws_position_view,
                                       in_mrAO_view,
                                       in_depth_view,
                                       in_extra01_view
                                       }) {

            image_infos[idx] = wvk::types::CreateVkDescriptorImageInfo();
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
        image_infos[5].imageLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;

        vkUpdateDescriptorSets(
            vk_device,
            static_cast<std::uint32_t>(write_ds.size()),
            write_ds.data(),
            0,
            nullptr
            );

        return descriptor_set;
    }

    /**
     * @DEPRECATED
     */
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

    /**
     * @DEPRECATED
     */
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
    inline void DestroySyncFences(std::array<VkFence, N> & out_fences,
                                  const VkDevice & in_device)
    {
        for(auto & fnc : out_fences) {
            vkDestroyFence(
                in_device,
                fnc,
                nullptr
                );

            fnc = VK_NULL_HANDLE;
        }
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

    inline void RndCmd_TransitionGBufferWriteLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_albedo,
        const VkImage & in_emission,
        const VkImage & in_normal,
        const VkImage & in_ws_position,
        const VkImage & in_mrAO,
        const VkImage & in_depth,
        const VkImage & in_extra01
        ) {
        // Image Layouts
        RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            in_albedo,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            in_emission,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            in_normal,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            in_ws_position,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            in_mrAO,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            in_extra01,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        RndCmd_TransitionRenderImageLayout(
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

    inline void RndCmd_TransitionGBufferReadLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_albedo,
        const VkImage & in_emission,
        const VkImage & in_normal,
        const VkImage & in_ws_position,
        const VkImage & in_mrAO,
        const VkImage & in_depth,
        const VkImage & in_extra01
        ) {

        wvk::render::RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            in_albedo,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );

        wvk::render::RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            in_emission,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );

        wvk::render::RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            in_normal,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );

        wvk::render::RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            in_ws_position,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );

        wvk::render::RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            in_mrAO,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );

        wvk::render::RndCmd_TransitionRenderImageLayout(
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

        wvk::render::RndCmd_TransitionRenderImageLayout(
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

    inline void RndCmd_BeginGBuffersRendering(
        const VkCommandBuffer & in_command_buffer,
        const VkImageView & in_albedo_view,
        const VkImageView & in_emission_view,
        const VkImageView & in_normal_view,
        const VkImageView & in_ws_position_view,
        const VkImageView & in_mrAO_view,
        const VkImageView & in_depth_view,
        const VkImageView & in_extra01_view,
        const VkExtent2D & in_extent
        ) {

        std::array<VkRenderingAttachmentInfo, WENG_VK_GBUFFERS_COUNT - 1> color_attachments;

        // albedo Attachment
        color_attachments[0] = wvk::types::CreateVkRenderingAttachmentInfo();
        color_attachments[0].imageView = in_albedo_view;
        color_attachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        // TODO WENG_ALBEDO_CLEAR_VALUE
        color_attachments[0].clearValue = {0.18, 0.18, 0.18, 1.f};

        // emission attachment
        color_attachments[1] = wvk::types::CreateVkRenderingAttachmentInfo();
        color_attachments[1].imageView = in_emission_view;
        color_attachments[1].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachments[1].clearValue = {0.f, 0.f, 0.f, 1.f};

        // Normal Attachment
        color_attachments[2] = wvk::types::CreateVkRenderingAttachmentInfo();
        color_attachments[2].imageView = in_normal_view;
        color_attachments[2].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachments[2].clearValue = {0.f, 0.f, 0.f, 1.f};

        // WS Position Attachment
        color_attachments[3] = wvk::types::CreateVkRenderingAttachmentInfo();
        color_attachments[3].imageView = in_ws_position_view;
        color_attachments[3].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachments[3].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachments[3].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachments[3].clearValue = {0.f, 0.f, 0.f, 1.f};

        // mrAO attachment
        color_attachments[4] = wvk::types::CreateVkRenderingAttachmentInfo();
        color_attachments[4].imageView = in_mrAO_view;
        color_attachments[4].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachments[4].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachments[4].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachments[4].clearValue = {0.f, 0.f, 0.f, 1.f};

        // extra01 attachment
        color_attachments[5] = wvk::types::CreateVkRenderingAttachmentInfo();
        color_attachments[5].imageView = in_extra01_view;
        color_attachments[5].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachments[5].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachments[5].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachments[5].clearValue = {0.f, 0.f, 0.f, 1.f};

        // Depth Attachment
        VkRenderingAttachmentInfo depth_attachment =
            wvk::types::CreateVkRenderingAttachmentInfo();
        depth_attachment.imageView = in_depth_view;
        depth_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment.clearValue = {1.f, 0.f};

        VkRenderingInfo rendering_info =
            wvk::types::CreateVkRenderingInfo();
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

    inline void RndCmd_TransitionOffscreenWriteLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_color
        ) {
        RndCmd_TransitionRenderImageLayout(
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

    inline void RndCmd_TransitionOffscreenReadLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_color
        ) {
        wvk::render::RndCmd_TransitionRenderImageLayout(
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

    inline void RndCmd_BeginOffscreenRendering(
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

    inline void RndCmd_BeginPostprocessRendering(
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

    inline void RndCmd_TransitionTonemappingWriteLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_color
        ) {
        wvk::render::RndCmd_TransitionRenderImageLayout(
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

    inline void RndCmd_TransitionTonemappingReadLayout(
        const VkCommandBuffer & in_command_buffer,
        const VkImage & in_color
        ) {
        wvk::render::RndCmd_TransitionRenderImageLayout(
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

    inline void RndCmd_BeginTonemappingRendering(
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

    inline VkDescriptorSet CreateTonemappingDescriptor(
        const VkDevice & vk_device,
        const VkDescriptorPool & in_desc_pool,
        const VkDescriptorSetLayout & in_desc_lay,
        const VkSampler & in_sampler,
        const VkImageView & in_color_view
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
                ) != VK_SUCCESS ) {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }

        std::array<VkWriteDescriptorSet,1> write_ds;
        std::array<VkDescriptorImageInfo,1> image_info;

        image_info[0] = {};
        image_info[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info[0].imageView = in_color_view;
        image_info[0].sampler = in_sampler;
        
        write_ds[0] = {};
        write_ds[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_ds[0].dstBinding = 0;
        write_ds[0].dstSet = descriptor_set;
        write_ds[0].dstArrayElement = 0;
        write_ds[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write_ds[0].descriptorCount=1;
        write_ds[0].pImageInfo = &image_info[0];
        write_ds[0].pNext = VK_NULL_HANDLE;

        vkUpdateDescriptorSets(
            vk_device,
            static_cast<std::uint32_t>(write_ds.size()),
            write_ds.data(),
            0,
            nullptr
            );

        return descriptor_set;
    }

    inline void TonemappingBindings(
        const VkCommandBuffer & in_command_buffer,
        const VkBuffer & in_vertex_buffer,
        const VkBuffer & in_index_buffer,
        const VkDeviceSize & in_offsets,
        VkDescriptorSet in_descriptorset,
        const VkPipelineLayout & in_pipeline_layout
        ) {
        vkCmdBindVertexBuffers(
            in_command_buffer,
            0,
            1,
            &in_vertex_buffer,
            &in_offsets
            );

        vkCmdBindIndexBuffer(
            in_command_buffer,
            in_index_buffer,
            0,
            VK_INDEX_TYPE_UINT32
            );

        std::array<VkDescriptorSet,1> descsets = {
            in_descriptorset
        };

        vkCmdBindDescriptorSets(in_command_buffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                in_pipeline_layout,
                                0,
                                static_cast<std::uint32_t>(descsets.size()),
                                descsets.data(),
                                0,
                                nullptr);
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

    template<std::uint8_t FramesInFlight>
    inline void UpdatePPcessGlobalDescriptorSet(
        WVkPostprocessGlobalDescriptorRAII<FramesInFlight> & out_ppcss,
        const WVkAttachmentsGBuffersRAII<FramesInFlight> & gbffr_attach,
        const WVkAttachmentsOffscreenRAII<FramesInFlight> & offscrn_attach,
        VkSampler in_sampler,
        std::uint8_t in_frm_indx
        ) {

        auto to_desc_info = [in_sampler](VkImageView in_img_view) -> VkDescriptorImageInfo {
            return {
                .sampler=in_sampler,
                .imageView=in_img_view,
                .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };
        };

        // std::array<VkDescriptorImageInfo,
        //            out_ppcss.BINDING_COUNT> image_infos;

        // image_infos[out_ppcss.PREV_BINDING] =
        //     to_desc_info(offscrn_attach.Color(in_frm_indx).View());

        // image_infos[out_ppcss.COLOR_BINDING] =
        //     to_desc_info(offscrn_attach.Color(in_frm_indx).View());

        // image_infos[out_ppcss.ALBEDO_BINDING] =
        //     to_desc_info(gbffr_attach.Albedo(in_frm_indx).View());

        // image_infos[out_ppcss.COLOR_BINDING] =
        //     to_desc_info()
            
        // image_infos[out_ppcss.NORMAL_BINDING] =
        //     to_desc_info(gbffr_attach.Normal(in_frm_indx).View());
            
        // image_infos[out_ppcss.WS_POSITION_BINDING] =
        //     to_desc_info(gbffr_attach.WsPosition(in_frm_indx).View());

        // image_infos[out_ppcss.DEPTH_BINDING] =
        //     to_desc_info(gbffr_attach.Depth(in_frm_indx).View());
        

        std::array image_infos {
            to_desc_info(offscrn_attach.Color(in_frm_indx).View()),
            to_desc_info(offscrn_attach.Color(in_frm_indx).View()),
            to_desc_info(gbffr_attach.Albedo(in_frm_indx).View()),
            to_desc_info(gbffr_attach.Emission(in_frm_indx).View()),
            to_desc_info(gbffr_attach.Normal(in_frm_indx).View()),
            to_desc_info(gbffr_attach.WsPosition(in_frm_indx).View()),
            to_desc_info(gbffr_attach.MrAO(in_frm_indx).View()),
            to_desc_info(gbffr_attach.Depth(in_frm_indx).View()),
            to_desc_info(gbffr_attach.Extra01(in_frm_indx).View())
        };

        out_ppcss.UpdateDescriptorSet(
            in_frm_indx,
            image_infos
            );

    }    

    template<std::uint8_t FramesInFlight>
    inline void UpdatePPcessGlobalDescriptorSet(
        WVkPostprocessGlobalDescriptorRAII<FramesInFlight> & out_ppcss,
        const WVkAttachmentsGBuffersRAII<FramesInFlight> & gbffr_attach,
        const WVkAttachmentsOffscreenRAII<FramesInFlight> & offscrn_attach,
        VkSampler in_sampler
        ) {

        for(std::uint32_t frm=0; frm<FramesInFlight; frm++) {
            
            UpdatePPcessGlobalDescriptorSet(
                out_ppcss,
                gbffr_attach,
                offscrn_attach,
                in_sampler,
                frm
                );
        }
    }

}

