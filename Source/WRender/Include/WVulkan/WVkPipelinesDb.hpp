#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WDescriptorPoolUtils.hpp"
#include "WCore/WConcepts.hpp"
#include "WCore/WStringUtils.hpp"

#include <utility>
#include <vulkan/vulkan_core.h>

template<std::uint32_t FramesInFlight,
         typename WPipelineIdType=WAssetId>
class WVkPipelinesDb {
public:

    using WVkPipelineDb = TObjectDataBase<WVkRenderPipelineInfo, void, WPipelineIdType>;
    using WVkDescSetLayoutDb = TObjectDataBase<WVkDescriptorSetLayoutInfo, void, WPipelineIdType>;
    using WVkDescriptorPoolDb = std::array<
        TObjectDataBase<WVkDescriptorPoolInfo, void, WPipelineIdType>,
        FramesInFlight
        >;

public:

    WVkPipelinesDb() noexcept = default;

    virtual ~WVkPipelinesDb() = default;

    WVkPipelinesDb(const WVkPipelinesDb &) = delete;

    WVkPipelinesDb(WVkPipelinesDb &&) noexcept = default;

    WVkPipelinesDb & operator=(const WVkPipelinesDb &) = delete;

    WVkPipelinesDb & operator=(WVkPipelinesDb &&) noexcept = default;

    template<CCallable<void, WVkRenderPipelineInfo&, const WVkDescriptorSetLayoutInfo &> CreateFn>
    void CreatePipeline(const WVkDeviceInfo & in_device_info,
                        const WPipelineIdType & in_id,
                        const WRenderPipelineStruct & pipeline_struct,
                        const WVkDescriptorSetLayoutInfo & descriptor_set_layout,
                        CreateFn && create_fn) {
        std::vector<WVkShaderStageInfo> shaders;
        shaders.reserve(pipeline_struct.shaders_count);

        // TODO transition to slang
        for (uint8_t i=0; i < pipeline_struct.shaders_count; i++) {
            shaders.push_back(
                WVulkan::BuildGraphicsShaderStageInfo(
                    WStringUtils::SystemPath(pipeline_struct.shaders[i].file).c_str(),
                    pipeline_struct.shaders[i].entry,
                    pipeline_struct.shaders[i].type
                    )
                );
        }

        WVkRenderPipelineInfo render_pipeline_info;
        render_pipeline_info.type = pipeline_struct.type;

        create_fn(render_pipeline_info, descriptor_set_layout);

        pipelines.InsertAt(in_id, render_pipeline_info);

        render_pipeline_info.wid = in_id;
        render_pipeline_info.descriptor_set_layout_id = in_id;
    }

    template<CCallable<void, WVkDescriptorSetLayoutInfo&> ConfigInfoFn>
    void CreateDescriptorSetLayout(const WVkDeviceInfo & in_device,
                                   const WPipelineIdType & in_id,
                                   ConfigInfoFn && info_fn) {
        WVkDescriptorSetLayoutInfo descriptor_set_layout_info;

        std::forward<ConfigInfoFn>(info_fn)(descriptor_set_layout_info);

        descriptor_set_layouts.CreateAt(
            in_id,
            [this, &descriptor_set_layout_info, &in_device]
            (const WPipelineIdType & _in_id) -> auto {
                WVulkan::Create(
                    descriptor_set_layout_info,
                    in_device
                    );
                descriptor_set_layout_info.wid = _in_id;
                return descriptor_set_layout_info;
            });
    }

    template<CCallable<void, WVkDescriptorPoolInfo & /* out */, const WVkDeviceInfo&> TCreateFn>
    void CreateDescriptorPool(const WVkDeviceInfo & in_device,
                              const WPipelineIdType & in_id,
                              TCreateFn && create_fn
        ) {
        for(std::uint32_t i=0; i<FramesInFlight; i++) {
            
            WVkDescriptorPoolInfo dpoolinfo{};

            // Create a descriptor pool by frame index
            create_fn(dpoolinfo, in_device);

            descriptor_pools[i].InsertAt(
                in_id, dpoolinfo
                );
        }
    }


    void ResetDescriptorPool(const WVkDeviceInfo & in_device,
                             const WPipelineIdType & in_id,
                             const std::uint32_t  & in_frameindex) {
        vkResetDescriptorPool(
            in_device.vk_device,
            descriptor_pools[in_frameindex].Get(in_id).descriptor_pool,
            {}
            );
    }

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
    WVkDescriptorPoolDb descriptor_pools{};

};
