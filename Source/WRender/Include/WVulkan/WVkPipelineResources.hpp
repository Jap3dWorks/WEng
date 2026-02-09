#pragma once

#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkanUtils.hpp"
#include "WVulkan/WVulkan.hpp"
#include <vulkan/vulkan_core.h>

#include <cassert>

/**
 * Contains common render pipelines resources
 */
class WVkPipelineResources {

public:

    WVkPipelineResources()=default;

    virtual ~WVkPipelineResources()=default;

    WVkPipelineResources(const WVkPipelineResources &)=delete;

    // TODO
    WVkPipelineResources(WVkPipelineResources &&)=default;

    WVkPipelineResources & operator=(const WVkPipelineResources &)=delete;

    // TODO
    WVkPipelineResources & operator=(WVkPipelineResources &&)=default;

    void Initialize(const WVkDeviceInfo & in_device_info,
                    const WVkCommandPoolInfo & in_command_pool) {

        assert(device_info_.vk_device == VK_NULL_HANDLE && "Pipeline resources already initialized!");

        device_info_ = in_device_info;

        InitializeRenderPlane(device_info_, in_command_pool);

        sampler_ = WVulkanUtils::CreateRenderPlaneSampler(device_info_.vk_device);

    }

    void Destroy() {
        assert(device_info_.vk_device != VK_NULL_HANDLE && "Pipeline resources not initialized!");

        WVulkan::Destroy(render_plane_, device_info_);
        render_plane_ = {};

        WVulkan::Destroy(sampler_, device_info_);
        sampler_ = VK_NULL_HANDLE;

        device_info_ = {};
    }

    const WVkMeshInfo & RenderPlane() const noexcept {
        return render_plane_;
    }

    const VkSampler & Sampler() const noexcept {
        return sampler_;
    }

private:

    void InitializeRenderPlane(const WVkDeviceInfo & in_device,
                               const WVkCommandPoolInfo & in_command_pool) {
        auto plane_vertex = WVulkanUtils::RenderPlaneVertex();
        auto plane_index = WVulkanUtils::RenderPlaneIndexes();

        WVulkan::CreateMeshBuffers(
            render_plane_,
            plane_vertex.data(),
            sizeof(decltype(plane_vertex)::value_type) * plane_vertex.size(),
            plane_index.data(),
            sizeof(decltype(plane_vertex)::value_type) * plane_index.size(),
            plane_index.size(),
            in_device,
            in_command_pool
            );
    }

private:

    WVkMeshInfo render_plane_{};

    VkSampler sampler_{VK_NULL_HANDLE};

    WVkDeviceInfo device_info_{};
};
