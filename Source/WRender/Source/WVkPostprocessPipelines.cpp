#include "WVulkan/WVkPostprocessPipelines.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WVkPostprocessPipeUtils.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVulkanUtils.hpp"
#include <algorithm>
#include <vulkan/vulkan_core.h>

void WVkPostprocessPipelines::Initialize(const VkDevice & in_device,
                                         const VkPhysicalDevice & in_physical_device) {
    Super::Initialize(in_device, in_physical_device);
    Initialize_GlobalResources();
}

void WVkPostprocessPipelines::CreatePipeline(const WAssetId & in_id,
                                             const WRenderPipelineStruct & in_pipeline_struct) {

    std::vector<WVkShaderStageInfo> shaders = pipelines_db_.BuildShaders(
        in_pipeline_struct.shaders,
        WVkPostprocessPipeUtils::BuildPostprocessShaderStageInfo
        );
    
    // One Ubo should be required as first binding (0)
    pipelines_db_.CreateDescSetLayout(
        in_id,
        device_,
        in_pipeline_struct.params_descriptor,
        WVulkanUtils::UpdateDescriptorSetLayout
        );

    pipelines_db_.CreatePipeline(
        in_id,
        device_,
        in_id,
        shaders,
        [this, &in_pipeline_struct]
        (auto& _rp, const auto & _dvc, const auto & _desclay, const auto & _shdrs) {
            WVkPostprocessPipeUtils::CreatePostprocessPipeline(
                _rp,
                _dvc,
                {
                    global_resources_.descset_layout_info.descset_layout,
                    _desclay.descset_layout
                },
                _shdrs
                );

            _rp.params_descriptor = in_pipeline_struct.params_descriptor;
        }
        );

    pipelines_db_.CreateDescSetPool(
        in_id, device_,
        WVkPostprocessPipeUtils::CreateDescSetPool
        );

    pipeline_bindings_[in_id] = {};
}

WEntityComponentId WVkPostprocessPipelines::CreateBinding(
    const WEntityComponentId & in_binding_id,
    const WAssetId & in_pipeline_id,
    const std::vector<WVkDescriptorSetUBOWriteStruct> & in_ubos,
    const std::vector<WVkDescriptorSetTextureWriteStruct> & in_texture
    ) {

    WVkRenderPipelineInfo pipeline_info = Pipeline(in_pipeline_id);

    pipelines_db_.bindings.InsertAt(
        in_binding_id,
        WVkPipelineBindingInfo{
            in_pipeline_id,
            {},
                InitUboDescriptorBindings(pipeline_info.params_descriptor,
                                          in_ubos),
                InitTextureDescriptorBindings(pipeline_info.params_descriptor,
                                              in_texture)}
        );

    pipeline_bindings_[in_pipeline_id].Insert(in_binding_id.GetId(), in_binding_id);

    return in_binding_id;
}

void WVkPostprocessPipelines::Destroy() {
    if(device_ != VK_NULL_HANDLE) {
        ClearPipelinesDb();

        Destroy_GlobalResources();

        device_=VK_NULL_HANDLE;
        physical_device_ = VK_NULL_HANDLE;
    }

}

void WVkPostprocessPipelines::CalcBindingOrder() {
    binding_order_.clear();
    binding_order_.resize(pipelines_db_.bindings.Count());

    std::uint32_t idx = 0;
    for(const auto & bid : pipelines_db_.bindings.IterIndexes()) {
        binding_order_[idx] = bid;
        idx++;
    }

    std::sort(binding_order_.begin(), binding_order_.end());
}

void WVkPostprocessPipelines::Initialize_GlobalResources() {
    auto plane_vertex = WVulkanUtils::RenderPlaneVertex();
    auto plane_index = WVulkanUtils::RenderPlaneIndexes();

    global_resources_.descset_layout_info = {};
    WVkPostprocessPipeUtils::UpdateDSL_DefaultGlobalBindings(
        global_resources_.descset_layout_info
        );

    WVulkan::Create(
        global_resources_.descset_layout_info,
        device_
        );

    for(auto & descpool : global_resources_.descpool_info) {
        descpool = {};
        WVkPostprocessPipeUtils::CreateGlobalResourcesDescPool(
            descpool,
            device_
            );
    }

}

void WVkPostprocessPipelines::Destroy_GlobalResources() {

    WVulkan::Destroy(
        global_resources_.descset_layout_info,
        device_
        );

    for(auto & descpool : global_resources_.descpool_info) {
        WVulkan::Destroy(descpool, device_);
    }
}
