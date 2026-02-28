#pragma once

#include "WCore/TIterator.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
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
    };

public:

    void Initialize(const VkDevice & in_device,
                    const VkPhysicalDevice & in_physical_device) override;

    void CreatePipeline(const WAssetId & in_id,
                        const WRenderPipelineStruct & in_pipeline_struct);

    WEntityComponentId CreateBinding(const WEntityComponentId & in_binding_id,
                                     const WAssetId & in_pipeline_id,
                                     const std::vector<WVkDescriptorSetUBOWriteStruct> & in_ubos,
                                     const std::vector<WVkDescriptorSetTextureWriteStruct> & in_texture);

    const WVkDescriptorPoolInfo & GlobalDescriptorPool(const std::uint32_t & in_frame_index) const {
        return global_resources_.descpool_info[in_frame_index];
    }

    const WVkDescriptorSetLayoutInfo & GlobalDescSetLayout() const {
        return global_resources_.descset_layout_info;
    }

    void ResetGlobalDescriptorPool(const std::uint32_t & in_frame_index) {
        vkResetDescriptorPool(device_,
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

    // void Initialize(const VkDevice & in_device,
    //                 const VkPhysicalDevice & in_physical_device) override {}

    void Initialize_GlobalResources();

    void Destroy_GlobalResources();

    std::vector<WEntityComponentId> binding_order_{};

    GlobalPostprocessResources global_resources_{};

};


