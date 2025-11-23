#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkanUtils.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVkPipelineHelper.hpp"

#include "WShaderUtils.hpp"
#include "WCore/WStringUtils.hpp"
#include <vulkan/vulkan_core.h>

/**
 * @brief Offsccreen pipeline outputs the render using GBuffers.
 */
template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkOffscreenPipeline {

public:

    WVkOffscreenPipeline() noexcept = default;

    virtual ~WVkOffscreenPipeline() = default;

public:

    void Initialize(const WVkDeviceInfo & in_device,
                    const WVkCommandPoolInfo & in_command_pool) {
        device_info_ = in_device;

        auto plane_vertex = WVulkanUtils::RenderPlaneVertex();
        auto plane_index = WVulkanUtils::RenderPlaneIndexes();

        WVulkan::CreateMeshBuffers(
            render_plane_,
            WVulkanUtils::RenderPlaneVertex().data(),
            sizeof(decltype(plane_vertex)::value_type) * plane_vertex.size(),
            WVulkanUtils::RenderPlaneIndexes().data(),
            sizeof(decltype(plane_index)::value_type) * plane_index.size(),
            plane_index.size(),
            in_device,
            in_command_pool
            );

        InitializeDescSetLayout();

        InitializeDescPool();

        render_sampler_ = WVulkanUtils::CreateRenderPlaneSampler(
            in_device.vk_device
            );
    }

    void Destroy() {
        if (device_info_.vk_device != VK_NULL_HANDLE) {
            DestroyRenderPipeline();

            DestroyDescPool();

            DestroyDescSetLayout();

            WVulkan::Destroy(
                render_sampler_,
                device_info_
                );

            WVulkan::Destroy(
                render_plane_,
                device_info_
                );

            device_info_ = {};
        }
    }

    const WVkMeshInfo & RenderPlane() const noexcept{
        return render_plane_;
    }

    const WVkDescriptorPoolInfo & DescriptorPool(const std::uint32_t & in_frame_index) const noexcept {
        return descpool_info_[in_frame_index];
    }

    const VkDescriptorSetLayout & DescSetLayout() const noexcept {
        return descset_layout_;
    }

    const VkSampler & Sampler() const noexcept {
        return render_sampler_;
    }

private:

    void UpdateDSL(WVkDescriptorSetLayoutInfo & out_descset_lay) {
        VkDescriptorSetLayoutBinding albedo_binding{};
        albedo_binding.binding=0;
        albedo_binding.descriptorCount=1;
        albedo_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        albedo_binding.pImmutableSamplers = nullptr;
        albedo_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding normal_binding{};
        normal_binding.binding=0;
        normal_binding.descriptorCount=1;
        normal_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        normal_binding.pImmutableSamplers = nullptr;
        normal_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding ws_position_binding{};
        ws_position_binding.binding=0;
        ws_position_binding.descriptorCount=1;
        ws_position_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        ws_position_binding.pImmutableSamplers = nullptr;
        ws_position_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        // extras ...

        VkDescriptorSetLayoutBinding depth_binding{};
        depth_binding.binding=0;
        depth_binding.descriptorCount=1;
        depth_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        depth_binding.pImmutableSamplers = nullptr;
        depth_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        out_descset_lay.bindings = {
            albedo_binding,
            normal_binding,
            ws_position_binding,
            // extras ..
            depth_binding
        };
    }

    void InitializeDescPool() {
        for(auto & descpool : descpool_info_) {
            
            std::array<VkDescriptorPoolSize, 2> pool_sizes;

            pool_sizes[0]={};
            pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            // bytes reserved for Uniform Buffer descriptors (like shader parameters)
            pool_sizes[0].descriptorCount = 32;
        
            pool_sizes[1]={};
            pool_sizes[1].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            pool_sizes[1].descriptorCount = 10;

            VkDescriptorPoolCreateInfo pool_info{};
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
            pool_info.pPoolSizes = pool_sizes.data();
            pool_info.maxSets = 32;

            if (vkCreateDescriptorPool(
                    device_info_.vk_device,
                    &pool_info,
                    nullptr,
                    &descpool.descriptor_pool) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create descriptor pool!");
            }
        }
    }

    void InitializeDescSetLayout() {
        // TODO: Uniform Buffer with Render Parameters
        VkDescriptorSetLayoutBinding sampler_binding{};
        sampler_binding.binding = 0;
        sampler_binding.descriptorCount = 1;
        sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_binding.pImmutableSamplers = nullptr;
        sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 1;
        layout_info.pBindings=&sampler_binding;

        if (vkCreateDescriptorSetLayout(device_info_.vk_device,
                                        &layout_info,
                                        nullptr,
                                        &descset_layout_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Descriptor Set Layout!");
        }
    }

    void InitializeRenderPipeline() {
        std::vector<char> shadercode = WShaderUtils::ReadShader(
            WStringUtils::SystemPath(shader_path_)
            );

        VkShaderModule shader_module = WVulkanUtils::CreateShaderModule(
            device_info_.vk_device,
            shadercode.data(),
            shadercode.size()
            );

        std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages;
        WVkPipelineHelper::RenderPlane_UpdateVkPipelineShaderStageCreateInfo(
            shader_stages.data(),
            shader_module
            );

        std::array<VkVertexInputAttributeDescription, 2> attr_desc;
        WVkPipelineHelper::RenderPlane_UpdateVertexInputAttributeDescriptor(attr_desc.data());

        VkVertexInputBindingDescription binding_desc =
            WVkPipelineHelper::RenderPlane_VertBindingDescrpt();

                VkPipelineVertexInputStateCreateInfo vertex_input_info =
            WVkPipelineHelper::RenderPlane_VkPipelineVertexInputStateCreateInfo(
                binding_desc, attr_desc.data(), static_cast<std::uint32_t>(attr_desc.size())
                );

        VkPipelineInputAssemblyStateCreateInfo input_assembly =
            WVkPipelineHelper::RenderPlane_VkPipelineInputAssemblyStateCreateInfo();

        VkPipelineViewportStateCreateInfo viewport_state =
            WVkPipelineHelper::RenderPlane_VkPipelineViewportStateCreateInfo();

        VkPipelineRasterizationStateCreateInfo rasterizer =
            WVkPipelineHelper::RenderPlane_VkPipelineRasterizationStateCreateInfo();

        VkPipelineMultisampleStateCreateInfo multisampling =
            WVkPipelineHelper::RenderPlane_VkPipelineMultisampleStateCreateInfo();
        
        VkPipelineDepthStencilStateCreateInfo depth_stencil =
            WVkPipelineHelper::RenderPlane_VkPipelineDepthStencilStateCreateInfo();

        VkPipelineColorBlendAttachmentState color_blend_attachment =
            WVkPipelineHelper::RenderPlane_VkPipelineColorBlendAttachmentState();

        VkPipelineColorBlendStateCreateInfo color_blending =
            WVkPipelineHelper::RenderPlane_VkPipelineColorBlendStateCreateInfo(
                color_blend_attachment
                );

        std::vector<VkDynamicState> dynamic_states;
        VkPipelineDynamicStateCreateInfo dynamic_state =
            WVkPipelineHelper::RenderPlane_VkPipelineDynamicStateCreateInfo(
                dynamic_states
                );

        pipeline_layout_ = WVkPipelineHelper::RenderPlane_VkPipelineLayout(
            device_info_.vk_device,
            descset_layout_
            );

        VkGraphicsPipelineCreateInfo graphics_pipeline_info =
            WVkPipelineHelper::RenderPlane_VkGraphicsPipelineCreateInfo(
                static_cast<std::uint32_t>(shader_stages.size()),
                shader_stages.data(),
                &vertex_input_info,
                &input_assembly,
                &viewport_state,
                &rasterizer,
                &multisampling,
                &depth_stencil,
                &color_blending,
                &dynamic_state,
                pipeline_layout_
            );

        // Dynamic rendering info

        pipeline_ = WVkPipelineHelper::RenderPlane_VkPipeline(
            VK_FORMAT_B8G8R8A8_SRGB, // TODO: 16 bit format?
            VK_FORMAT_D32_SFLOAT,
            graphics_pipeline_info,
            device_info_.vk_device
            );

        vkDestroyShaderModule(device_info_.vk_device,
                              shader_module,
                              nullptr);
    }

    void DestroyDescSetLayout() {
        vkDestroyDescriptorSetLayout(
            device_info_.vk_device,
            descset_layout_,
            nullptr
            );

        descset_layout_ = VK_NULL_HANDLE;
    }

    void DestroyDescPool() {
        for(auto & descpool : descpool_info_) {
            WVulkan::Destroy(descpool, device_info_);
        }
    }

    void DestroyRenderPipeline() {
        vkDestroyPipeline(device_info_.vk_device,
                          pipeline_,
                          nullptr);


        pipeline_=VK_NULL_HANDLE;

        vkDestroyPipelineLayout(device_info_.vk_device,
                                pipeline_layout_,
                                nullptr);

        pipeline_layout_ = VK_NULL_HANDLE;

    }

private:

    std::array<WVkDescriptorPoolInfo, FramesInFlight> descpool_info_{};
    VkDescriptorSetLayout descset_layout_{};
    WVkMeshInfo render_plane_{};
    VkSampler render_sampler_{};
        
    // WVkRenderPipelineInfo render_pipeline_{};
    VkPipeline pipeline_{};
    VkPipelineLayout pipeline_layout_{};
    WVkDeviceInfo device_info_{};

    const char * shader_path_{"Content/Shaders/WRender_Offscreen.spv"}; // TODO Config file

};

