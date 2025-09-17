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
    
    static constexpr std::uint8_t frames_in_flight = FramesInFlight;

public:

    WVkPipelinesBase() noexcept = default;

    virtual ~WVkPipelinesBase()=default;

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
    
    virtual constexpr void Initialize(const WVkDeviceInfo & in_device) {
        device_info_ = in_device;
    }

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
            for (auto & bid : pipeline_bindings_[in_id]) {
                pipelines_db_.RemoveBinding(bid, device_info_);
            }

            pipeline_bindings_.erase(in_id);

            pipelines_db_.RemoveDescPool(in_id, device_info_);
            pipelines_db_.RemovePipeline(in_id, device_info_);
            pipelines_db_.RemoveDescSetLayout(in_id, device_info_);

        }

    void DeleteBinding(const WBindingIdType & in_id) {
        pipelines_db_.RemoveBinding(in_id,
                                    device_info_);

        for(auto & p : pipeline_bindings_) {
            if (p.second.Contains(in_id.GetId())) {
                p.second.Remove(in_id.GetId());            
            }
        }
    }

    void ResetDescriptorPool(const WPipelineIdType & in_id,
                             const std::uint32_t  & in_frameindex) {
        vkResetDescriptorPool(
            device_info_.vk_device,
            pipelines_db_.descriptor_pools[in_frameindex].Get(in_id).descriptor_pool,
            {}
            );
    }

    void ResetDescriptorPools(const std::uint32_t  & in_frameindex) {
        pipelines_db_.descriptor_pools[in_frameindex].ForEach(
            [&dev_info = device_info_](WVkDescriptorPoolInfo & _pool) {
                vkResetDescriptorPool(
                    dev_info.vk_device,
                    _pool.descriptor_pool,
                    {}
                    );
            });
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
        for (const auto & wid : pipeline_bindings_.at(in_pipeline_id)) {
            std::forward<TFn>(in_predicate)(wid);
        }
    }

    template<CCallable<void, const WVkPipelineBindingInfo &> TFn>
    void ForEachBinding(const WPipelineIdType & in_pipeline_id, TFn && in_fn) const {
        for (const auto & wid : pipeline_bindings_.at(in_pipeline_id)) {
            std::forward<TFn>(in_fn)(pipelines_db_.bindings.Get(wid));
        }
    }

    auto IterPipelines() const {
        return pipelines_db_.pipelines.IterIndexes();
    }

    auto IterBindings(const WPipelineIdType & in_pipeline_id) const {
        return WIteratorUtils::DefaultIteratorPtr<const WBindingIdType>(
            &(*pipeline_bindings_.at(in_pipeline_id).begin()),
            &(*pipeline_bindings_.at(in_pipeline_id).end())
            );
    }

    template<typename TUBOType>
    std::array<WVkDescriptorSetUBOBinding, FramesInFlight> InitUboDescriptorBinding() {
        std::array<WVkDescriptorSetUBOBinding, frames_in_flight> bindings;

        for(uint32_t i = 0; i < bindings.size(); i++) {
            
            bindings[i].binding = 0;
            bindings[i].ubo_info.range = sizeof(TUBOType);

            WVulkan::CreateUBO(bindings[i].ubo_info, device_info_);

            bindings[i].buffer_info.buffer = bindings[i].ubo_info.uniform_buffer;
            bindings[i].buffer_info.offset = 0;
            bindings[i].buffer_info.range = bindings[i].ubo_info.range;
        }

        return bindings;
    }

    std::vector<WVkDescriptorSetTextureBinding> InitTextureDescriptorBindings(
        const std::vector<WVkTextureInfo> & in_textures,
        const std::vector<std::uint16_t> & in_bindings
        ) {
        std::vector<WVkDescriptorSetTextureBinding> tx{in_textures.size()};
        
        for (uint32_t i=0; i<frames_in_flight; i++) {
            for (uint32_t j = 0; j<tx.size(); j++) {

                tx[j].image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                tx[j].binding = in_bindings[j];
                tx[j].image_info.imageView = in_textures[j].image_view;
                tx[j].image_info.sampler = in_textures[j].sampler;
            }
        }
        return tx;
    }

protected:

    WVkPipelinesDb<WPipelineIdType, WBindingIdType, FramesInFlight> pipelines_db_{};

    
    std::unordered_map<WPipelineIdType, TSparseSet<WBindingIdType>> pipeline_bindings_{};

    WVkDeviceInfo device_info_{};
  
};
