#pragma once

#include "WVulkan/RAII/WVkAttachmentsGBuffersRAII.hpp"
#include "WVulkan/RAII/WVkAttachmentsLightingRAII.hpp"
#include "WVulkan/RAII/WVkPostprocessGlobalDescriptorRAII.hpp"
#include "WVulkan/WVulkanStructs.hpp"

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
            throw std::runtime_error("o!");
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

    template<std::uint8_t FramesInFlight>
    inline void UpdatePPcessGlobalDescriptorSet(
        WVkPostprocessGlobalDescriptorRAII<FramesInFlight> & ppcess_global_descriptor,
        WVkAttachmentsGBuffersRAII<FramesInFlight> const & gbffr_attach,
        WVkAttachmentsLightingRAII<FramesInFlight> const & offscrn_attach,
        VkSampler in_sampler,
        std::uint8_t in_frm_indx
        ) {

        // TODO Read write layouts of each attachment it its Attachment RAII class.        

        auto to_desc_info = [in_sampler](VkImageView in_img_view) -> VkDescriptorImageInfo {
            return {
                .sampler=in_sampler,
                .imageView=in_img_view,
                .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };
        };

        auto to_depth_desc_info =
            [in_sampler]
            (VkImageView in_img_view) -> VkDescriptorImageInfo {
            return {
                .sampler=in_sampler,
                .imageView=in_img_view,
                .imageLayout=VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL
            };
        };

        // gbffr_attach.Depth().

        std::array image_infos {
            to_desc_info(offscrn_attach.Color(in_frm_indx).View()),
            to_desc_info(offscrn_attach.Color(in_frm_indx).View()),
            to_desc_info(gbffr_attach.Albedo(in_frm_indx).View()),
            to_desc_info(gbffr_attach.Emission(in_frm_indx).View()),
            to_desc_info(gbffr_attach.Normal(in_frm_indx).View()),
            to_desc_info(gbffr_attach.ORM(in_frm_indx).View()),
            to_depth_desc_info(gbffr_attach.Depth(in_frm_indx).View()),
            to_desc_info(gbffr_attach.Extra01(in_frm_indx).View())
        };

        ppcess_global_descriptor.UpdateDescriptorSet(
            in_frm_indx,
            image_infos
            );

    }    

    template<std::uint8_t FramesInFlight>
    inline void UpdatePPcessGlobalDescriptorSet(
        WVkPostprocessGlobalDescriptorRAII<FramesInFlight> & out_ppcss,
        const WVkAttachmentsGBuffersRAII<FramesInFlight> & gbffr_attach,
        const WVkAttachmentsLightingRAII<FramesInFlight> & offscrn_attach,
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

