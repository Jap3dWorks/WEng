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

    WVkPipelineResources(const VkDevice & in_device,
                         const VkPhysicalDevice & in_physical_device,
                         const VkQueue & in_graphics_queue,
                         const VkCommandPool & in_command_pool) {
        Initialize(in_device,
                   in_physical_device,
                   in_graphics_queue,
                   in_command_pool);
    }

    ~WVkPipelineResources() {
        Destroy();
    }

    WVkPipelineResources(const WVkPipelineResources &)=delete;

    WVkPipelineResources(WVkPipelineResources && other) noexcept :
        device_(std::move(other.device_)),
        render_plane_(std::move(other.render_plane_)),
        sampler_(std::move(other.sampler_))
        {

            other.device_ = VK_NULL_HANDLE;
            other.render_plane_ = {};
            other.sampler_ = VK_NULL_HANDLE;
        }

    WVkPipelineResources & operator=(const WVkPipelineResources &)=delete;

    WVkPipelineResources & operator=(WVkPipelineResources && other) noexcept {
        if (this != &other) {
            Destroy();

            device_ = std::move(other.device_);
            render_plane_ = std::move(other.render_plane_);
            sampler_ = std::move(other.sampler_);

            other.device_ = VK_NULL_HANDLE;
            other.render_plane_ = {};
            other.sampler_ = VK_NULL_HANDLE;
        }

        return * this;
    }

    const WVkMeshInfo & RenderPlane() const noexcept {
        return render_plane_;
    }

    const VkSampler & Sampler() const noexcept {
        return sampler_;
    }

private:
    
    void Initialize(const VkDevice & in_device,
                    const VkPhysicalDevice & in_physical_device,
                    const VkQueue & in_graphics_queue,
                    const VkCommandPool & in_command_pool) {

        assert(device_ == VK_NULL_HANDLE && "Pipeline resources already initialized!");

        device_ = in_device;

        InitializeRenderPlane(device_,
                              in_physical_device,
                              in_graphics_queue,
                              in_command_pool);

        sampler_ = WVulkanUtils::CreateRenderPlaneSampler(device_);
    }

    void Destroy() {
        if (device_ != VK_NULL_HANDLE) {
            WVulkan::Destroy(render_plane_, device_);
            render_plane_ = {};

            WVulkan::Destroy(sampler_, device_);
            sampler_ = VK_NULL_HANDLE;

            device_ = VK_NULL_HANDLE;
        }
    }

    void InitializeRenderPlane(const VkDevice & in_device,
                               const VkPhysicalDevice & in_physical_device,
                               const VkQueue & in_graphics_queue,
                               const VkCommandPool & in_command_pool) {
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
            in_physical_device,
            in_graphics_queue,
            in_command_pool
            );
    }

private:

    VkDevice device_{VK_NULL_HANDLE};

    WVkMeshInfo render_plane_{};

    VkSampler sampler_{VK_NULL_HANDLE};

};

