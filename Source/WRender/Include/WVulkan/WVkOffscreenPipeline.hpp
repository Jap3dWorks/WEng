#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkanUtils.hpp"
#include "WVulkan/WVulkan.hpp"
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
    };

public:

    WVkOffscreenPipeline() noexcept = default;

public:

    void Initialize(const WVkDeviceInfo & in_device,
                    const WVkCommandPoolInfo & in_command_pool) {
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
            // WVkPostprocessPipeUtils::CreateGlobalResourcesDescPool(
            //     descpool,
            //     device_info_
            //     );
        }

        global_resources_.render_sampler = WVulkanUtils::CreateRenderPlaneSampler(
            in_device.vk_device
            );
    }

    void Destroy();

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

    void Destroy_GlobalResources();

    void UpdateDSL(WVkDescriptorSetLayoutInfo & out_descset_lay) {
        VkDescriptorSetLayoutBinding albedo_binding{};
        albedo_binding.binding=0;
        albedo_binding.descriptorCount=1;
        albedo_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        albedo_binding.pImmutableSamplers = nullptr;
        albedo_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding normal_binding{};

        VkDescriptorSetLayoutBinding ws_position_binding{};

        VkDescriptorSetLayoutBinding depth_binding{};

        out_descset_lay.bindings = {
            albedo_binding,
            normal_binding,
            ws_position_binding,
            depth_binding
        };
    }

    GlobalOffscreenResources global_resources_{};
    
};

