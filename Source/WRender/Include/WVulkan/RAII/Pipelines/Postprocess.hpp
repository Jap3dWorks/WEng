#pragma once

#include "WCore/TIterator.hpp"
#include "WCore/WCore.hpp"
#include "WCore/WId.hpp"
#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/RAII/Pipelines/WVkPipelinesBase.hpp"
#include "WAssets/RenderPipeline.hpp"
#include <vulkan/vulkan_core.h>

// TODO template with max frames in flight as param

namespace wvk::raii::pipelines {

    class Postprocess :
        public WVkPipelinesBase<wcr::wid::WAssetId,
                                wcr::wid::WEntityComponentId,
                                WVK_MAX_FRAMES_IN_FLIGHT>
    {
    public:
        using Super =
            WVkPipelinesBase<wcr::wid::WAssetId, wcr::wid::WEntityComponentId, frames_in_flight>;

    public:

        Postprocess()=default;

        Postprocess(const VkDevice & in_device,
                                    const VkPhysicalDevice & in_physical_device);

        virtual ~Postprocess()=default; 

        Postprocess(const Postprocess &)=delete;
        Postprocess & operator=(const Postprocess &)=delete;

        Postprocess(Postprocess && other) noexcept;

        Postprocess & operator=(Postprocess && other) noexcept;

        void CreatePipeline(
            wcr::wid::WAssetId in_id,
            const was::RenderPipeline & in_pipeline_struct,
            VkDescriptorSetLayout in_global_descriptor,
            VkDescriptorSetLayout in_ppcess_global_descriptor
            );

        void CreateBindingSet(
            wcr::wid::WEntityComponentId binding_set_id,
            wcr::wid::WAssetId in_pipeline_id,
            // wcr::wid::WTypeAssetIndexId in_mesh_asset_id,
            std::vector<WVkDescSetUBOBinding<frames_in_flight>> in_ubos,
            std::vector<WVkDescSetTextureBinding> in_textures
            ){
            WVkRenderPipeline pipeline_info = Super::Pipeline(in_pipeline_id);

            Super::pipelines_db_.pipe_bindings.Insert(
                binding_set_id,
                WVkPipelineBinding{
                    .pipeline_id = in_pipeline_id,
                    .mesh_asset_id = wcr::wid::nullid,
                    .ubos = std::move(in_ubos),
                    .textures = std::move(in_textures)
                }
                );

            Super::pipeline_bindings_[in_pipeline_id]
                .Insert(binding_set_id.GetId(), binding_set_id);
        }

        /**
         * @Brief Computes pipeline bindings order, order is relevant because 
         * in a postprocess stack each postprocess is applied over the previous postprocess.
         * This function is required to be called at least once.
         */
        void ComputeBindingOrder();

        auto BindingOrderIterator() const {
            return WIteratorUtils::DefaultIteratorPtr<const wcr::wid::WEntityComponentId>(
                &(*binding_order_.begin()),
                &(*binding_order_.end())
                );
        }

    private:

        std::vector<wcr::wid::WEntityComponentId> binding_order_{};

    };


}
