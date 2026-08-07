#pragma once

#include "WCore/WCoreMacros.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"
#include <vulkan/vulkan_core.h>

namespace wvk::descriptor {

    // [[deprecated]] void Create(
    //     WVkDescriptorSetLayoutInfo& out_descriptor_set_layout_info,
    //     const VkDevice & device
    //     );

    [[deprecated]] void Create(
        VkDescriptorPool & out_descriptor_pool_info,
        const VkDevice & device
        );

    // [[deprecated]] void Create(
    //     VkDescriptorSet& out_descriptor_set_info,
    //     const VkDevice & device,
    //     const WVkDescriptorSetLayoutInfo& descriptor_set_layout_info,
    //     const VkDescriptorPool & descriptor_pool_info
    //     );

    WNODISCARD VkDescriptorSetLayout CreateDescriptorSetLayout(
        std::span<VkDescriptorSetLayoutBinding> layout_bindings,
        // VkDescriptorSetLayoutBinding * in_bindings_ptr,
        // std::uint32_t in_binding_count,
        VkDevice & in_device
        );

    template<std::uint8_t N>
    VkDescriptorPool CreateDescriptorPool(
        VkDevice in_device,
        const std::array<VkDescriptorPoolSize,N> pool_sizes,
        std::uint32_t max_sets
        ) {
        
        VkDescriptorPool result;
        VkDescriptorPoolCreateInfo pool_info{};
        
        pool_info = wvk::types::VkDescriptorPoolCreateInfo();
        
        pool_info.poolSizeCount = static_cast<std::uint32_t>(
            pool_sizes.size()
            );
        pool_info.pPoolSizes = pool_sizes.data();
        pool_info.maxSets = max_sets;

        wvk::vulkan::ExecVkProcChecked(
            vkCreateDescriptorPool,
            "Failed to create descriptor pool!",
            in_device,
            &pool_info,
            nullptr,
            &result
            );

        return result;
    }

    WNODISCARD VkDescriptorSet CreateDescriptor(
        VkDevice device,
        VkDescriptorSetLayout descriptor_set_layout_info,
        VkDescriptorPool descriptor_pool_info
        );

    // [[deprecated]] void Destroy(
    //     WVkDescriptorSetLayoutInfo & descriptor_set_layout_info,
    //     const VkDevice & device
    //     );

    void Destroy(
        VkDescriptorSetLayout in_descriptor_set_layout,
        VkDevice in_device
        );

    void Destroy(
        VkDescriptorPool & out_descriptor_pool_info,
        const VkDevice & in_device
        );

    template<std::size_t N>
    void DestroyDescPools(std::array<VkDescriptorPool, N> & out_desc_pools,
                          const VkDevice & in_device) {
        for (std::uint32_t i=0; i<N; i++) {
            if(out_desc_pools[i]) {
                vkDestroyDescriptorPool(
                    in_device,
                    out_desc_pools[i],
                    nullptr
                    );
            }

            out_desc_pools[i] = VK_NULL_HANDLE;
        }
    }

    inline constexpr
    std::optional<VkDescriptorType> ToDescriptorType(
        wct::render::ERPipeParamType param_type,
        VkDescriptorType vk_texture_type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        VkDescriptorType vk_ubo_type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
        ) noexcept {
        switch(param_type) {

        case wct::render::ERPipeParamType::None:
            return std::nullopt;

        case wct::render::ERPipeParamType::Texture:
            return vk_texture_type;

        default:
            return vk_ubo_type;
        }
    }

    inline std::vector<VkDescriptorSetLayoutBinding> ToDescriptorSetLayoutBinding(
        const wct::render::RPipeParamDescriptorsLayout & in_param_list,
        VkDescriptorType vk_texture_type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        VkDescriptorType vk_ubo_type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
        ) {
        std::vector<VkDescriptorSetLayoutBinding> result;

        result.reserve(in_param_list.size());

        auto to_descriptor_type =
            [vk_ubo_type, vk_texture_type](wct::render::ERPipeParamType param_type)
                {
                    return ToDescriptorType(param_type,
                                            vk_texture_type,
                                            vk_ubo_type);
                };

        wct::render::ForEach(
            in_param_list,
            [&result, &to_descriptor_type]
            (const auto& _prm) {
                VkDescriptorSetLayoutBinding bndng{};

                auto descriptor_type = to_descriptor_type(_prm.type);
                if (!descriptor_type) return;

                bndng.descriptorType=*descriptor_type;
                bndng.binding = _prm.binding;
                bndng.descriptorCount = 1;
                bndng.pImmutableSamplers = nullptr;
                bndng.stageFlags = wvk::types::ToVkShaderStageFlag(_prm.stage_flags);

                result.push_back(bndng);
            }
            );

        return result;
    }

    // inline void UpdateDescriptorSetLayout(
    //     WVkDescriptorSetLayoutInfo & out_dsl,
    //     const wct::render::RPipeParamDescriptorsLayout & in_param_list
    //     ) {

    //     out_dsl.bindings = ToDescriptorSetLayoutBinding(in_param_list);

    // }

    constexpr void UpdateWriteDescriptorSet_Texture(
        VkWriteDescriptorSet & out_write_descriptor_set,
        const uint32_t & in_binding,
        const VkDescriptorImageInfo & in_image_info,
        const VkDescriptorSet & dst_set
        )
    {
        out_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        out_write_descriptor_set.dstBinding = in_binding;
        out_write_descriptor_set.dstSet = dst_set;
        out_write_descriptor_set.dstArrayElement = 0;
        out_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        out_write_descriptor_set.descriptorCount = 1;
        out_write_descriptor_set.pImageInfo = &in_image_info;
        out_write_descriptor_set.pBufferInfo = VK_NULL_HANDLE;
        out_write_descriptor_set.pNext = VK_NULL_HANDLE;
    }

}
