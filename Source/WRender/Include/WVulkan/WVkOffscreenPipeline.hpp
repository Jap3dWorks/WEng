#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkanUtils.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WShaderUtils.hpp"
#include "WCore/WStringUtils.hpp"
#include <vulkan/vulkan_core.h>

/**
 * @brief Offsccreen pipeline outputs the render using GBuffers.
 */
template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkOffscreenPipeline {

public:

    struct GlobalOffscreenResources {
        std::array<WVkDescriptorPoolInfo, FramesInFlight> descpool_info{};
        WVkDescriptorSetLayoutInfo descset_layout_info{};
        WVkMeshInfo render_plane{};
        VkSampler render_sampler{};
        
        WVkRenderPipelineInfo render_pipeline{};
    };

public:

    WVkOffscreenPipeline() noexcept = default;

public:

    void Initialize(const WVkDeviceInfo & in_device,
                    const WVkCommandPoolInfo & in_command_pool) {
        device_info_ = in_device;

        auto plane_vertex = WVulkanUtils::RenderPlaneVertex();
        auto plane_index = WVulkanUtils::RenderPlaneIndexes();

        WVulkan::CreateMeshBuffers(
            global_resources_.render_plane,
            WVulkanUtils::RenderPlaneVertex().data(),
            sizeof(decltype(plane_vertex)::value_type) * plane_vertex.size(),
            WVulkanUtils::RenderPlaneIndexes().data(),
            sizeof(decltype(plane_index)::value_type) * plane_index.size(),
            plane_index.size(),
            in_device,
            in_command_pool
            );

        global_resources_.descset_layout_info = {};
        UpdateDSL(
            global_resources_.descset_layout_info
            );

        WVulkan::Create(
            global_resources_.descset_layout_info,
            in_device
            );

        for(auto & descpool : global_resources_.descpool_info) {
            descpool = {};
            InitializeDescPool(
                descpool,
                in_device
                );
        }

        global_resources_.render_sampler = WVulkanUtils::CreateRenderPlaneSampler(
            in_device.vk_device
            );
    }

    void Destroy() {
        if (device_info_.vk_device != VK_NULL_HANDLE) {

            WVulkan::Destroy(
                global_resources_.render_plane,
                device_info_
                );

            WVulkan::Destroy(
                global_resources_.descset_layout_info,
                device_info_
                );

            for(auto & descpool : global_resources_.descpool_info) {
                WVulkan::Destroy(descpool, device_info_);
            }

            WVulkan::Destroy(
                global_resources_.render_sampler,
                device_info_
                );

            device_info_ = {};
        }
    }

    const WVkMeshInfo & RenderPlane() const noexcept{
        return global_resources_.render_plane;
    }

    const WVkDescriptorPoolInfo & GlobalDescriptorPool(const std::uint32_t & in_frame_index) const noexcept {
        return global_resources_.descpool_info[in_frame_index];
    }

    const WVkDescriptorSetLayoutInfo & GlobalDescSetLayout() const noexcept {
        return global_resources_.render_sampler;
    }

    const VkSampler & GlobalSampler() const noexcept {
        return global_resources_.render_sampler;
    }

private:

    GlobalOffscreenResources global_resources_{};

    WVkDeviceInfo device_info_{};

    void Destroy_GlobalResources();

    void UpdateDSL(WVkDescriptorSetLayoutInfo & out_descset_lay) {
        VkDescriptorSetLayoutBinding albedo_binding{};
        albedo_binding.binding=0;
        albedo_binding.descriptorCount=1;
        albedo_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        albedo_binding.pImmutableSamplers = nullptr;
        albedo_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding normal_binding{};
        normal_binding.binding=0;
        normal_binding.descriptorCount=1;
        normal_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        normal_binding.pImmutableSamplers = nullptr;
        normal_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding ws_position_binding{};
        ws_position_binding.binding=0;
        ws_position_binding.descriptorCount=1;
        ws_position_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        ws_position_binding.pImmutableSamplers = nullptr;
        ws_position_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        // extras ...

        VkDescriptorSetLayoutBinding depth_binding{};
        depth_binding.binding=0;
        depth_binding.descriptorCount=1;
        depth_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        depth_binding.pImmutableSamplers = nullptr;
        depth_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        out_descset_lay.bindings = {
            albedo_binding,
            normal_binding,
            ws_position_binding,
            // extras ..
            depth_binding
        };
    }

    void InitializeDescPool(
        WVkDescriptorPoolInfo & out_descriptor_pool_info,
        const WVkDeviceInfo & in_device
        ) {
        std::array<VkDescriptorPoolSize, 2> pool_sizes;

        pool_sizes[0]={};
        pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount = 32; // bytes reserved for descriptors of this type
        // TODO check other uniform buffers descriptorCount
        
        pool_sizes[1]={};
        pool_sizes[1].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[1].descriptorCount = 10;

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes = pool_sizes.data();
        pool_info.maxSets = 32;

        if (vkCreateDescriptorPool(
                in_device.vk_device,
                &pool_info,
                nullptr,
                &out_descriptor_pool_info.descriptor_pool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
    }

    void InitializeRenderPipeline(
        WVkRenderPipelineInfo & render_pipeline,
        const WVkDeviceInfo & in_device
        ) {
        std::vector<char> shadercode = WShaderUtils::ReadShader(
            WStringUtils::SystemPath(shader_path)
            );

        VkShaderModule shader_module = WVulkanUtils::CreateShaderModule(
            in_device.vk_device,
            shadercode.data(),
            shadercode.size()
            );

        std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages;
        
        WVulkanUtils::UpdateVertexFragmentVkPipelineShaderStageCreateInfo(
            shader_stages.data(),
            shader_module
            );

    }

    const char * shader_path{"Content/Shaders/WRender_Offscreen.spv"}; // TODO Config file

};

