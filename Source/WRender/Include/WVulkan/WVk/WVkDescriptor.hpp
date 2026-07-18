#pragma once

#include "WCore/WCoreMacros.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkTypes.hpp"

namespace wvk::descriptor {

    /**
     * @DEPRECATED
     */
    void Create(
        WVkDescriptorSetLayoutInfo& out_descriptor_set_layout_info,
        const VkDevice & device
        );

    WNODISCARD VkDescriptorSetLayout Create(
        VkDescriptorSetLayoutBinding * in_bindings_ptr,
        std::uint32_t in_binding_count,
        VkDevice & in_device
        );

    /**
     * @DEPRECATED
     */
    void Create(
        VkDescriptorPool & out_descriptor_pool_info,
        const VkDevice & device
        );

    /**
     * @DEPRECATED
     */
    void Create(
        VkDescriptorSet& out_descriptor_set_info,
        const VkDevice & device,
        const WVkDescriptorSetLayoutInfo& descriptor_set_layout_info,
        const VkDescriptorPool & descriptor_pool_info
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

    /**
     * @DEPRECATED
     */
    void Destroy(
        WVkDescriptorSetLayoutInfo & descriptor_set_layout_info,
        const VkDevice & device
        );

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

    inline std::vector<VkDescriptorSetLayoutBinding> ToDescriptorSetLayoutBinding(
        const wct::render::RPipeParamDescLayList & in_param_list
        ) {
        std::vector<VkDescriptorSetLayoutBinding> result;

        result.reserve(in_param_list.size());

        wct::render::ForEach(
            in_param_list,
            [&result]
            (const auto& _prm) {
                VkDescriptorSetLayoutBinding bndng{};

                switch(_prm.type) {

                case wct::render::ERPipeParamType::UBO_Entity_Dynamic:
                    bndng.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;                
                    break;

                case wct::render::ERPipeParamType::Texture:
                    bndng.descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    break;
                
                default:
                    return;
                                  
                }

                bndng.binding = _prm.binding;
                bndng.descriptorCount = 1;
                bndng.pImmutableSamplers = nullptr;
                bndng.stageFlags = wvk::types::ToVkShaderStageFlag(_prm.stage_flags);

                result.push_back(bndng);
            }
            );

        return result;
    }

    /**
     * @DEPRECATED
     */
    inline void UpdateDescriptorSetLayout(
        WVkDescriptorSetLayoutInfo & out_dsl,
        const wct::render::RPipeParamDescLayList & in_param_list
        ) {

        out_dsl.bindings = ToDescriptorSetLayoutBinding(in_param_list);

    }

    constexpr void UpdateWriteDescriptorSet_UBO(
        VkWriteDescriptorSet & out_write_descriptor_set,
        uint32_t binding,
        VkDescriptorBufferInfo * buffer_info,
        VkDescriptorSet dst_set
        )
    {
        out_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        out_write_descriptor_set.dstBinding = binding;
        out_write_descriptor_set.dstSet = dst_set;
        out_write_descriptor_set.dstArrayElement = 0;
        out_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        out_write_descriptor_set.descriptorCount = 1;
        out_write_descriptor_set.pBufferInfo = buffer_info;
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
