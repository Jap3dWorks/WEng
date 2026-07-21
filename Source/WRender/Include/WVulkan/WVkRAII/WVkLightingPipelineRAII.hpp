#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
// #include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkTypes.hpp"
#include "WVulkan/WVk/WVkDescriptor.hpp"
// #include "WVulkan/WVk/WVulkan.hpp"
#include "WVulkan/WVk/WVkShader.hpp"
#include "WVulkan/WVk/WVkRenderPlane.hpp"
#include "WVulkan/WVkRAII/_WVkLightingPipelineRAII_.hpp"

#include "WRender/WShader.hpp"
#include "WString/WString.hpp"
#include <vulkan/vulkan_core.h>

/**
 * @brief Offsccreen pipeline outputs the render using GBuffers.
 */
template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkLightingPipelineRAII {

public:

    WVkLightingPipelineRAII() noexcept = default;

    WVkLightingPipelineRAII(
        const VkDevice & in_device,
        VkDescriptorSetLayout in_global_desc_layout) :
        device_(in_device)
        {
            InitializeDescSetLayout();

            InitializeDescPool();

            InitializeRenderPipeline(in_global_desc_layout);
        }

    ~WVkLightingPipelineRAII() {
        Destroy();
    }

    WVkLightingPipelineRAII(const WVkLightingPipelineRAII &)=delete;
    WVkLightingPipelineRAII & operator=(const WVkLightingPipelineRAII &)=delete;

    WVkLightingPipelineRAII(WVkLightingPipelineRAII && other) noexcept :
        device_(std::move(other.device_)),
        descpool_info_(std::move(other.descpool_info_)),
        descset_layout_(std::move(other.descset_layout_)),
        pipeline_(std::move(other.pipeline_)),
        pipeline_layout_(std::move(other.pipeline_layout_))
        {
            other.device_ = VK_NULL_HANDLE;
            other.descpool_info_ = {};
            other.descset_layout_ = VK_NULL_HANDLE;
            other.pipeline_ = VK_NULL_HANDLE;
            other.pipeline_layout_ = VK_NULL_HANDLE;
        }

    WVkLightingPipelineRAII & operator=(WVkLightingPipelineRAII && other) noexcept {
        if (this != &other) {
            Destroy();

            device_=std::move(other.device_);
            descpool_info_=std::move(other.descpool_info_);
            descset_layout_=std::move(other.descset_layout_);
            pipeline_=std::move(other.pipeline_);
            pipeline_layout_=std::move(other.pipeline_layout_);

            other.device_ = VK_NULL_HANDLE;
            other.descpool_info_={};
            other.descset_layout_ = VK_NULL_HANDLE;
            other.pipeline_ = VK_NULL_HANDLE;
            other.pipeline_layout_ = VK_NULL_HANDLE;

        }
        return *this;
    }

public:

    
    const VkDescriptorPool & DescriptorPool(const std::uint32_t & in_frame_index) const noexcept {
        return descpool_info_[in_frame_index];
    }

    const VkDescriptorSetLayout & DescriptorSetLayout() const noexcept {
        return descset_layout_;
    }

    const VkPipeline & Pipeline() const noexcept {
        return pipeline_;
    }

    const VkPipelineLayout & PipelineLayout() const noexcept {
        return pipeline_layout_;
    }

    void ResetDescriptorPool(const std::uint32_t & in_frame_index) {
        vkResetDescriptorPool(device_,
                              descpool_info_[in_frame_index],
                              0);
    }

private:

    void Destroy() {
        if (device_ != VK_NULL_HANDLE) {
            DestroyRenderPipeline();

            wvk::descriptor::DestroyDescPools(
                descpool_info_,
                device_);

            DestroyDescSetLayout();

            device_ = VK_NULL_HANDLE;
        }
    }

    void InitializeDescPool() {
        for(VkDescriptorPool & descpool : descpool_info_) {
            
            std::array<VkDescriptorPoolSize, 2> pool_sizes;

            pool_sizes[0]={};
            pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            pool_sizes[0].descriptorCount = 4;
        
            pool_sizes[1]={};
            pool_sizes[1].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            pool_sizes[1].descriptorCount = 12;  // TODO to CONFIG

            VkDescriptorPoolCreateInfo pool_info{};
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
            pool_info.pPoolSizes = pool_sizes.data();
            pool_info.maxSets = 32;

            if (vkCreateDescriptorPool(
                    device_,
                    &pool_info,
                    nullptr,
                    &descpool) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create descriptor pool!");
            }
        }
    }

    void InitializeDescSetLayout() {
        // TODO: Uniform Buffer with Render Parameters

        // albedo,emission,normal,ws_position,orm,depth,(extra01)

        std::array<VkDescriptorSetLayoutBinding, WENG_VK_GBUFFERS_COUNT> dsl_bindings;
        for(std::uint32_t i=0; i<dsl_bindings.size(); i++) {
            dsl_bindings[i]=wvk::types::CreateVkDescriptorSetLayoutBinding();
            dsl_bindings[i].binding = i;
            dsl_bindings[i].descriptorCount = 1;
            dsl_bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            dsl_bindings[i].pImmutableSamplers = nullptr;
            dsl_bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<std::uint32_t>(dsl_bindings.size());
        layout_info.pBindings=dsl_bindings.data();

        if (vkCreateDescriptorSetLayout(device_,
                                        &layout_info,
                                        nullptr,
                                        &descset_layout_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Descriptor Set Layout!");
        }
    }

    void InitializeRenderPipeline(
        VkDescriptorSetLayout in_global_set_layout
        ) {
        std::vector<char> shadercode = wrd::shader::ReadShader(
            wstr::SystemPath(std::string(shader_path_))
            );

        VkShaderModule shader_module = wvk::shader::CreateShaderModule(
            device_,
            shadercode.data(),
            shadercode.size()
            );

        std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages;
        wvk::render_plane::UpdateVkPipelineShaderStageCreateInfo(
            shader_stages.data(),
            shader_module
            );

        std::array<VkVertexInputAttributeDescription, 2> attr_desc;
        wvk::render_plane::UpdateVertexInputAttributeDescriptor(attr_desc.data());

        VkVertexInputBindingDescription binding_desc =
            wvk::render_plane::VertBindingDescrpt();

        VkPipelineVertexInputStateCreateInfo vertex_input_info =
            wvk::render_plane::VkPipelineVertexInputStateCreateInfo(
                binding_desc,
                attr_desc.data(),
                static_cast<std::uint32_t>(attr_desc.size())
                );

        VkPipelineInputAssemblyStateCreateInfo input_assembly =
            wvk::render_plane::VkPipelineInputAssemblyStateCreateInfo();

        VkPipelineViewportStateCreateInfo viewport_state =
            wvk::render_plane::VkPipelineViewportStateCreateInfo();

        VkPipelineRasterizationStateCreateInfo rasterizer =
            wvk::render_plane::VkPipelineRasterizationStateCreateInfo();

        VkPipelineMultisampleStateCreateInfo multisampling =
            wvk::render_plane::VkPipelineMultisampleStateCreateInfo();
        
        VkPipelineDepthStencilStateCreateInfo depth_stencil =
            wvk::render_plane::VkPipelineDepthStencilStateCreateInfo();

        VkPipelineColorBlendAttachmentState color_blend_attachment =
            wvk::render_plane::VkPipelineColorBlendAttachmentState();

        VkPipelineColorBlendStateCreateInfo color_blending =
            wvk::render_plane::VkPipelineColorBlendStateCreateInfo(
                color_blend_attachment
                );

        std::vector<VkDynamicState> dynamic_states;
        
        VkPipelineDynamicStateCreateInfo dynamic_state =
            wvk::render_plane::VkPipelineDynamicStateCreateInfo(
                dynamic_states
                );

        std::array<VkDescriptorSetLayout, 2> layouts {
            in_global_set_layout,            
            descset_layout_
        };

        pipeline_layout_ = wvr::offscreen_pipelines::PipelineLayout(
            layouts,
            device_
            );

        VkGraphicsPipelineCreateInfo graphics_pipeline_info =
            wvk::render_plane::VkGraphicsPipelineCreateInfo(
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

        VkFormat color_format = WENG_VK_LIGHTING_RENDER_COLOR_FORMAT;

        pipeline_ = wvk::render_plane::VkPipeline(
            &color_format,
            1,
            graphics_pipeline_info,
            device_
            );

        vkDestroyShaderModule(device_,
                              shader_module,
                              nullptr);
    }

    void DestroyDescSetLayout() {
        vkDestroyDescriptorSetLayout(
            device_,
            descset_layout_,
            nullptr
            );

        descset_layout_ = VK_NULL_HANDLE;
    }

    void DestroyRenderPipeline() {
        vkDestroyPipeline(device_,
                          pipeline_,
                          nullptr);


        pipeline_=VK_NULL_HANDLE;

        vkDestroyPipelineLayout(device_,
                                pipeline_layout_,
                                nullptr);

        pipeline_layout_ = VK_NULL_HANDLE;

    }

private:

    VkDevice device_{VK_NULL_HANDLE};

    std::array<VkDescriptorPool, FramesInFlight> descpool_info_{};
    VkDescriptorSetLayout descset_layout_{VK_NULL_HANDLE};

    VkPipeline pipeline_{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};

    const std::string_view shader_path_{WENG_VK_LIGHTING_SHADER_PATH};

};

