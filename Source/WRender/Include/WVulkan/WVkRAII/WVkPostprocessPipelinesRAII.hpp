#pragma once

#include "WCore/TIterator.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVkPipelinesBase.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include <vulkan/vulkan_core.h>

// TODO template with max frames in flight as param

class WVkPostprocessPipelinesRAII :
    public WVkPipelinesBase<wcr::wid::WAssetId,
                            wcr::wid::WEntityComponentId,
                            WENG_MAX_FRAMES_IN_FLIGHT>
{
public:
    using Super = WVkPipelinesBase<wcr::wid::WAssetId, wcr::wid::WEntityComponentId, frames_in_flight>;

public:

    WVkPostprocessPipelinesRAII()=default;

    WVkPostprocessPipelinesRAII(const VkDevice & in_device,
                            const VkPhysicalDevice & in_physical_device);

    virtual ~WVkPostprocessPipelinesRAII()=default; 

    WVkPostprocessPipelinesRAII(const WVkPostprocessPipelinesRAII &)=delete;
    WVkPostprocessPipelinesRAII & operator=(const WVkPostprocessPipelinesRAII &)=delete;

    WVkPostprocessPipelinesRAII(WVkPostprocessPipelinesRAII && other) noexcept;

    WVkPostprocessPipelinesRAII & operator=(WVkPostprocessPipelinesRAII && other) noexcept;

    void CreatePipeline(
        wcr::wid::WAssetId in_id,
        const WRenderPipelineAsset & in_pipeline_struct,
        VkDescriptorSetLayout in_global_descriptor,
        VkDescriptorSetLayout in_ppcess_global_descriptor
        );

    wcr::wid::WEntityComponentId CreateBinding(const wcr::wid::WEntityComponentId & in_binding_id,
                                     const wcr::wid::WAssetId & in_pipeline_id,
                                     const std::vector<WVkDescriptorSetUBOWriteStruct> & in_ubos,
                                     const std::vector<WVkDescriptorSetTextureBinding> & in_texture);

    /**
     * @Brief Computes pipeline bindings order, order is relevant because 
     * in a postprocess stack each postprocess is applied over the previous postprocess.
     * This function is required to be called at least once.
     */
    void ComputeBindingOrder();

    auto BindingOrderIterator() const {
        return WIteratorUtils::DefaultIteratorPtr<const wcr::wid::WEntityComponentId>(
            &(*binding_order_.begin()),
            &(*binding_order_.end())
            );
    }

private:

    std::vector<wcr::wid::WEntityComponentId> binding_order_{};

};


