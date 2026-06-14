#pragma once

#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkTypes.hpp"

namespace wvk::descriptor {

    void Create(
        WVkDescriptorSetLayoutInfo& out_descriptor_set_layout_info,
        const VkDevice & device
        );
    
    inline void UpdateDescriptorSetLayout(
        WVkDescriptorSetLayoutInfo & out_dsl,
        const WPipeParamDescriptorList & in_param_list
        ) {

        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.reserve(in_param_list.size());

        WRenderUtils::ForEach(
            in_param_list,
            [&bindings]
            (const auto& _prm) {
                VkDescriptorSetLayoutBinding bndng{};

                switch(_prm.type) {

                case EPipeParamType::Ubo:
                    bndng.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;                
                    break;

                case EPipeParamType::Texture:
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
}
