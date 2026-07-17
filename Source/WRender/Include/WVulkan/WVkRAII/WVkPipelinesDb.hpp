#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkDescriptor.hpp"
#include "WVulkan/WVk/WVkBuffer.hpp"
#include "WVulkan/WVk/WVkPipeline.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include "WCore/WConcepts.hpp"
#include "WString/WString.hpp"

#include <utility>
#include <vulkan/vulkan_core.h>

template<wcr::wid::CIsWId WPipelineIdType=wcr::wid::WAssetId,
         wcr::wid::CIsWId WBindingIdType=wcr::wid::WEntityComponentId,
         std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkPipelinesDb {
public:

    using WVkPipelineDb =
        TObjectDataBase<WVkRenderPipelineInfo, void, typename WPipelineIdType::IdType>;

    using WVkDescSetLayoutDb =
        TObjectDataBase<WVkDescriptorSetLayoutInfo, void, typename  WPipelineIdType::IdType>;

    using WVkDescriptorPoolDb = std::array<
        TObjectDataBase<VkDescriptorPool, void, typename WPipelineIdType::IdType>,
        FramesInFlight
        >;

    using WVkPipelineBindingDb = TObjectDataBase<WVkPipelineBindingInfo, void, wcr::wid::WEntityComponentId::IdType>;

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
                       const VkDevice &,
                       const WVkDescriptorSetLayoutInfo &,
                       const std::vector<WVkShaderStageInfo> &> TCrtFn>
    void CreatePipeline(const WPipelineIdType & in_pipeline_id,
                        const VkDevice & in_device,
                        const WPipelineIdType & in_desclay_id,
                        const std::vector<WVkShaderStageInfo> & in_shaders,
                        TCrtFn && in_create_fn
        ) {
        
        WVkRenderPipelineInfo render_pipeline_info;

        std::forward<TCrtFn>(in_create_fn)(
            render_pipeline_info,
            in_device,
            descriptor_set_layouts.Get(in_desclay_id),
            in_shaders
            );

        pipelines.InsertAt(in_pipeline_id, render_pipeline_info);

        render_pipeline_info.wid = in_pipeline_id;
        render_pipeline_info.descriptor_set_layout_id = in_pipeline_id;
    }

    template<CCallable<void, WVkDescriptorSetLayoutInfo&, const wct::render::RPipeParamDescLayList &> ConfigInfoFn>
    void CreateDescSetLayout(const WPipelineIdType & in_id,
                             const VkDevice & in_device,
                             const wct::render::RPipeParamDescLayList & params,
                             ConfigInfoFn && config_fn) {
        
        WVkDescriptorSetLayoutInfo descriptor_set_layout_info;

        std::forward<ConfigInfoFn>(config_fn)(descriptor_set_layout_info, params);

        descriptor_set_layouts.CreateAt(
            in_id,
            [this, &descriptor_set_layout_info, &in_device]
            (const WPipelineIdType & _in_id) -> auto {
                wvk::descriptor::Create(
                    descriptor_set_layout_info,
                    in_device
                    );
                // descriptor_set_layout_info.wid = _in_id;
                return descriptor_set_layout_info;
            });
    }

    template<CCallable<void, VkDescriptorPool & /* out */, const VkDevice&> TCreateFn>
    void CreateDescSetPool(const WPipelineIdType & in_id,
                           const VkDevice & in_device,
                           TCreateFn && create_fn) {
        
        for(std::uint32_t i=0; i<FramesInFlight; i++) {
            
            VkDescriptorPool dpoolinfo{};

            // Create a descriptor pool by frame index
            create_fn(dpoolinfo, in_device);

            descriptor_pools[i].InsertAt(
                in_id, dpoolinfo
                );
        }
    }

    // TODO do not use const char *, use std::string_view
    template<CCallable<WVkShaderStageInfo,
                       const char*, const char *,
                       wct::render::EShaderStageFlag> TFn>
    std::vector<WVkShaderStageInfo> BuildShaders(const wct::render::ShaderList & in_data,
                                                 TFn && in_fn) {
        std::vector<WVkShaderStageInfo> result;

        result.reserve(in_data.size());

        wct::render::ForEach(
            in_data,
            [&result, &in_fn]
            (const wct::render::ShaderInfo & shd) {
                result.push_back(
                    in_fn(wstr::SystemPath(shd.file.View()).c_str(),
                          std::string(shd.entry.View()).c_str(),
                          shd.type)
                    );
            });
        
        return result;
    }

    void RemovePipeline(const WPipelineIdType & in_id, const VkDevice & in_device) {
        pipelines.Remove(in_id, [&in_device](auto & rpip) {
            wvk::pipeline::Destroy(
                rpip,
                in_device);
        });
    }

    void RemoveDescSetLayout(const WPipelineIdType & in_id, const VkDevice & in_device) {
        descriptor_set_layouts.Remove(in_id,
                                      [&in_device](auto & dsetlay) {
                                          wvk::descriptor::Destroy(dsetlay,
                                                                   in_device);
                                      });
    }

    void RemoveDescPool(const WPipelineIdType & in_id, const VkDevice & in_device) {
        for(std::uint32_t i=0; i<FramesInFlight; i++) {
            descriptor_pools[i].Remove(in_id,
                                       [&in_device]
                                       (auto & dpool) {
                                           wvk::descriptor::Destroy(dpool, in_device);
                                       });
        }
    }

    void RemoveBinding( const WBindingIdType & in_id, const VkDevice & in_device) {
        // TODO do not remove ubos, let it to AssetRenderDataRAII
        bindings.Remove(in_id,
                        [di_=in_device](auto & b) {
                            for(auto& ubofrm: b.ubos) {
                                for(auto& ubo:ubofrm) {
                                    wvk::buffer::Destroy(ubo.ubo_info, di_);
                                }
                            }
                        }
            );
    }

    void Clear(const VkDevice & in_device) {
        
        for(std::uint32_t i=0; i < FramesInFlight; i++) {
            descriptor_pools[i].Clear(
                [di_=in_device](auto & b) {
                    wvk::descriptor::Destroy(b, di_);
                }
                );
        }
        
        pipelines.Clear(
            [di_=in_device](auto & p) {
                wvk::pipeline::Destroy(
                    p,
                    di_
                    );
            });

        descriptor_set_layouts.Clear(
            [di_=in_device](auto & d) {
                wvk::descriptor::Destroy(
                    d,
                    di_
                    );
            });

        bindings.Clear(
            [di_=in_device](auto & b) {
                for(auto& ubofrm: b.ubos) {
                    for(auto& ubo : ubofrm) {
                        // TODO do not remove ubos, let it to AssetRenderDataRAII
                        wvk::buffer::Destroy(ubo.ubo_info, di_);
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
