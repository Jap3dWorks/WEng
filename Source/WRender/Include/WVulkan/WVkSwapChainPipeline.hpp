#pragma once

#include "WCore/WStringUtils.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkanUtils.hpp"
#include "WVulkan/WVkPipelineHelper.hpp"
#include "WShaderUtils.hpp"

#include <glm/fwd.hpp>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>
#include <glm/glm.hpp>

#include <stdexcept>

template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkSwapChainPipeline {
    
public:
    
    WVkSwapChainPipeline() noexcept = default;
    
    virtual ~WVkSwapChainPipeline() {
        Destroy();
    }

    WVkSwapChainPipeline(const WVkSwapChainPipeline & other) = delete;

    WVkSwapChainPipeline(WVkSwapChainPipeline && other) noexcept :
        device_info_(std::move(other.device_info_)),
        pipeline_layout_(std::move(other.pipeline_layout_)),
        pipeline_(std::move(other.pipeline_)),
        descset_layout_(std::move(other.descset_layout_)),
        descriptor_pool_(std::move(other.descriptor_pool_)),
        input_render_sampler_(std::move(other.input_render_sampler_))
        {
            other.device_info_ = {};
            
            other.pipeline_layout_ = VK_NULL_HANDLE;
            other.descset_layout_ = VK_NULL_HANDLE;
            other.pipeline_ = VK_NULL_HANDLE;
            other.input_render_sampler_ = VK_NULL_HANDLE;

            for(std::uint32_t i=0; i<FramesInFlight; i++) {
                other.descriptor_pool_[i] = VK_NULL_HANDLE;
            }
        }

    WVkSwapChainPipeline & operator=(const WVkSwapChainPipeline & other) = delete;

    WVkSwapChainPipeline & operator=(WVkSwapChainPipeline && other) noexcept {
        if (this != &other) {
            device_info_ = std::move(other.device_info_);
            descset_layout_ = std::move(other.descset_layout_);
            pipeline_layout_ = std::move(other.pipeline_layout_);
            pipeline_ = std::move(other.pipeline_);
            input_render_sampler_ = std::move(other.input_render_sampler_);

            descriptor_pool_ = std::move(other.descriptor_pool_);

            other.device_info_ = {};
            other.descset_layout_ = VK_NULL_HANDLE;
            other.pipeline_layout_ = VK_NULL_HANDLE;
            other.pipeline_ = VK_NULL_HANDLE;
            other.input_render_sampler_ = VK_NULL_HANDLE;

            for(std::uint32_t i=0; i<FramesInFlight; i++) {
                other.descriptor_pool_[i] = VK_NULL_HANDLE;
            }
        }
        return *this;
    }

    void Initialize(const WVkDeviceInfo & in_device,
                    const WVkCommandPoolInfo & in_command_pool,
                    VkFormat in_swap_chain_format=VK_FORMAT_B8G8R8A8_SRGB) {

        assert(device_info_.vk_device == VK_NULL_HANDLE);

        device_info_ = in_device;

        InitializeDescSetLayout();

        InitializeRenderPipeline(in_swap_chain_format);

        InitializeDescriptorPool();

        input_render_sampler_ =
            WVulkanUtils::CreateRenderPlaneSampler(device_info_.vk_device);

        InitializeRenderPlane(in_command_pool);

    }

    void Destroy() {
        if (!device_info_.vk_device) {
            return;
        }

        for (std::uint32_t i=0; i<FramesInFlight; i++) {
            if (descriptor_pool_[i]) {
                vkDestroyDescriptorPool(
                    device_info_.vk_device,
                    descriptor_pool_[i],
                    nullptr
                    );
            }
            
            descriptor_pool_[i] = VK_NULL_HANDLE;
        }

        if (pipeline_) {
            vkDestroyPipeline(device_info_.vk_device,
                              pipeline_,
                              nullptr);
        }

        pipeline_=VK_NULL_HANDLE;

        if (pipeline_layout_) {
            vkDestroyPipelineLayout(device_info_.vk_device,
                                    pipeline_layout_,
                                    nullptr);
        }

        pipeline_layout_ = VK_NULL_HANDLE;
        
        if (descset_layout_) {
            vkDestroyDescriptorSetLayout(device_info_.vk_device,
                                         descset_layout_,
                                         nullptr);
        }

        descset_layout_ = VK_NULL_HANDLE;

        if (input_render_sampler_) {
            WVulkan::Destroy(input_render_sampler_,
                             device_info_);
        }

        input_render_sampler_=VK_NULL_HANDLE;

        WVulkan::Destroy(render_plane_,
                         device_info_);

        device_info_ = {};
    }

    const VkPipeline & Pipeline() const noexcept { return pipeline_; }

    const VkPipelineLayout & PipelineLayout() const noexcept {
        return pipeline_layout_;
    }

    const VkDescriptorPool & DescriptorPool(const std::uint32_t & in_frame_index) const noexcept {
        return descriptor_pool_[in_frame_index];
    }

    const VkDescriptorSetLayout & DescriptorSetLayout() const noexcept {
        return descset_layout_;
    }

    const VkSampler & InputRenderSampler() const noexcept {
        return input_render_sampler_;
    }

    const WVkMeshInfo & RenderPlane() const noexcept {
        return render_plane_;
    }

    void ResetDescriptorPool(const std::uint32_t & in_frame_index) {
        vkResetDescriptorPool(device_info_.vk_device,
                              descriptor_pool_[in_frame_index],
                              0);
    }

private:

    void InitializeDescSetLayout() {
        
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

    void InitializeRenderPipeline(VkFormat swap_chain_format=VK_FORMAT_B8G8R8A8_SRGB) {
        // shader modules
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
            swap_chain_format,
            VK_FORMAT_D32_SFLOAT,   // depth format
            graphics_pipeline_info,
            device_info_.vk_device
            );

        vkDestroyShaderModule(device_info_.vk_device,
                              shader_module,
                              nullptr);
    }

    void InitializeDescriptorPool() {
        std::array<VkDescriptorPoolSize, 2> pool_sizes;
        pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount = 1;
        pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[1].descriptorCount = 1;

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<std::uint32_t>(
            pool_sizes.size()
            );
        pool_info.pPoolSizes = pool_sizes.data();
        pool_info.maxSets = 2;

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

    void InitializeRenderPlane(const WVkCommandPoolInfo & in_command_pool) {

        auto plane_vertex = WVulkanUtils::RenderPlaneVertex();
        auto plane_index = WVulkanUtils::RenderPlaneIndexes();

        WVulkan::CreateMeshBuffers(
            render_plane_,
            WVulkanUtils::RenderPlaneVertex().data(),
            sizeof(decltype(plane_vertex)::value_type) * plane_vertex.size(),
            // sizeof(float) * 16,
            WVulkanUtils::RenderPlaneIndexes().data(),
            sizeof(decltype(plane_index)::value_type) * plane_index.size(),
            // sizeof(std::uint32_t) * 6,
            plane_index.size(),
            device_info_,
            in_command_pool
            );
    }

    VkPipeline pipeline_{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};

    VkDescriptorSetLayout descset_layout_{VK_NULL_HANDLE};

    std::array<VkDescriptorPool, FramesInFlight> descriptor_pool_{};

    VkSampler input_render_sampler_{VK_NULL_HANDLE};

    WVkMeshInfo render_plane_{};

    const char * shader_path{WENG_VK_SWAPCHAIN_SHADER_PATH};

    WVkDeviceInfo device_info_{};

};
