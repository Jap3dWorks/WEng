#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVkPipelinesDb.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVulkanStructs.hpp"

#include <cstdint>
#include <unordered_set>

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

    void UpdateBinding(const WBindingIdType & in_binding_id,
                       const std::uint32_t & in_frame_index,
                       const WVkDescriptorSetUBOWriteStruct & ubo_write
        ) {
        auto & binding = pipelines_db_.bindings.Get(in_binding_id);
        
        TVkDescriptorSetUBOBindingFrames<FramesInFlight> * ubo = nullptr;

        // TODO: Check using pipeline description
        for ( auto & b : binding.ubos) {
            if(b[0].binding == ubo_write.binding) {
                ubo = &b;
                break;
            }
        }

        if(!ubo) return;

        WVulkan::MapUBO((*ubo)[in_frame_index].ubo_info, device_info_);
        
        WVulkan::UpdateUBO((*ubo)[in_frame_index].ubo_info, ubo_write.data, ubo_write.size, ubo_write.offset);

        WVulkan::UnmapUBO((*ubo)[in_frame_index].ubo_info, device_info_);
    }

    void UpdateBinding(const WBindingIdType & in_binding_id,
                       const WVkDescriptorSetUBOWriteStruct & ubo_write
        ) {
        auto & binding = pipelines_db_.bindings.Get(in_binding_id);
        
        TVkDescriptorSetUBOBindingFrames<FramesInFlight> * ubo = nullptr;

        // TODO: Check using pipeline description
        for ( auto & b : binding.ubos) {
            if(b[0].binding == ubo_write.binding) {
                ubo = &b;
                break;
            }
        }

        if(!ubo) return;

        for(auto & b: (*ubo)) {
            WVulkan::MapUBO(b.ubo_info, device_info_);
            WVulkan::UpdateUBO(b.ubo_info, ubo_write.data, ubo_write.size, ubo_write.offset);
            WVulkan::UnmapUBO(b.ubo_info, device_info_);            
        }
    }

protected:
    
    std::vector<TVkDescriptorSetUBOBindingFrames<FramesInFlight>> InitUboDescriptorBindings(
        const std::vector<WVkDescriptorSetUBOWriteStruct> & in_ubos
        ) {

        std::unordered_map<std::uint32_t, TVkDescriptorSetUBOBindingFrames<FramesInFlight>> bindings;
        std::vector<TVkDescriptorSetUBOBindingFrames<FramesInFlight>> result;

        for(std::uint32_t i = 0; i < in_ubos.size(); i++) {
            if(!bindings.contains(in_ubos[i].binding)) {
                for (std::uint32_t frm=0; frm<FramesInFlight; frm++) {
                    
                    bindings[i][frm].binding = in_ubos[i].binding;
                    bindings[i][frm].ubo_info.range = in_ubos[i].range;

                    WVulkan::CreateUBO(bindings[i][frm].ubo_info, device_info_);

                    bindings[i][frm].buffer_info.buffer = bindings[i][frm].ubo_info.buffer;
                    bindings[i][frm].buffer_info.offset = 0; // use the whole buffer
                    bindings[i][frm].buffer_info.range = bindings[i][frm].ubo_info.range;
                }
                result.push_back(bindings[i]);
            }

            for (std::uint32_t frm=0; i<FramesInFlight; frm++) {
                WVulkan::MapUBO(bindings[i][frm].ubo_info, device_info_);
                WVulkan::UpdateUBO(bindings[i][frm].ubo_info,
                                   in_ubos[i].data,
                                   in_ubos[i].size,
                                   in_ubos[i].offset);
                WVulkan::UnmapUBO(bindings[i][frm].ubo_info, device_info_);                
            }
        }

        return result;
    }

    std::vector<WVkDescriptorSetTextureBinding> InitTextureDescriptorBindings(
        const std::vector<WVkDescriptorSetTextureWriteStruct> & in_textures
        ) {
        std::vector<WVkDescriptorSetTextureBinding> tx{in_textures.size()};

        for (std::uint32_t i = 0; i<tx.size(); i++) {
            tx[i].binding = in_textures[i].binding;

            tx[i].image_info.imageLayout = in_textures[i].image_info.imageLayout;
            tx[i].image_info.imageView = in_textures[i].image_info.imageView;
            tx[i].image_info.sampler = in_textures[i].image_info.sampler;
        }

        return tx;
    }

protected:

    WVkPipelinesDb<WPipelineIdType, WBindingIdType, FramesInFlight> pipelines_db_{};

    std::unordered_map<WPipelineIdType, TSparseSet<WBindingIdType>> pipeline_bindings_{};

    WVkDeviceInfo device_info_{};
  
};
