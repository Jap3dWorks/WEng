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

template<typename WPipelineIdType=WAssetId,
         typename WBindingIdType=WEntityComponentId,
         std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkPipelinesDb {
public:

    using WVkPipelineDb = TObjectDataBase<WVkRenderPipelineInfo, void, WPipelineIdType>;
    using WVkDescSetLayoutDb = TObjectDataBase<WVkDescriptorSetLayoutInfo, void, WPipelineIdType>;
    using WVkDescriptorPoolDb = std::array<
        TObjectDataBase<WVkDescriptorPoolInfo, void, WPipelineIdType>,
        FramesInFlight
        >;
    using WVkPipelineBindingDb = TObjectDataBase<WVkPipelineBindingInfo, void, WEntityComponentId>;

public:

    WVkPipelinesDb() noexcept = default;

    virtual ~WVkPipelinesDb() = default;

    WVkPipelinesDb(const WVkPipelinesDb &) = delete;

    WVkPipelinesDb(WVkPipelinesDb &&) noexcept = default;

    WVkPipelinesDb & operator=(const WVkPipelinesDb &) = delete;

    WVkPipelinesDb & operator=(WVkPipelinesDb &&) noexcept = default;

public:

    template<CCallable<void,
                       WVkRenderPipelineInfo &,
                       const WVkDeviceInfo &,
                       const WVkDescriptorSetLayoutInfo &,
                       const std::vector<WVkShaderStageInfo> &> TCrtFn>
    void CreatePipeline(const WPipelineIdType & in_pipeline_id,
                        const WVkDeviceInfo & in_device_info,
                        const WPipelineIdType & in_desclay_id,
                        const std::vector<WVkShaderStageInfo> & in_shaders,
                        TCrtFn && in_create_fn
        ) {
        
        WVkRenderPipelineInfo render_pipeline_info;

        std::forward<TCrtFn>(in_create_fn)(
            render_pipeline_info,
            in_device_info,
            descriptor_set_layouts.Get(in_desclay_id),
            in_shaders
            );

        pipelines.InsertAt(in_pipeline_id, render_pipeline_info);

        render_pipeline_info.wid = in_pipeline_id;
        render_pipeline_info.descriptor_set_layout_id = in_pipeline_id;
    }

    template<CCallable<void, WVkDescriptorSetLayoutInfo&, const WPipeParamDescriptorList &> ConfigInfoFn>
    void CreateDescSetLayout(const WPipelineIdType & in_id,
                             const WVkDeviceInfo & in_device,
                             const WPipeParamDescriptorList & params,
                             ConfigInfoFn && info_fn) {
        
        WVkDescriptorSetLayoutInfo descriptor_set_layout_info;

        std::forward<ConfigInfoFn>(info_fn)(descriptor_set_layout_info, params);

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
    void CreateDescSetPool(const WPipelineIdType & in_id,
                           const WVkDeviceInfo & in_device,
                           TCreateFn && create_fn) {
        
        for(std::uint32_t i=0; i<FramesInFlight; i++) {
            
            WVkDescriptorPoolInfo dpoolinfo{};

            // Create a descriptor pool by frame index
            create_fn(dpoolinfo, in_device);

            descriptor_pools[i].InsertAt(
                in_id, dpoolinfo
                );
        }
    }

    template<CCallable<WVkShaderStageInfo,
                       const char*, const char *,
                       EShaderStageFlag> TFn>
    std::vector<WVkShaderStageInfo> BuildShaders(const WShaderList & in_data,
                                                 TFn && in_fn) {
        std::vector<WVkShaderStageInfo> result;

        result.reserve(in_data.size());

        WRenderUtils::ForEach(
            in_data,
            [&result, &in_fn]
            (const WShaderStruct & shd) {
                result.push_back(
                    in_fn(WStringUtils::SystemPath(shd.file).c_str(),
                          shd.entry,
                          shd.type)
                    );
            });
        
        return result;
    }

    void RemovePipeline(const WPipelineIdType & in_id, const WVkDeviceInfo & in_device) {
        pipelines.Remove(in_id, [&in_device](auto & rpip) {
            WVulkan::Destroy(
                rpip,
                in_device);
        });
    }

    void RemoveDescSetLayout(const WPipelineIdType & in_id, const WVkDeviceInfo & in_device) {
        descriptor_set_layouts.Remove(in_id,
                                      [&in_device](auto & dsetlay) {
                                          WVulkan::Destroy(dsetlay,
                                                           in_device);
                                      });
    }

    void RemoveDescPool(const WPipelineIdType & in_id, const WVkDeviceInfo & in_device) {
        for(std::uint32_t i=0; i<FramesInFlight; i++) {
            descriptor_pools[i].Remove(in_id,
                                       [&in_device]
                                       (auto & dpool) {
                                           WVulkan::Destroy(dpool, in_device);
                                       });
        }
    }

    void RemoveBinding( const WBindingIdType & in_id, const WVkDeviceInfo & in_device) {
        bindings.Remove(in_id,
                        [di_=in_device](auto & b) {
                            for(auto& ubofrm: b.ubos) {
                                for(auto& ubo:ubofrm) {
                                    WVulkan::Destroy(ubo.ubo_info, di_);
                                }
                            }
                        }
            );
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

        bindings.Clear(
            [di_=in_device](auto & b) {
                for(auto& ubofrm: b.ubos) {
                    for(auto& ubo : ubofrm) {
                        WVulkan::Destroy(ubo.ubo_info, di_);
                    }
                }
            }
            );

    }

public:

    WVkPipelineDb pipelines{};
    WVkDescSetLayoutDb descriptor_set_layouts{};
    WVkDescriptorPoolDb descriptor_pools{};
    WVkPipelineBindingDb bindings{};

};
