#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "_WVKGBufferPipelinesRAII_.hpp"

#include <vector>
#include "WVkPipelinesBase.hpp"

/**
 * @brief Graphics Pipelines outputs the GBuffers.
 */
template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkGBufferPipelinesRAII : public WVkPipelinesBase<WAssetId,
                                                        WEntityComponentId,
                                                        FramesInFlight>
{

public:

    using Super = WVkPipelinesBase<WAssetId,
                                   WEntityComponentId,
                                   FramesInFlight>;

    WVkGBufferPipelinesRAII() noexcept=default;

    WVkGBufferPipelinesRAII(
        const VkDevice & in_device,
        const VkPhysicalDevice & in_physical_device
        ) : Super(in_device, in_physical_device) {
    }

    virtual ~WVkGBufferPipelinesRAII() override {
        Destroy();
        Super::~Super();
    }

    WVkGBufferPipelinesRAII(const WVkGBufferPipelinesRAII & other)=delete;

    WVkGBufferPipelinesRAII & operator=(
        const WVkGBufferPipelinesRAII & other
        ) = delete;

    WVkGBufferPipelinesRAII(
        WVkGBufferPipelinesRAII && other
        ) noexcept :
        Super(std::move(other)) {}

    WVkGBufferPipelinesRAII & operator=(
        WVkGBufferPipelinesRAII && other
        ) noexcept {
        if (this != &other) {
            Destroy();
            Super::operator=(std::move(other));
        }

        return *this;
    }

    void CreatePipeline(
        const WAssetId & in_id,
        const WRenderPipelineAsset & in_pipeline_struct,
        VkDescriptorSetLayout in_global_descset_layout
        ) {
        std::vector<WVkShaderStageInfo> shaders = Super::pipelines_db_.BuildShaders(
            in_pipeline_struct.Get_shader_list(),
            wvr::gbuffer_pipelines::BuildShaderStageInfo
            );

        // pipeparams
        // one Ubo should be required as first binding (0)
        Super::pipelines_db_.CreateDescSetLayout(
            in_id,
            Super::Device(),
            in_pipeline_struct.Get_descriptor_list(),
            // in_pipeline_struct.params_descriptor,
            wvk::descriptor::UpdateDescriptorSetLayout
            );

        Super::pipelines_db_.CreatePipeline(
            in_id,
            Super::Device(),
            in_id,
            shaders,
            [this, &in_pipeline_struct, in_global_descset_layout]
            (auto& _rp, const auto &_dvc, const auto &_desclay, const auto & _shdrs) {

                wvr::gbuffer_pipelines::CreatePipeline(
                    _rp,
                    _dvc,
                    {
                        in_global_descset_layout,
                        _desclay.descset_layout
                    },
                    _shdrs
                    );
                _rp.params_descriptor = in_pipeline_struct.Get_descriptor_list();
                // _rp.params_descriptor = in_pipeline_struct.params_descriptor;
            }
            );

        Super::pipelines_db_.CreateDescSetPool(
            in_id,
            Super::Device(),
            wvr::gbuffer_pipelines::CreateDescSetPool);

        Super::pipeline_bindings_[in_id] = {};
    }

public:

    WEntityComponentId CreateBinding(
        const WEntityComponentId & in_component_id,
        const WAssetId & in_pipeline_id,
        const WAssetIndexId & in_mesh_asset_id,
        const std::vector<WVkDescriptorSetUBOWriteStruct> & in_ubos,
        const std::vector<WVkDescriptorSetTextureWriteStruct> & in_textures
        ) {
        WVkRenderPipelineInfo pipeline_info = Super::Pipeline(in_pipeline_id);

        Super::pipelines_db_.bindings.InsertAt(
            in_component_id,
            WVkPipelineBindingInfo{in_pipeline_id,
                                   in_mesh_asset_id,
                                   Super::InitUboDescriptorBindings(
                                       pipeline_info.params_descriptor,
                                       in_ubos),
                                   Super::InitTextureDescriptorBindings(
                                       pipeline_info.params_descriptor,
                                       in_textures)}
            );

        Super::pipeline_bindings_[in_pipeline_id].Insert(in_component_id.GetId(), in_component_id);

        return in_component_id;
    }

private:

    void Destroy() {}

 };

