#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WDescriptorPoolUtils.hpp"

#include <vulkan/vulkan_core.h>

template<std::uint32_t FramesInFlight,
         typename WPipelineIdType=WAssetId>
class WVkPipelinesDb {
public:

    using WVkPipelineDb = TObjectDataBase<WVkRenderPipelineInfo, void, WPipelineIdType>;
    using WVkDescSetLayoutDb = TObjectDataBase<WVkDescriptorSetLayoutInfo, void, WPipelineIdType>;
    
    // using WVkPipelineBindingDb = TObjectDataBase<WVkPipelineBindingInfo, void, WBindingIdType>;
    
    using WVkDescriptorPoolDb = std::array<
        TObjectDataBase<WVkDescriptorPoolInfo, void, WPipelineIdType>,
        FramesInFlight
        >;
    
    void Clear(const WVkDeviceInfo & in_device) {
        
        for(std::uint32_t i=0; i < FramesInFlight; i++) {
            descriptor_pools[i].Clear(
                [di_=in_device](auto & b) {
                    WDescPoolUtils::Destroy(b, di_);
                }
                );
        }
        
        pipelines.Clear(
            [di_=in_device](auto & p) {
                WVulkan::Destroy(
                    p,
                    di_
                    );
            });

        descriptor_set_layouts.Clear(
            [di_=in_device](auto & d) {
                WVulkan::Destroy(
                    d,
                    di_
                    );
            });

    }

public:

    WVkPipelineDb pipelines{};
    WVkDescSetLayoutDb descriptor_set_layouts{};
    // WVkPipelineBindingDb bindings{};
    WVkDescriptorPoolDb descriptor_pools{};

    std::uint32_t width{0};
    std::uint32_t height{0};

};
