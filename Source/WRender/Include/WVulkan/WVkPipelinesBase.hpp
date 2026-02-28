#pragma once

#include "WCore/WCore.hpp"
#include "WStructs/WRenderStructs.hpp"
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
        device_(std::move(other.device_)),
        physical_device_(std::move(other.physical_device_))
        // device_info_(std::move(other.device_info_))
        {
            other.device_ = VK_NULL_HANDLE;
            other.physical_device_ = VK_NULL_HANDLE;
            // other.device_info_ = {};
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
            device_ = std::move(other.device_);
            physical_device_ = std::move(other.physical_device_);

            other.device_ = VK_NULL_HANDLE;
            other.physical_device_ = VK_NULL_HANDLE;
        }

        return *this;
    }

public:
    
    virtual constexpr void Initialize(
        const VkDevice & in_device, const VkPhysicalDevice & in_physical_device) {

        device_ = in_device;
        physical_device_ = in_physical_device;
    }

    /**
     * @brief Remove current active pipelines and bindings, resulting instance can be used.
     */
    void ClearPipelinesDb() {
        pipelines_db_.Clear(device_);
    }

    void DeletePipeline(
        const WPipelineIdType & in_id
        ) {
        // Remove bindings
        for (auto & bid : pipeline_bindings_[in_id]) {
            pipelines_db_.RemoveBinding(bid, device_);
        }

        pipeline_bindings_.erase(in_id);

        pipelines_db_.RemoveDescPool(in_id, device_);
        pipelines_db_.RemovePipeline(in_id, device_);
        pipelines_db_.RemoveDescSetLayout(in_id, device_);

    }

    void DeleteBinding(const WBindingIdType & in_id) {
        pipelines_db_.RemoveBinding(in_id, device_);

        for(auto & p : pipeline_bindings_) {
            if (p.second.Contains(in_id.GetId())) {
                p.second.Remove(in_id.GetId());            
            }
        }
    }

    void ResetDescriptorPool(const WPipelineIdType & in_id,
                             const std::uint32_t  & in_frameindex) {
        vkResetDescriptorPool(
            device_,
            pipelines_db_.descriptor_pools[in_frameindex].Get(in_id).descriptor_pool,
            {}
            );
    }

    void ResetDescriptorPools(const std::uint32_t  & in_frameindex) {
        pipelines_db_.descriptor_pools[in_frameindex].ForEach(
            [&dev_info = device_](WVkDescriptorPoolInfo & _pool) {
                vkResetDescriptorPool(
                    dev_info,
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

        for ( auto & b : binding.ubos) {
            if(b[0].binding == ubo_write.binding) {
                ubo = &b;
                break;
            }
        }

        if(!ubo) return;

        WVulkan::MapUBO((*ubo)[in_frame_index].ubo_info, device_);
        WVulkan::UpdateUBO((*ubo)[in_frame_index].ubo_info, ubo_write.data, ubo_write.size, ubo_write.offset);
        WVulkan::UnmapUBO((*ubo)[in_frame_index].ubo_info, device_);
    }

    void UpdateBinding(const WBindingIdType & in_binding_id,
                       const WVkDescriptorSetUBOWriteStruct & ubo_write
        ) {
        auto & binding = pipelines_db_.bindings.Get(in_binding_id);

        TVkDescriptorSetUBOBindingFrames<FramesInFlight> * ubo = nullptr;

        for ( auto & b : binding.ubos) {
            if(b[0].binding == ubo_write.binding) {
                ubo = &b;
                break;
            }
        }

        if(!ubo) return;

        for(auto & b: (*ubo)) {
            WVulkan::MapUBO(b.ubo_info, device_);
            WVulkan::UpdateUBO(b.ubo_info, ubo_write.data, ubo_write.size, ubo_write.offset);
            WVulkan::UnmapUBO(b.ubo_info, device_);            
        }
    }

protected:
    
    std::vector<TVkDescriptorSetUBOBindingFrames<FramesInFlight>> InitUboDescriptorBindings(
        const WPipeParamDescriptorList & in_param_descriptors,
        const std::vector<WVkDescriptorSetUBOWriteStruct> & in_ubos
        ) {

        std::unordered_map<
            std::uint8_t,
            TVkDescriptorSetUBOBindingFrames<FramesInFlight>
            > bindings;

        std::vector<TVkDescriptorSetUBOBindingFrames<FramesInFlight>> result;
        result.reserve(in_param_descriptors.size());

        WRenderUtils::ForEach(
            in_param_descriptors,
            [&bindings, &result, this]
            (const WPipeParamDescriptorStruct & ubopd) {
                if(ubopd.type == EPipeParamType::Ubo && !bindings.contains(ubopd.binding)) {

                    for (std::uint32_t frm=0; frm<FramesInFlight; frm++) {
                        bindings[ubopd.binding][frm]={};
                        bindings[ubopd.binding][frm].binding = ubopd.binding;
                        bindings[ubopd.binding][frm].ubo_info.range = ubopd.range;

                        WVulkan::CreateUBO(
                            bindings[ubopd.binding][frm].ubo_info,
                            device_,
                            physical_device_
                            );

                        bindings[ubopd.binding][frm].buffer_info.buffer =
                            bindings[ubopd.binding][frm].ubo_info.buffer;
                        
                        bindings[ubopd.binding][frm].buffer_info.offset = 0;

                        bindings[ubopd.binding][frm].buffer_info.range =
                            bindings[ubopd.binding][frm].ubo_info.range;
                    }
            
                    result.push_back(bindings[ubopd.binding]);
                }
            });
        
        for(auto & ubo : in_ubos) {
            for (std::uint32_t frm=0; frm<FramesInFlight; frm++) {
                WVulkan::MapUBO(bindings[ubo.binding][frm].ubo_info, device_);
                WVulkan::UpdateUBO(bindings[ubo.binding][frm].ubo_info,
                                   in_ubos[ubo.binding].data,
                                   in_ubos[ubo.binding].size,
                                   in_ubos[ubo.binding].offset);
                WVulkan::UnmapUBO(bindings[ubo.binding][frm].ubo_info, device_);                
            }
        }

        return result;
    }

    std::vector<WVkDescriptorSetTextureBinding> InitTextureDescriptorBindings(
        const WPipeParamDescriptorList & in_param_descriptors,
        const std::vector<WVkDescriptorSetTextureWriteStruct> & in_textures
        ) {

        std::vector<WPipeParamDescriptorStruct> result;
        result.reserve(in_param_descriptors.size());
        
        std::unordered_map<std::uint8_t, WVkDescriptorSetTextureWriteStruct> bindings{};

        WRenderUtils::ForEach(
            in_param_descriptors,
            [&bindings]
            (const WPipeParamDescriptorStruct & _ubopd) {
                if(_ubopd.type == EPipeParamType::Texture && !bindings.contains(_ubopd.binding)) {
                    bindings[_ubopd.binding]={};
                    bindings[_ubopd.binding].binding = _ubopd.binding;
                    bindings[_ubopd.binding].image_info = {}; // TODO: default image or something
                }
            });

        for(auto& wrt : in_textures) {
            bindings[wrt.binding].image_info.imageLayout = wrt.image_info.imageLayout;
            bindings[wrt.binding].image_info.imageView = wrt.image_info.imageView;
            bindings[wrt.binding].image_info.sampler = wrt.image_info.sampler;
        }

        std::vector<WVkDescriptorSetTextureBinding> tx{};
        tx.reserve(bindings.size());

        for(auto&p : bindings) {
            tx.push_back(p.second);
        }

        return tx;
    }

    inline bool ValidateBindingParams(
        const WPipeParamDescriptorList & in_pipeline_params,
        const std::vector<WVkDescriptorSetUBOWriteStruct> & in_ubos,
        const std::vector<WVkDescriptorSetTextureWriteStruct> & in_textures) {

        std::unordered_set<std::uint8_t> pipebindings{};
        
        for(const auto & b : in_pipeline_params) {
            pipebindings.insert(b.binding);
        }

        for(const auto & ubo : in_ubos) {
            if (!pipebindings.contains(ubo.binding)) {
                WFLOG("Invalid binding {}", ubo.binding);
                return false;
            }
            pipebindings.erase(ubo.binding);
        }

        for (const auto & tex : in_textures) {
            if (!pipebindings.contains(tex.binding)) {
                WFLOG("Invalid binding {}", tex.binding);
                return false;
            }
            pipebindings.erase(tex.binding);
        }

        return pipebindings.empty();

    }

protected:

    WVkPipelinesDb<WPipelineIdType, WBindingIdType, FramesInFlight> pipelines_db_{};

    std::unordered_map<WPipelineIdType, TSparseSet<WBindingIdType>> pipeline_bindings_{};

    VkDevice device_{VK_NULL_HANDLE};
    VkPhysicalDevice physical_device_{VK_NULL_HANDLE};
  
};
