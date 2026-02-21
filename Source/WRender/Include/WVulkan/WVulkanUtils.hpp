#pragma once

#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVulkanStructs.hpp"

#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <vulkan/vulkan_core.h>
#include <array>
#include <stdexcept>
#include <vector>

namespace WVulkanUtils {

    // TODO This file should contain more WEng based vulkan functions and procedures

    /**
     * @brief Enum representing the shader stage flags for Vulkan.
     */
    inline VkShaderStageFlagBits ToShaderStageFlagBits(const EShaderStageFlag & type) {
        switch (type)
        {
        case EShaderStageFlag::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case EShaderStageFlag::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case EShaderStageFlag::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            throw std::runtime_error("Invalid shader type!");
        }
    }

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

    inline std::vector<VkShaderModule> CreateShaderModules(
        WVkShaderStageInfo & out_vertex_stage,
        std::vector<VkPipelineShaderStageCreateInfo> & out_shader_stages,
        const VkDevice & in_device,
        const std::vector<WVkShaderStageInfo> & stage_infos) {

        out_shader_stages.clear();
        out_shader_stages.resize(stage_infos.size());
        
        std::vector<VkShaderModule> shader_modules(stage_infos.size(), VK_NULL_HANDLE);

        const WVkShaderStageInfo * vertex_shader_stage = nullptr;

        for (uint32_t i = 0; i < stage_infos.size(); i++)
        {
            out_shader_stages[i] = {};
            out_shader_stages[i].pNext = VK_NULL_HANDLE;
            out_shader_stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            out_shader_stages[i].stage = ToShaderStageFlagBits(
                stage_infos[i].type
                );

            if (stage_infos[i].type == EShaderStageFlag::Vertex)
            {
                vertex_shader_stage = &stage_infos[i];
            }

            shader_modules[i] = WVulkanUtils::CreateShaderModule(in_device,
                                                                 stage_infos[i].code.data(),
                                                                 stage_infos[i].code.size());

            out_shader_stages[i].module = shader_modules[i];
            out_shader_stages[i].pName = stage_infos[i].entry_point.c_str();
        }

        if (vertex_shader_stage == nullptr)
        {
            throw std::runtime_error("Vertex shader stage not found!");
        }

        out_vertex_stage = *vertex_shader_stage;

        return shader_modules;
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

    inline std::array<float, 16> RenderPlaneVertex() noexcept {
        return {
            -1.f, -1.f, 0.f, 0.f,
            1.f, -1.f, 1.f, 0.f,
            1.f, 1.f, 1.f, 1.f,
            -1.f, 1.f, 0.f, 1.f
        };
    }

    inline std::array<std::uint32_t, 6> RenderPlaneIndexes() noexcept {
        return { 2,1,0,0,3,2 };
    }

    WNODISCARD inline VkSampler CreateRenderPlaneSampler(const VkDevice & in_device) {
        VkSampler result;
        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.anisotropyEnable = VK_FALSE;
        sampler_info.maxAnisotropy = 0;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;

        if(vkCreateSampler(in_device,
                           &sampler_info,
                           nullptr,
                           &result) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sampler!");
        }

        return result;
    }

    inline VkShaderStageFlags ToVkShaderStageFlag(const EShaderStageFlag & in_pipe_flags) {

        VkShaderStageFlags result{0};
        for(const auto & f : WRenderUtils::SHADER_STAGE_FLAGS_LIST) {
            
            if ((f & in_pipe_flags) == EShaderStageFlag::None)
                continue; 

            VkShaderStageFlags n{0};
            
            switch(f) {
            case EShaderStageFlag::Vertex:
                n = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case EShaderStageFlag::Fragment:
                n = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case EShaderStageFlag::Compute:
                n = VK_SHADER_STAGE_COMPUTE_BIT;
                break;
            default:
                continue;
            }

            result |= n;
        }

        return result;

    }

    inline void UpdateDescriptorSetLayout(
        WVkDescriptorSetLayoutInfo & out_dsl,
        const WPipeParamDescriptorList & in_param_list
        ) {

        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.reserve(in_param_list.size());

        WRenderUtils::ForEach(
            in_param_list,
            [&bindings]
            (const auto& _prm) {
                VkDescriptorSetLayoutBinding bndng{};

                switch(_prm.type) {

                case EPipeParamType::Ubo:
                    bndng.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;                
                    break;

                case EPipeParamType::Texture:
                    bndng.descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    break;
                
                default:
                    return;
                                  
                }

                bndng.binding = _prm.binding;
                bndng.descriptorCount = 1;
                bndng.pImmutableSamplers = nullptr;
                bndng.stageFlags = ToVkShaderStageFlag(_prm.stage_flags);

                bindings.push_back(bndng);
            }
            );

        out_dsl.bindings = bindings;

    }


}
