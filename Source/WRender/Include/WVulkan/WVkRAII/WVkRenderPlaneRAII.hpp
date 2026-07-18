#pragma once

#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkTypes.hpp"
#include "WVulkan/WVk/WVkMesh.hpp"
#include "WVulkan/WVk/WVkTexture.hpp"
// #include "WVulkan/WVk/WVulkan.hpp"
#include "WVulkan/WVk/WVkRenderPlane.hpp"
#include <vulkan/vulkan_core.h>

#include <cassert>

/**
 * Contains common render pipelines resources
 */
class WVkRenderPlaneRAII {

public:

    WVkRenderPlaneRAII()=default;

    WVkRenderPlaneRAII(const VkDevice & in_device,
                             const VkPhysicalDevice & in_physical_device,
                             const VkQueue & in_graphics_queue,
                             const VkCommandPool & in_command_pool) {
        Initialize(in_device,
                   in_physical_device,
                   in_graphics_queue,
                   in_command_pool);
    }

    ~WVkRenderPlaneRAII() {
        Destroy();
    }

    WVkRenderPlaneRAII(const WVkRenderPlaneRAII &)=delete;

    WVkRenderPlaneRAII(WVkRenderPlaneRAII && other) noexcept :
        device_(std::move(other.device_)),
        render_plane_(std::move(other.render_plane_)),
        sampler_(std::move(other.sampler_))
        {

            other.device_ = VK_NULL_HANDLE;
            other.render_plane_ = {};
            other.sampler_ = VK_NULL_HANDLE;
        }

    WVkRenderPlaneRAII & operator=(const WVkRenderPlaneRAII &)=delete;

    WVkRenderPlaneRAII & operator=(WVkRenderPlaneRAII && other) noexcept {
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

    const WVkMesh & RenderPlane() const noexcept {
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

        sampler_ = wvk::render_plane::CreateRenderPlaneSampler(device_);
    }

    void Destroy() {
        if (device_ != VK_NULL_HANDLE) {
            wvk::mesh::Destroy(render_plane_, device_);
            render_plane_ = {};

            wvk::texture::DestroyVkSampler(sampler_, device_);
            sampler_ = VK_NULL_HANDLE;

            device_ = VK_NULL_HANDLE;
        }
    }

    void InitializeRenderPlane(const VkDevice & in_device,
                               const VkPhysicalDevice & in_physical_device,
                               const VkQueue & in_graphics_queue,
                               const VkCommandPool & in_command_pool) {
        auto plane_vertex = wvk::render_plane::RenderPlaneVertex();
        auto plane_index = wvk::render_plane::RenderPlaneIndexes();

        wvk::mesh::CreateMeshBuffers(
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

    WVkMesh render_plane_{};

    VkSampler sampler_{VK_NULL_HANDLE};

};

