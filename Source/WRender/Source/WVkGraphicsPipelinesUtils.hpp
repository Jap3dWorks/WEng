#pragma once

#include "WCore/WCore.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WStructs/WGeometryStructs.hpp"
#include "WShaderUtils.hpp"
#include <stdexcept>
#include <array>

namespace WVkGraphicsPipelinesUtils {
    
    inline void CreateDescSetPool(
        WVkDescriptorPoolInfo & out_descriptor_pool_info,
        const WVkDeviceInfo & device
        ) {
        
        std::array<VkDescriptorPoolSize, 2> pool_sizes;

        pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount = 20;

        pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[1].descriptorCount = 40;

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes = pool_sizes.data();
        
        pool_info.maxSets = 60;

        if (vkCreateDescriptorPool(
                device.vk_device,
                &pool_info,
                nullptr,
                &out_descriptor_pool_info.descriptor_pool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
    }

    inline WVkShaderStageInfo BuildShaderStageInfo(
        const char * in_shader_file_path,
        const char * in_entry_point,
        EShaderType in_shader_type
        ) {
        WVkShaderStageInfo result;

        result.code = WShaderUtils::ReadShader(in_shader_file_path);

        result.entry_point = in_entry_point;
        result.type = in_shader_type;

        if (in_shader_type == EShaderType::Vertex)
        {
            result.attribute_descriptors.resize(3);

            // Geometry data structure

            result.attribute_descriptors[0].binding = 0;
            result.attribute_descriptors[0].location = 0;
            result.attribute_descriptors[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            result.attribute_descriptors[0].offset = offsetof(WVertexStruct, Position);

            result.attribute_descriptors[1].binding = 0;
            result.attribute_descriptors[1].location = 1;
            result.attribute_descriptors[1].format = VK_FORMAT_R32G32_SFLOAT;
            result.attribute_descriptors[1].offset = offsetof(WVertexStruct, TexCoords);

            result.attribute_descriptors[2].binding = 0;
            result.attribute_descriptors[2].location = 2;
            result.attribute_descriptors[2].format = VK_FORMAT_R32G32B32_SFLOAT;
            result.attribute_descriptors[2].offset = offsetof(WVertexStruct, Color);

            // TODO Normals

            // more vertex data bindings here

            result.binding_descriptors.resize(1);
            result.binding_descriptors[0].binding = 0;
            result.binding_descriptors[0].stride = sizeof(WVertexStruct);
            result.binding_descriptors[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        }

        return result;
    }

    /**
     * @brief Adds common graphic pipeline bindings, should be used in set=1
     */
    inline void UpdateDSL_DefaultGraphicBindings(WVkDescriptorSetLayoutInfo & out_dsl) {
        // Model UBO
        VkDescriptorSetLayoutBinding ubo_binding{};
        ubo_binding.binding = 0;
        ubo_binding.descriptorCount = 1;
        ubo_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ubo_binding.pImmutableSamplers = nullptr;
        ubo_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        // A texture in the fragment shader
        VkDescriptorSetLayoutBinding sampler_binding{};
        sampler_binding.binding = 1;
        sampler_binding.descriptorCount = 1;
        sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_binding.pImmutableSamplers = nullptr;
        sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        // TODO: Other texture bindings (Normal, MRExtr)

        out_dsl.bindings = {
            ubo_binding,
            sampler_binding
        };
    }

    /**
     * @brief Adds the Camera and lightings UBOs (std140, set=0)
     */
    inline void UpdateDSL_DefaultGlobalGraphicBindings(WVkDescriptorSetLayoutInfo & out_dsl) {
        VkDescriptorSetLayoutBinding camera_binding{};
        camera_binding.binding=0;
        camera_binding.descriptorCount = 1;
        camera_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        camera_binding.pImmutableSamplers = nullptr;
        camera_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        // TODO lights here as another ubo

        out_dsl.bindings = {
            camera_binding
        };
    }

}
