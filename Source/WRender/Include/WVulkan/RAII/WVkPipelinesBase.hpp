#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/WVkConfig.hpp"
#include "WVkPipelinesDb.hpp"
#include "WVulkan/Vk/WVkBuffer.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/WVulkanStructs.hpp"

#include <cstdint>
#include <unordered_set>

template<typename WPipelineIdType=wcr::wid::WAssetId,
         typename WBindingIdType=wcr::wid::WEntityComponentId,
         std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
class WVkPipelinesBase {
public:
    
    static constexpr std::uint8_t frames_in_flight = FramesInFlight;

public:

    WVkPipelinesBase() noexcept = default;

    WVkPipelinesBase(
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device
        ) : device_(in_device),
            physical_device_(in_physical_device)
        {}

    virtual ~WVkPipelinesBase() {
        Destroy();
    }

    WVkPipelinesBase(const WVkPipelinesBase & other)=delete;
    WVkPipelinesBase & operator=(const WVkPipelinesBase & other)=delete;

    WVkPipelinesBase(WVkPipelinesBase && other) noexcept :
        pipelines_db_(std::move(other.pipelines_db_)),
        device_(std::move(other.device_)),
        physical_device_(std::move(other.physical_device_)),
        pipeline_bindings_(std::move(other.pipeline_bindings_))
        {
            other.device_ = VK_NULL_HANDLE;
            other.physical_device_ = VK_NULL_HANDLE;
        }

    WVkPipelinesBase & operator=(WVkPipelinesBase && other) noexcept {
        if (this != &other) {
            Destroy();
            
            pipelines_db_ = std::move(other.pipelines_db_);
            device_ = std::move(other.device_);
            physical_device_ = std::move(other.physical_device_);
            pipeline_bindings_ = std::move(other.pipeline_bindings_);

            other.device_ = VK_NULL_HANDLE;
            other.physical_device_ = VK_NULL_HANDLE;
        }

        return *this;
    }

public:

    /**
     * @brief Remove current active pipelines and bindings,
     *  resulting instance can be used.
     */
    void ClearPipelinesDb() {
        pipelines_db_.Clear(device_);
        pipeline_bindings_.clear();
    }

    void DeletePipeline(
        const WPipelineIdType & in_id
        ) {
        // Remove bindings
        for (auto & bid : pipeline_bindings_[in_id]) {
            pipelines_db_.RemoveBinding(bid, device_);
        }

        // remove pipeline binding track
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
            pipelines_db_.descriptor_pools[in_frameindex].Get(in_id),
            {}
            );
    }

    void ResetDescriptorPools(const std::uint32_t  & in_frameindex) {
        pipelines_db_.descriptor_pools[in_frameindex].ForEach(
            [&dev_info = device_](VkDescriptorPool & _pool) {
                vkResetDescriptorPool(
                    dev_info,
                    _pool,
                    {}
                    );
            });
    }

    WNODISCARD const WVkRenderPipeline & Pipeline(const WPipelineIdType & in_id) const {
        return pipelines_db_.pipelines.Get(in_id);
    }

    WNODISCARD const WVkDescriptorSetLayoutInfo & DescriptorSetLayout(const WPipelineIdType & in_id) const {
        return pipelines_db_.descriptor_set_layouts.Get(in_id);
    }

    WNODISCARD const VkDescriptorPool & DescriptorPool(const WPipelineIdType & in_id,
                                                            const std::uint32_t & in_frameindex) const {
        return pipelines_db_.descriptor_pools.at(in_frameindex).Get(in_id);
    }

    [[deprecated]] WNODISCARD
    DELETE_WVkPipelineBindingInfo const & Binding(const WBindingIdType & in_id) const {
        return pipelines_db_.bindings.Get(in_id);
    }

    WNODISCARD
    WVkPipelineBinding<FramesInFlight> const & GetBinding(WBindingIdType id) const {
        return pipelines_db_.pipe_bindings.Get(id);
    }

    template<CCallable<void, const WPipelineIdType &, WVkRenderPipeline&> TFn>
    void ForEachPipeline(TFn && in_fn) const {
        pipelines_db_.pipelines.ForEachIdValue(std::forward<TFn>(in_fn));
    }

    template<CCallable<void, const WBindingIdType &> TFn>
    void ForEachBinding(const WPipelineIdType & in_pipeline_id, TFn && in_predicate) const {
        for (const auto & wid : pipeline_bindings_.at(in_pipeline_id)) {
            std::forward<TFn>(in_predicate)(wid);
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

    WVkDescSetUBOBinding<FramesInFlight> GetUBOBinding (
        WBindingIdType binding_set_id, std::uint8_t binding
        )  const {
        for (auto & b : pipelines_db_.pipe_bindings.Get(binding_set_id).ubos) {
            if (b.binding == binding) {
                return b;
            }
        }
    }

protected:

    VkDevice Device() const {
        return device_;
    }
    
    VkPhysicalDevice PhysicalDevice() const {
        return physical_device_;
    }
    
    inline bool ValidateBindingParams(
        const wct::render::RPipeParamDescLayList & in_pipeline_params,
        const std::vector<WVkDescSetUBOWrite> & in_ubos,
        const std::vector<WVkDescSetTextureBinding> & in_textures) {

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

private:

    void Destroy() {
        if (device_ != VK_NULL_HANDLE) {
            ClearPipelinesDb();
            
            device_ = VK_NULL_HANDLE;
            physical_device_ = VK_NULL_HANDLE;
        }
    }

protected:

    VkDevice device_{VK_NULL_HANDLE};
    VkPhysicalDevice physical_device_{VK_NULL_HANDLE};

    WVkPipelinesDb<WPipelineIdType, WBindingIdType, FramesInFlight> pipelines_db_{};

    // pipline_id binding_id tracks
    std::unordered_map<WPipelineIdType, TSparseSet<WBindingIdType>> pipeline_bindings_{};

  
};
