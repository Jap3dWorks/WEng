#pragma once

#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVulkan.hpp"

#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <vulkan/vulkan_core.h>
#include <array>
#include <stdexcept>
#include <vector>

// WVkWengUtils.hpp

// TODO refactor namespaces wvk::texture:: wvk::image:: wvk::vulkan:: etc
//  namespaces to sneaky_case
namespace WVkWengUtils {

    // TODO This file should contain more WEng based vulkan functions and procedures

    /**
     * @brief Enum representing the shader stage flags for Vulkan.
     */
    inline VkShaderStageFlagBits ToShaderStageFlagBits(const EShaderStageFlag & type) {
        switch (type)
        {
        case EShaderStageFlag::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case EShaderStageFlag::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case EShaderStageFlag::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            throw std::runtime_error("Invalid shader type!");
        }
    }

    inline VkShaderStageFlags ToVkShaderStageFlag(const EShaderStageFlag & in_pipe_flags) {

        VkShaderStageFlags result{0};
        for(const auto & f : WRenderUtils::SHADER_STAGE_FLAGS_LIST) {
            
            if ((f & in_pipe_flags) == EShaderStageFlag::None)
                continue; 

            VkShaderStageFlags n{0};
            
            switch(f) {
            case EShaderStageFlag::Vertex:
                n = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case EShaderStageFlag::Fragment:
                n = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case EShaderStageFlag::Compute:
                n = VK_SHADER_STAGE_COMPUTE_BIT;
                break;
            default:
                continue;
            }

            result |= n;
        }

        return result;

    }

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
                bndng.stageFlags = ToVkShaderStageFlag(_prm.stage_flags);

                bindings.push_back(bndng);
            }
            );

        out_dsl.bindings = bindings;

    }


}
