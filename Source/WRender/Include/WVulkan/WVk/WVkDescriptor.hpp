#pragma once

#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkTypes.hpp"

namespace wvk::descriptor {

    void Create(
        WVkDescriptorSetLayoutInfo& out_descriptor_set_layout_info,
        const VkDevice & device
        );
    
    void Create(
        VkDescriptorPool & out_descriptor_pool_info,
        const VkDevice & device
        );

    void Create(
        VkDescriptorSet& out_descriptor_set_info,
        const VkDevice & device,
        const WVkDescriptorSetLayoutInfo& descriptor_set_layout_info,
        const VkDescriptorPool & descriptor_pool_info
        );

    void Destroy(
        WVkDescriptorSetLayoutInfo & descriptor_set_layout_info,
        const VkDevice & device
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

    inline void UpdateDescriptorSetLayout(
        WVkDescriptorSetLayoutInfo & out_dsl,
        const wct::render::WPipeParamDescriptorList & in_param_list
        ) {

        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.reserve(in_param_list.size());

        wct::render::ForEach(
            in_param_list,
            [&bindings]
            (const auto& _prm) {
                VkDescriptorSetLayoutBinding bndng{};

                switch(_prm.type) {

                case wct::render::EPipeParamType::Ubo:
                    bndng.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;                
                    break;

                case wct::render::EPipeParamType::Texture:
                    bndng.descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    break;
                
                default:
                    return;
                                  
                }

                bndng.binding = _prm.binding;
                bndng.descriptorCount = 1;
                bndng.pImmutableSamplers = nullptr;
                bndng.stageFlags = wvk::types::ToVkShaderStageFlag(_prm.stage_flags);

                bindings.push_back(bndng);
            }
            );

        out_dsl.bindings = bindings;

    }

    constexpr void UpdateWriteDescriptorSet_UBO(
        VkWriteDescriptorSet & out_write_descriptor_set,
        const uint32_t & in_binding,
        const VkDescriptorBufferInfo & in_buffer_info,
        const VkDescriptorSet & dst_set
        )
    {
        out_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        out_write_descriptor_set.dstBinding = in_binding;
        out_write_descriptor_set.dstSet = dst_set;
        out_write_descriptor_set.dstArrayElement = 0;
        out_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        out_write_descriptor_set.descriptorCount = 1;
        out_write_descriptor_set.pBufferInfo = &in_buffer_info;
        out_write_descriptor_set.pImageInfo = VK_NULL_HANDLE;
        out_write_descriptor_set.pNext = VK_NULL_HANDLE;
    }

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
