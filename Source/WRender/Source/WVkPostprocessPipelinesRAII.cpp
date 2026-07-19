#include "WVulkan/WVkRAII/WVkPostprocessPipelinesRAII.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVkPostprocessPipeUtils.hpp"
#include "WVulkan/WVk/WVkDescriptor.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include "WVulkan/WVk/WVkDescriptor.hpp"
// #include "WVulkan/WVk/WVkTypes.hpp"
#include "WVulkan/WVk/WVkRenderPlane.hpp"
#include <algorithm>
#include <vulkan/vulkan_core.h>


WVkPostprocessPipelinesRAII::WVkPostprocessPipelinesRAII(
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device
    ) : Super(in_device, in_physical_device) {}

WVkPostprocessPipelinesRAII::WVkPostprocessPipelinesRAII(
    WVkPostprocessPipelinesRAII && other
    ) noexcept  :
    Super(std::move(other)),
    binding_order_(std::move(other.binding_order_))
{}

WVkPostprocessPipelinesRAII & WVkPostprocessPipelinesRAII::operator=(
    WVkPostprocessPipelinesRAII && other
    ) noexcept {
    if (this != &other) {
        Super::operator=(std::move(other));

        binding_order_ = std::move(other.binding_order_);
    }

    return *this;
}


void WVkPostprocessPipelinesRAII::CreatePipeline(
    wcr::wid::WAssetId in_id,
    const WRenderPipelineAsset & in_pipeline_asset,
    VkDescriptorSetLayout in_global_descriptor,
    VkDescriptorSetLayout in_ppcess_global_descriptor
    ) {

    std::vector<WVkShaderStageInfo> shaders = pipelines_db_.BuildShaders(
        in_pipeline_asset.Get_shader_list(),
        WVkPostprocessPipeUtils::BuildPostprocessShaderStageInfo
        );
    
    pipelines_db_.CreateDescSetLayout(
        in_id,
        device_,
        in_pipeline_asset.Get_descriptor_list(),
        wvk::descriptor::UpdateDescriptorSetLayout
        );

    pipelines_db_.CreatePipeline(
        in_id,
        device_,
        in_id,
        shaders,
        [this, &in_pipeline_asset, &in_global_descriptor, &in_ppcess_global_descriptor]
        (auto& _rp, const auto & _dvc, const auto & _desclay, const auto & _shdrs) {
            WVkPostprocessPipeUtils::CreatePostprocessPipeline(
                _rp,
                _dvc,
                {
                    in_global_descriptor,
                    _desclay.descset_layout,
                    in_ppcess_global_descriptor
                },
                _shdrs
                );

            _rp.params_descriptor = in_pipeline_asset.Get_descriptor_list();
        }
        );

    pipelines_db_.CreateDescSetPool(
        in_id, device_,
        WVkPostprocessPipeUtils::CreateDescSetPool
        );

    pipeline_bindings_[in_id] = {};
}

// wcr::wid::WEntityComponentId WVkPostprocessPipelinesRAII::CreateBinding(
//     const wcr::wid::WEntityComponentId & in_binding_id,
//     const wcr::wid::WAssetId & in_pipeline_id,
//     const std::vector<WVkDescSetUBOWrite> & in_ubos,
//     const std::vector<WVkDescSetTextureBinding> & in_texture
//     ) {

//     WVkRenderPipeline pipeline_info = Pipeline(in_pipeline_id);

//     pipelines_db_.bindings.InsertAt(
//         in_binding_id,
//         DELETE_WVkPipelineBindingInfo{
//             in_pipeline_id,
//             {},
//                 InitUboDescriptorBindings(pipeline_info.params_descriptor,
//                                           in_ubos),
//                 InitTextureDescriptorBindings(pipeline_info.params_descriptor,
//                                               in_texture)}
//         );

//     pipeline_bindings_[in_pipeline_id].Insert(in_binding_id.GetId(), in_binding_id);

//     return in_binding_id;
// }

void WVkPostprocessPipelinesRAII::ComputeBindingOrder() {
    binding_order_.clear();
    binding_order_.resize(pipelines_db_.pipe_bindings.Count());

    std::uint32_t idx = 0;
    for(const auto & bid : pipelines_db_.pipe_bindings.IterIndexes()) {
        binding_order_[idx] = bid;
        idx++;
    }

    std::sort(binding_order_.begin(), binding_order_.end());
}

