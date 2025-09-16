#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVkPipelinesDb.hpp"
#include "WVulkan/WVulkanStructs.hpp"

#include <cstdint>

template<typename WPipelineIdType=WAssetId,
         typename WBindingIdType=WEntityComponentId,
         std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkPipelinesBase {

public:

    WVkPipelinesBase() noexcept = default;

    virtual ~WVkPipelinesBase()=default;

    WVkPipelinesBase(
        WVkDeviceInfo in_device 
        ) :
        pipelines_db_(),
        device_info_(in_device) {}

    WVkPipelinesBase(const WVkPipelinesBase & other)=delete;

    WVkPipelinesBase(
        WVkPipelinesBase && other
        ) noexcept :
        pipelines_db_(std::move(other.pipelines_db_)),
        device_info_(std::move(other.device_info_))
        {
            other.device_info_ = {};
        }

    WVkPipelinesBase & operator=(
        const WVkPipelinesBase & other
        )=delete;

    WVkPipelinesBase & operator=(
        WVkPipelinesBase && other
        ) noexcept {
        if (this != &other) {
            ClearPipelinesDb();
            
            pipelines_db_ = std::move(other.pipelines_db_);
            device_info_ = std::move(other.device_info_);

            other.device_info_ = {};
        }

        return *this;
    }

public:

    /**
     * @brief Remove current active pipelines and bindings, resulting instance can be used.
     */
    void ClearPipelinesDb() {
        pipelines_db_.Clear(device_info_);
    }

    void DeletePipeline(
        const WPipelineIdType & in_id
        )
        {
            // Remove bindings
            for (auto & bid : pipeline_pbindings_[in_id]) {
                pipelines_db_.RemoveBinding(bid, device_info_);
            }

            pipeline_pbindings_.erase(in_id);

            pipelines_db_.RemoveDescPool(in_id, device_info_);
            pipelines_db_.RemovePipeline(in_id, device_info_);
            pipelines_db_.RemoveDescSetLayout(in_id, device_info_);

        }

    void DeleteBinding(const WBindingIdType & in_id) {
        pipelines_db_.RemoveBinding(in_id,
                                    device_info_);

        for(auto & p : pipeline_pbindings_) {
            if (p.second.Contains(in_id.GetId())) {
                p.second.Remove(in_id.GetId());            
            }
        }
    }

    void ResetDescriptorPool(const WPipelineIdType & in_pipeline_id, const std::uint32_t & in_frameindex) {
        pipelines_db_.ResetDescriptorPool(in_pipeline_id, device_info_, in_frameindex);
    }

    WNODISCARD const WVkRenderPipelineInfo & Pipeline(const WPipelineIdType & in_id) const {
        return pipelines_db_.pipelines.Get(in_id);
    }

    WNODISCARD const WVkDescriptorSetLayoutInfo & DescriptorSetLayout(const WPipelineIdType & in_id) const {
        return pipelines_db_.descriptor_set_layouts.Get(in_id);
    }

    WNODISCARD const WVkDescriptorPoolInfo & DescriptorPool(const WPipelineIdType & in_id,
                                                            const std::uint32_t & in_frameindex) const {
        return pipelines_db_.descriptor_pools.at(in_frameindex).Get(in_id);
    }

    WNODISCARD const WVkPipelineBindingInfo & Binding(const WBindingIdType & in_id) const {
        return pipelines_db_.bindings.Get(in_id);
    }

    template<CCallable<void, const WPipelineIdType &, WVkRenderPipelineInfo&> TFn>
    void ForEachPipeline(TFn && in_fn) const {
        pipelines_db_.pipelines.ForEachIdValue(std::forward<TFn>(in_fn));
    }

    template<CCallable<void, const WBindingIdType &> TFn>
    void ForEachBinding(const WPipelineIdType & in_pipeline_id, TFn && in_predicate) const {
        for (const auto & wid : pipeline_pbindings_.at(in_pipeline_id)) {
            std::forward<TFn>(in_predicate)(wid);
        }
    }

    template<CCallable<void, const WVkPipelineBindingInfo &> TFn>
    void ForEachBinding(const WPipelineIdType & in_pipeline_id, TFn && in_fn) const {
        for (const auto & wid : pipeline_pbindings_.at(in_pipeline_id)) {
            std::forward<TFn>(in_fn)(pipelines_db_.bindings.Get(wid));
        }
    }

    auto IterPipelines() const {
        return pipelines_db_.pipelines.IterIndexes();
    }

    auto IterBindings(const WPipelineIdType & in_pipeline_id) const {
        return WIteratorUtils::DefaultIteratorPtr<const WBindingIdType>(
            &(*pipeline_pbindings_.at(in_pipeline_id).begin()),
            &(*pipeline_pbindings_.at(in_pipeline_id).end())
            );
    }

protected:

    WVkPipelinesDb<WPipelineIdType, WBindingIdType, FramesInFlight> pipelines_db_{};

    std::unordered_map<WPipelineIdType, TSparseSet<WBindingIdType>> pipeline_pbindings_{};

    WVkDeviceInfo device_info_{};
  
};
