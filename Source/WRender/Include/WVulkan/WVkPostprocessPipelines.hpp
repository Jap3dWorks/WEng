#pragma once

#include "WCore/TIterator.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkanStructs.hpp"
#include "WVulkan/WVkPipelinesBase.hpp"
#include <vulkan/vulkan_core.h>

class WVkPostprocessPipelines :
    public WVkPipelinesBase<WAssetId,
                            WEntityComponentId,
                            WENG_MAX_FRAMES_IN_FLIGHT>
{
public:
    using Super = WVkPipelinesBase<WAssetId, WEntityComponentId, frames_in_flight>;

    struct GlobalPostprocessResources {
        std::array<WVkDescriptorPoolInfo, frames_in_flight> descpool_info{};
        WVkDescriptorSetLayoutInfo descset_layout_info{};
        
        // std::array<WVkDescriptorSetInfo, frames_in_flight> descset_info{};

        WVkMeshInfo render_plane{};
        VkSampler render_sampler{};
    };

public:

    void Initialize(const WVkDeviceInfo & in_device, const WVkCommandPoolInfo & in_command_pool);

    void CreatePipeline(const WAssetId & in_id,
                        const WRenderPipelineStruct & in_pipeline_struct);

    WEntityComponentId CreateBinding(const WEntityComponentId & in_binding_id,
                                     const WAssetId & in_pipeline_id,
                                     std::vector<WVkTextureInfo> in_textures,
                                     std::vector<std::uint16_t> in_texture_bindings);

    const WVkMeshInfo RenderPlane() const {
        return global_resources_.render_plane;
    }

    const WVkDescriptorPoolInfo & GlobalDescriptorPool(const std::uint32_t & in_frame_index) const {
        return global_resources_.descpool_info[in_frame_index];
    }

    const WVkDescriptorSetLayoutInfo & GlobalDescSetLayout() const {
        return global_resources_.descset_layout_info;
    }

    const VkSampler & GlobalSampler() const {
        return global_resources_.render_sampler;
    }

    void ResetGlobalDescriptorPool(const std::uint32_t & in_frame_index) {
        vkResetDescriptorPool(device_info_.vk_device,
                              global_resources_.descpool_info[in_frame_index].descriptor_pool,
                              {});
    }

    void Destroy();

    void CalcBindingOrder();

    auto IterBindingOrder() const {
        return WIteratorUtils::DefaultIteratorPtr<const WEntityComponentId>(
            &(*binding_order_.begin()),
            &(*binding_order_.end())
            );
    }

private:

    void Initialize(const WVkDeviceInfo & in_device) override {}

    void Initialize_GlobalResources(const WVkCommandPoolInfo & in_command_pool);

    void Destroy_GlobalResources();

    std::vector<WEntityComponentId> binding_order_{};

    GlobalPostprocessResources global_resources_{};

};


