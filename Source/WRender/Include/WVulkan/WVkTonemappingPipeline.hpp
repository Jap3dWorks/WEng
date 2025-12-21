#pragma once

#include "WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"

#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>


template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkTonemappingPipeline {

public:

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

    void InitializeRenderPipeline() {
        VkFormat from_format = VK_FORMAT_R16G16B16_SFLOAT;
        VkFormat to_format = VK_FORMAT_B8G8R8A8_SRGB;

        // TODO ...

    }

private:

    VkPipeline pipeline_{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
    
    VkDescriptorSetLayout descset_layout_{VK_NULL_HANDLE};

    std::array<VkDescriptorPool, FramesInFlight> descriptor_pool_{};
    VkSampler input_render_sampler_{VK_NULL_HANDLE};

    // TODO: common resource
    // WVkMeshInfo render_plane_{};

    const char* shader_path{WENG_VK_TONEMAPPING_SHADER_PATH};

    WVkDeviceInfo device_info_{};

};
