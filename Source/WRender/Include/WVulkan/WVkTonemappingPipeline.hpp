#pragma once

#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WShaderUtils.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVulkanUtils.hpp"
#include "WVulkan/WVkPipelineHelper.hpp"

#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>


template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkTonemappingPipeline {

public:

    WVkTonemappingPipeline() = default;

    virtual ~WVkTonemappingPipeline()=default;

    WVkTonemappingPipeline(const WVkTonemappingPipeline &) = delete;

    WVkTonemappingPipeline(WVkTonemappingPipeline && other) noexcept :
        device_info_(std::move(other.device_info_)),
        pipeline_layout_(std::move(other.pipeline_layout_)),
        pipeline_(std::move(other.pipeline_)),
        descset_layout_(std::move(other.descset_layout_)),
        descriptor_pool_(std::move(other.descriptor_pool_)),
        sampler_(std::move(other.sampler_))
        {
            other.device_info_ = {};

            other.pipeline_layout_ = VK_NULL_HANDLE;
            other.descset_layout_ = VK_NULL_HANDLE;
            other.pipeline_ = VK_NULL_HANDLE;
            other.sampler_ = VK_NULL_HANDLE;

            for(std::uint32_t i=0; i<FramesInFlight; i++) {
                other.descriptor_pool_[i] = VK_NULL_HANDLE;
            }
        }

    WVkTonemappingPipeline & operator=(const WVkTonemappingPipeline & other) = delete;

    WVkTonemappingPipeline & operator=(WVkTonemappingPipeline && other) noexcept {
        if (this != &other) {
            pipeline_ = std::move(other.pipeline_);
            pipeline_layout_ = std::move(other.pipeline_layout_);
            descset_layout_ = std::move(other.descset_layout_);
            descriptor_pool_ = std::move(other.descriptor_pool_);
            sampler_ = std::move(other.sampler_);
            device_info_ = std::move(other.device_info_);

            other.pipeline_ = VK_NULL_HANDLE;
            other.pipeline_layout_ = VK_NULL_HANDLE;
            other.descset_layout_ = VK_NULL_HANDLE;
            other.sampler_ = VK_NULL_HANDLE;
            other.descriptor_pool_ = {};
            other.device_info_ = {};
        }

        return *this;
    }

    void Initialize(const WVkDeviceInfo & in_device,
                    const VkFormat & in_color_format) {
        assert(device_info_.vk_device == VK_NULL_HANDLE);

        device_info_ = in_device;

        InitializeDescSetLayout();

        InitializeRenderPipeline(in_color_format);

        sampler_ = WVulkanUtils::CreateRenderPlaneSampler(device_info_.vk_device);
    }

    void Destroy() {
        if (!device_info_.vk_device) {
            return;
        }

        WVulkan::DestroyDescPools(descriptor_pool_, device_info_);

        if (pipeline_) {
            vkDestroyPipeline(device_info_.vk_device,
                              pipeline_,
                              nullptr);

            pipeline_=VK_NULL_HANDLE;
        }


        if (pipeline_layout_) {
            vkDestroyPipelineLayout(device_info_.vk_device,
                                    pipeline_layout_,
                                    nullptr);
            
            pipeline_layout_ = VK_NULL_HANDLE;
        }

        if (descset_layout_) {
            vkDestroyDescriptorSetLayout(device_info_.vk_device,
                                         descset_layout_,
                                         nullptr);

            descset_layout_ = VK_NULL_HANDLE;
        }

        if (sampler_) {
            WVulkan::Destroy(sampler_, device_info_);

            sampler_ = VK_NULL_HANDLE;
        }

        device_info_ = {};
    }

private:

    void InitializeDescSetLayout() {

        // input render image descriptor
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

    void InitializeDescriptorPool() {
        std::array<VkDescriptorPoolSize, 1> pool_sizes;
        pool_sizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[0].descriptorCount = 1;

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<std::uint32_t>(
            pool_sizes.size()
            );
        pool_info.pPoolSizes = pool_sizes.data();
        pool_info.maxSets = 1;

        for (std::uint32_t i=0; i<FramesInFlight; i++) {
            if (vkCreateDescriptorPool(
                    device_info_.vk_device,
                    &pool_info,
                    nullptr,
                    &descriptor_pool_[i]
                    )) {
                throw std::runtime_error("Failed to create descriptor pool!");
            }
        }
    }

    void InitializeRenderPipeline(VkFormat color_format) {

        std::vector<char> shadercode = WShaderUtils::ReadShader(
            WStringUtils::SystemPath(shader_path)
            );

        VkShaderModule shader_module = WVulkanUtils::CreateShaderModule(
            device_info_.vk_device,
            shadercode.data(),
            shadercode.size()
            );

        std::array<VkPipelineShaderStageCreateInfo,2> shader_stages;
        WVkPipelineHelper::RenderPlane_UpdateVkPipelineShaderStageCreateInfo(
            shader_stages.data(),
            shader_module
            );

        std::array<VkVertexInputAttributeDescription,2> attr_desc;
        WVkPipelineHelper::RenderPlane_UpdateVertexInputAttributeDescriptor(attr_desc.data());

        VkVertexInputBindingDescription binding_desc =
            WVkPipelineHelper::RenderPlane_VertBindingDescrpt();

        VkPipelineVertexInputStateCreateInfo vertex_input_info =
            WVkPipelineHelper::RenderPlane_VkPipelineVertexInputStateCreateInfo(
                binding_desc,
                attr_desc.data(),
                static_cast<std::uint32_t>(attr_desc.size())
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

        pipeline_ = WVkPipelineHelper::RenderPlane_VkPipeline(
            &color_format,
            1,
            VK_FORMAT_D32_SFLOAT,
            graphics_pipeline_info,
            device_info_.vk_device
            );

        vkDestroyShaderModule(device_info_.vk_device,
                              shader_module,
                              nullptr);
    }

private:

    VkPipeline pipeline_{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
    
    VkDescriptorSetLayout descset_layout_{VK_NULL_HANDLE};

    std::array<VkDescriptorPool, FramesInFlight> descriptor_pool_{};
    VkSampler sampler_{VK_NULL_HANDLE};

    // TODO: common resource
    // WVkMeshInfo render_plane_{};

    const char* shader_path{WENG_VK_TONEMAPPING_SHADER_PATH};

    WVkDeviceInfo device_info_{};

};
