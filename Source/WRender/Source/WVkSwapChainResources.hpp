#pragma once

#include "WVulkan/WVkRenderStructs.hpp"
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>

#include <stdexcept>

template<std::uint32_t ImagesInFlight>
class WVkSwapChainResources {

    WVkSwapChainResources();

    void Initialize() {

        InitializeDescriptorLayout();

        InitializePipeline();
        
    }

    void Destroy() {
        
    }

private:

    void InitializeDescriptorLayout() {
        VkDescriptorSetLayoutBinding sampler_binding;
        sampler_binding.binding = 0;
        sampler_binding.descriptorCount = 1;
        sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_binding.pImmutableSamplers = nullptr;
        sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo layout_info;
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 1;
        layout_info.pBindings=&sampler_binding;

        if (vkCreateDescriptorSetLayout(
                vk_device_,
                &layout_info,
                nullptr,
                &descriptor_layout_
                )) {
            throw std::runtime_error("Failed to create Descriptor Set Layout!");
        }

    }

    void InitializePipeline() {
        // shader modules
        VkPipelineShaderStageCreateInfo shader_stage;
        shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage.pNext = nullptr;
        shader_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        
    }

    VkPipeline pipeline_{VK_NULL_HANDLE};
    VkDescriptorSetLayout descriptor_layout_{VK_NULL_HANDLE};

    std::array<VkDescriptorPool, ImagesInFlight> descriptor_pool_;

    const char * shader_path{"/Content/Shaders/WengCore_DrawInSwapChain.slang"};

    VkDevice vk_device_;

};
