#pragma once

// #include "WVulkan/WVkRenderConfig.hpp"
// #include "WVulkan/WVulkanStructs.hpp"

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkanStructs.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WVulkan/WVkPipelinesDb.hpp"

class WVkPostprocessPipelines
{
public:

    // WVkPostprocessPipelines() noexcept =default;

    // WVkPostprocessPipelines(const WVkDeviceInfo & in_device_info) :
    //     device_info_(in_device_info) {}

    // virtual ~WVkPostprocessPipelines()=default;

    // WVkPostprocessPipelines(const WVkPostprocessPipelines &)=delete;

    // WVkPostprocessPipelines(WVkPostprocessPipelines &&) noexcept = default;

    // WVkPostprocessPipelines & operator=(WVkPostprocessPipelines &&) noexcept = default;
    
public:

    // void CreatePipeline(const WAssetId & in_id,
    //                     const WRenderPipelineStruct & in_pipeline_struct);

    // WEntityComponentId CreateBinding(const WAssetId & in_pipeline_id,
    //                                  std::vector<float> in_values,
    //                                  std::vector<std::uint16_t> in_values_bindings,
    //                                  std::vector<WVkTextureInfo> in_textures,
    //                                  std::vector<std::uint16_t> in_texture_bindings);

    // void DeletePipeline(
    //     const WAssetId & in_id
    //     );

    // void DeleteBinding(const WEntityComponentId & in_id);

    // void ResetDescriptorPool(const WAssetId & in_id, const std::uint32_t & in_frameindex);

    //     template<CCallable<void, const WAssetId&, WVkRenderPipelineInfo&> TFn>
    // void ForEachPipeline(TFn && in_fn) const {
    //     pipelines_db_.pipelines.ForEachIdValue(std::forward<TFn>(in_fn));
    // }

    // template<CCallable<void, const WEntityComponentId &> TFn>
    // void ForEachBinding(const WAssetId & in_pipeline_id, TFn && in_predicate) const {
    //     for (const auto & wid : pipeline_pbindings_.at(in_pipeline_id)) {
    //         std::forward<TFn>(in_predicate)(wid);
    //     }
    // }

    // template<CCallable<void, const WVkPipelineBindingInfo &> TFn>
    // void ForEachBinding(const WAssetId & in_pipeline_id, TFn && in_fn) const {
    //     for (const auto & wid : pipeline_pbindings_.at(in_pipeline_id)) {
    //         std::forward<TFn>(in_fn)(bindings_.Get(wid));
    //     }
    // }

    // auto IterPipelines() const {
    //     return pipelines_db_.pipelines.IterIndexes();
    // }


private:

    // WVkPipelinesDb<WAssetId, WENG_MAX_FRAMES_IN_FLIGHT> pipelines_db_;

    // WVkDeviceInfo device_info_;

};


