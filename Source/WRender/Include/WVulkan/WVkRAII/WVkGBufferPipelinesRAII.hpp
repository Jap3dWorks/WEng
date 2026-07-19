#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WDebug.hpp"
#include "WCore/WId.hpp"
#include "WLog.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "_WVkGBufferPipelinesRAII_.hpp"

#include <vector>
#include "WVkPipelinesBase.hpp"

/**
 * @brief Graphics Pipelines outputs the GBuffers.
 */
template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkGBufferPipelinesRAII : public WVkPipelinesBase<wcr::wid::WAssetId,
                                                        wcr::wid::WEntityComponentId,
                                                        FramesInFlight>
{

public:

    using Super = WVkPipelinesBase<wcr::wid::WAssetId,
                                   wcr::wid::WEntityComponentId,
                                   FramesInFlight>;

    using Super::Super;

    WVkGBufferPipelinesRAII() noexcept=default;

    ~WVkGBufferPipelinesRAII() override = default;

    WVkGBufferPipelinesRAII(const WVkGBufferPipelinesRAII&)=delete;
    WVkGBufferPipelinesRAII & operator=(const WVkGBufferPipelinesRAII&) = delete;

    WVkGBufferPipelinesRAII(
        WVkGBufferPipelinesRAII && other
        ) noexcept =default;
    WVkGBufferPipelinesRAII & operator=(
        WVkGBufferPipelinesRAII && other
        ) noexcept =default;

    void CreatePipeline(
        const wcr::wid::WAssetId & in_id,
        const WRenderPipelineAsset & pipeline_asset,
        VkDescriptorSetLayout in_global_descset_layout
        ) {
        std::vector<WVkShaderStageInfo> shaders = Super::pipelines_db_.BuildShaders(
            pipeline_asset.Get_shader_list(),
            wvr::gbuffer_pipelines::BuildShaderStageInfo
            );

        // pipeparams
        // one Ubo (Model Ubo) should be required as first binding (0)
        Super::pipelines_db_.CreateDescSetLayout(
            in_id,
            Super::Device(),
            pipeline_asset.Get_descriptor_list(),
            wvk::descriptor::UpdateDescriptorSetLayout
            );

        Super::pipelines_db_.CreatePipeline(
            in_id,
            Super::Device(),
            in_id,
            shaders,
            [this, &pipeline_asset, in_global_descset_layout]
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
                _rp.params_descriptor = pipeline_asset.Get_descriptor_list();
            }
            );

        Super::pipelines_db_.CreateDescSetPool(
            in_id,
            Super::Device(),
            wvr::gbuffer_pipelines::CreateDescSetPool);

        Super::pipeline_bindings_[in_id] = {};
    }

    void CreateBindingSet(
        wcr::wid::WEntityComponentId binding_set_id,
        wcr::wid::WAssetId in_pipeline_id,
        wcr::wid::WTypeAssetIndexId in_mesh_asset_id,
        std::vector<WVkDescSetUBOBinding<FramesInFlight>> in_ubos,
        std::vector<WVkDescSetTextureBinding> in_textures
        ){
        WVkRenderPipeline pipeline_info = Super::Pipeline(in_pipeline_id);


        WCORE_DEBUG_ONLY_INIT
            for(auto & ubo : in_ubos) {
                WFLOG("UBO Binding: {}", ubo.binding);
            }
        WCORE_DEBUG_ONLY_END


        Super::pipelines_db_.pipe_bindings.Insert(
            binding_set_id,
            WVkPipelineBinding{
                .pipeline_id = in_pipeline_id,
                .mesh_asset_id = in_mesh_asset_id,
                .ubos = std::move(in_ubos),
                .textures = std::move(in_textures)
            }
            );

        Super::pipeline_bindings_[in_pipeline_id]
            .Insert(binding_set_id.GetId(), binding_set_id);
    }

    // [[deprecated]] wcr::wid::WEntityComponentId CreateBinding(
    //     const wcr::wid::WEntityComponentId & in_component_id,
    //     const wcr::wid::WAssetId & in_pipeline_id,
    //     const wcr::wid::WTypeAssetIndexId & in_mesh_asset_id,
    //     const std::vector<WVkDescSetUBOWrite> & in_ubos,
    //     const std::vector<WVkDescSetTextureBinding> & in_textures
    //     ) {
    //     WVkRenderPipeline pipeline_info = Super::Pipeline(in_pipeline_id);

    //     Super::pipelines_db_.bindings.InsertAt(
    //         in_component_id,
    //         DELETE_WVkPipelineBindingInfo{in_pipeline_id,
    //                                in_mesh_asset_id,
    //                                Super::InitUboDescriptorBindings(
    //                                    pipeline_info.params_descriptor,
    //                                    in_ubos),
    //                                Super::InitTextureDescriptorBindings(
    //                                    pipeline_info.params_descriptor,
    //                                    in_textures)}
    //         );

    //     Super::pipeline_bindings_[in_pipeline_id].Insert(in_component_id.GetId(), in_component_id);

    //     return in_component_id;
    // }

 };

