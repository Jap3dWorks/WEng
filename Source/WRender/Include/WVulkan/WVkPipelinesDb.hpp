#pragma once

#include "WCore/WCore.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WCore/WConcepts.hpp"
#include "WCore/WStringUtils.hpp"

#include <utility>
#include <vulkan/vulkan_core.h>

template<typename WPipelineIdType=WAssetId, std::uint32_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
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

    void CreatePipeline(const WVkDeviceInfo & in_device_info,
                        const WPipelineIdType & in_pipeline_id,
                        const std::vector<WVkDescriptorSetLayoutInfo> & in_descset_layouts,
                        const std::vector<WVkShaderStageInfo> & in_shaders) {
        
        WVkRenderPipelineInfo render_pipeline_info;

        WVulkan::Create(
            render_pipeline_info,
            in_device_info,
            in_descset_layouts,
            in_shaders
            );

        pipelines.InsertAt(in_pipeline_id, render_pipeline_info);

        render_pipeline_info.wid = in_pipeline_id;
        render_pipeline_info.descriptor_set_layout_id = in_pipeline_id;
    }

    template<CCallable<void, WVkDescriptorSetLayoutInfo&> ConfigInfoFn>
    void CreateDescSetLayout(const WVkDeviceInfo & in_device,
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
    void CreateDescSetPool(const WVkDeviceInfo & in_device,
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

    template<typename ShadersData,
             CCallable<WVkShaderStageInfo, const char*, const char *, EShaderType> TFn>
    std::vector<WVkShaderStageInfo> BuildShaders(const std::uint32_t & shaders_count,
                                                 ShadersData && in_data,
                                                 TFn && in_fn) {
        std::vector<WVkShaderStageInfo> result;
        result.reserve(shaders_count);

        // TODO transition to slang
        for (uint8_t i=0; i < shaders_count; i++) {
            result.push_back(
                std::forward<TFn>(in_fn)(
                    WStringUtils::SystemPath(std::forward<ShadersData>(in_data)[i].file).c_str(),
                    std::forward<ShadersData>(in_data)[i].entry,
                    std::forward<ShadersData>(in_data)[i].type)
                );
        }
        
        return result;
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

    void RemovePipeline(const WVkDeviceInfo & in_device, const WPipelineIdType & in_id) {
        pipelines.Remove(in_id, [&in_device](auto & rpip) {
            WVulkan::Destroy(
                rpip,
                in_device);
        });
    }

    void RemoveDescSetLayout(const WVkDeviceInfo & in_device, const WPipelineIdType & in_id) {
        descriptor_set_layouts.Remove(in_id,
                                      [&in_device](auto & dsetlay) {
                                          WVulkan::Destroy(dsetlay,
                                                           in_device);
                                      });
    }

    void RemoveDescPool(const WVkDeviceInfo & in_device, const WPipelineIdType & in_id) {
        for(std::uint32_t i=0; i<FramesInFlight; i++) {
            descriptor_pools[i].Remove(in_id,
                                       [&in_device](auto & dpool) {
                                           WVulkan::Destroy(dpool, in_device);
                                       });
        }
    }

    void Clear(const WVkDeviceInfo & in_device) {
        
        for(std::uint32_t i=0; i < FramesInFlight; i++) {
            descriptor_pools[i].Clear(
                [di_=in_device](auto & b) {
                    WVulkan::Destroy(b, di_);
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
