#pragma once

#include "WCore/WStringUtils.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkanUtils.hpp"
#include "WShaderUtils.hpp"

#include <glm/fwd.hpp>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>
#include <glm/glm.hpp>

#include <stdexcept>

template<std::uint32_t FramesInFlight>
class WVkSwapChainResources {
    
public:
    
    WVkSwapChainResources() noexcept = default;
    
    virtual ~WVkSwapChainResources() {
        Destroy();
    }

    WVkSwapChainResources(const WVkSwapChainResources & other) = delete;

    WVkSwapChainResources(WVkSwapChainResources && other) noexcept :
        device_info_(std::move(other.device_info_)),
        pipeline_layout_(std::move(other.pipeline_layout_)),
        pipeline_(std::move(other.pipeline_)),
        descriptor_layout_(std::move(other.descriptor_layout_)),
        descriptor_pool_(std::move(other.descriptor_pool_)),
        input_render_sampler_(std::move(other.input_render_sampler_))
        {
            other.device_info_ = {};
            
            other.pipeline_layout_ = VK_NULL_HANDLE;
            other.descriptor_layout_ = VK_NULL_HANDLE;
            other.pipeline_ = VK_NULL_HANDLE;
            other.input_render_sampler_ = VK_NULL_HANDLE;

            for(std::uint32_t i=0; i<FramesInFlight; i++) {
                other.descriptor_pool_[i] = VK_NULL_HANDLE;
            }
        }

    WVkSwapChainResources & operator=(const WVkSwapChainResources & other) = delete;

    WVkSwapChainResources & operator=(WVkSwapChainResources && other) noexcept {
        if (this != &other) {
            device_info_ = std::move(other.device_info_);
            descriptor_layout_ = std::move(other.descriptor_layout_);
            pipeline_layout_ = std::move(other.pipeline_layout_);
            pipeline_ = std::move(other.pipeline_);
            input_render_sampler_ = std::move(other.input_render_sampler_);

            descriptor_pool_ = std::move(other.descriptor_pool_);

            other.device_info_ = {};
            other.descriptor_layout_ = VK_NULL_HANDLE;
            other.pipeline_layout_ = VK_NULL_HANDLE;
            other.pipeline_ = VK_NULL_HANDLE;
            other.input_render_sampler_ = VK_NULL_HANDLE;

            for(std::uint32_t i=0; i<FramesInFlight; i++) {
                other.descriptor_pool_[i] = VK_NULL_HANDLE;
            }
        }
        return *this;
    }

    void Initialize(const WVkDeviceInfo & in_device, const WVkCommandPoolInfo & in_command_pool) {

        assert(device_info_.vk_device == VK_NULL_HANDLE);

        device_info_ = in_device;

        InitializeDescriptorLayout();

        InitializePipeline();

        InitializeDescriptorPool();

        InitializeInputRenderSampler();

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
        
        if (descriptor_layout_) {
            vkDestroyDescriptorSetLayout(device_info_.vk_device,
                                         descriptor_layout_,
                                         nullptr);
        }

        descriptor_layout_ = VK_NULL_HANDLE;

        if (input_render_sampler_) {
            vkDestroySampler(device_info_.vk_device,
                             input_render_sampler_,
                             nullptr);
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
        return descriptor_layout_;
    }

    const VkSampler & InputRenderSampler() const noexcept {
        return input_render_sampler_;
    }

    const WVkMeshInfo & RenderPlane() const noexcept {
        return render_plane_;
    }

private:

    void InitializeDescriptorLayout() {
        
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
                                        &descriptor_layout_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Descriptor Set Layout!");
        }

    }

    void InitializePipeline() {
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

        // VkShaderStageFlagBits
        shader_stages[0] = VkPipelineShaderStageCreateInfo{};
        shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages[0].pNext=VK_NULL_HANDLE;
        shader_stages[0].module=shader_module;
        shader_stages[0].pName="vsMain";

        shader_stages[1] = VkPipelineShaderStageCreateInfo{};
        shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages[1].pNext = VK_NULL_HANDLE;
        shader_stages[1].module=shader_module;
        shader_stages[1].pName="fsMain";

        std::array<VkVertexInputAttributeDescription, 2> attr_desc;
        attr_desc[0].binding=0;
        attr_desc[0].location=0;
        attr_desc[0].format=VK_FORMAT_R32G32B32_SFLOAT;
        attr_desc[0].offset=0;  // position

        attr_desc[1].binding=0;
        attr_desc[1].location=1;
        attr_desc[1].format = VK_FORMAT_R32G32_SFLOAT;
        attr_desc[1].offset = sizeof(float) * 2; // uv

        VkVertexInputBindingDescription binding_desc{};
        binding_desc.binding=0;
        binding_desc.stride = sizeof(float) * 2 + sizeof(float) * 2;
        binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount=1;
        vertex_input_info.pVertexBindingDescriptions = &binding_desc;
        vertex_input_info.vertexAttributeDescriptionCount=static_cast<std::uint32_t>(
            attr_desc.size()
            );
        vertex_input_info.pVertexAttributeDescriptions=attr_desc.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount=1;
        viewport_state.scissorCount=1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // because of y-flip in the projection matrix
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;  // no msaa

        VkPipelineDepthStencilStateCreateInfo depth_stencil{};
        depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil.depthTestEnable = VK_FALSE;
        depth_stencil.depthWriteEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState color_blend_attachment{};
        color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo color_blending{};
        color_blending.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable = VK_FALSE;
        color_blending.logicOp = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.f;
        color_blending.blendConstants[1] = 0.f;
        color_blending.blendConstants[2] = 0.f;
        color_blending.blendConstants[3] = 0.f;

        std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state.pDynamicStates = dynamic_states.data();

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 1; // slayouts.size();
        pipeline_layout_info.pSetLayouts = &descriptor_layout_; // slayouts.data();

        if (vkCreatePipelineLayout(
                device_info_.vk_device,
                &pipeline_layout_info,
                nullptr,
                &pipeline_layout_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
        pipeline_info.pStages = shader_stages.data();
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multisampling;
        pipeline_info.pDepthStencilState = &depth_stencil;
        pipeline_info.pColorBlendState = &color_blending;
        pipeline_info.pDynamicState = &dynamic_state;
        pipeline_info.layout = pipeline_layout_;

        pipeline_info.renderPass = VK_NULL_HANDLE; // in_render_pass_info.render_pass;
        pipeline_info.subpass = 0;                 // out_pipeline_info.subpass;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

        // Dynamic rendering info

        VkFormat color_format = VK_FORMAT_B8G8R8A8_SRGB; // TODO: USE swap chain format
        VkFormat depth_format = VK_FORMAT_D32_SFLOAT;

        VkPipelineRenderingCreateInfo rendering_info{};
        rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachmentFormats = &color_format;
        rendering_info.depthAttachmentFormat = depth_format;
        rendering_info.pNext = VK_NULL_HANDLE;

        pipeline_info.pNext = &rendering_info;

        if (vkCreateGraphicsPipelines(
        device_info_.vk_device,
        VK_NULL_HANDLE,
        1,
        &pipeline_info,
        nullptr,
        &pipeline_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }

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

    void InitializeInputRenderSampler() {
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

        if(vkCreateSampler(device_info_.vk_device,
                           &sampler_info,
                           nullptr,
                           &input_render_sampler_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sampler!");
        }
    }

    std::array<float, 16> RenderPlaneVertex() noexcept {
        return {
            -1.f, -1.f, 0.f, 0.f,
            1.f, -1.f, 1.f, 0.f,
            1.f, 1.f, 1.f, 1.f,
            -1.f, 1.f, 0.f, 1.f
        };
    }

    std::array<std::uint32_t, 6> RenderPlaneIndexes() noexcept {
        return { 2,1,0,0,3,2 };
    }

    void InitializeRenderPlane(const WVkCommandPoolInfo & in_command_pool) {

        WVulkan::CreateMeshBuffers(
            render_plane_,
            RenderPlaneVertex().data(),
            sizeof(float) * 16,
            RenderPlaneIndexes().data(),
            sizeof(std::uint32_t) * 6,
            6,
            device_info_,
            in_command_pool
            );

    }

    VkPipeline pipeline_{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};

    VkDescriptorSetLayout descriptor_layout_{VK_NULL_HANDLE};

    std::array<VkDescriptorPool, FramesInFlight> descriptor_pool_{};

    VkSampler input_render_sampler_{VK_NULL_HANDLE};

    WVkMeshInfo render_plane_{};

    const char * shader_path{"Content/Shaders/WRender_DrawInSwapChain.graphic.spv"};

    WVkDeviceInfo device_info_{};

};
