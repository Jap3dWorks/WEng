#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WDebug.hpp"
#include "WCore/WId.hpp"
#include "WLog.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WAssets/RenderPipeline.hpp"
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
        const wcr::wid::WAssetId & pipeline_id,
        const was::RenderPipeline & pipeline_asset,
        VkDescriptorSetLayout global_descset_layout
        ) {
        std::vector<WVkShaderStageInfo> shaders = Super::pipelines_db_.BuildShaders(
            pipeline_asset.Get_shader_list(),
            wvr::gbuffer_pipelines::BuildShaderStageInfo
            );

        // pipeparams
        // one Ubo (Model Ubo) should be required as first binding (0)
        Super::pipelines_db_.CreateDescSetLayout(
            pipeline_id,
            Super::Device(),
            pipeline_asset.Get_descriptor_list(),
            wvk::descriptor::UpdateDescriptorSetLayout
            );

        Super::pipelines_db_.CreatePipeline(
            pipeline_id,
            Super::Device(),
            pipeline_id,
            shaders,
            [this, &pipeline_asset, global_descset_layout]
            (auto& _out_rp, auto const &_dvc, auto const &_desclay, auto const & _shdrs) {

                wvr::gbuffer_pipelines::CreatePipeline(
                    _out_rp,
                    _dvc,
                    {
                        global_descset_layout,
                        _desclay.descset_layout
                    },
                    _shdrs
                    );
                // _out_rp.params_descriptor = pipeline_asset.Get_descriptor_list();
            }
            );

        Super::pipelines_db_.CreateDescSetPool(
            pipeline_id,
            Super::Device(),
            wvr::gbuffer_pipelines::CreateDescSetPool);

        Super::pipeline_bindings_[pipeline_id] = {};
    }

    void CreateBindingSet(
        wcr::wid::WEntityComponentId binding_set_id,
        wcr::wid::WAssetId in_pipeline_id,
        wcr::wid::WTypeAssetIndexId in_mesh_asset_id,
        std::vector<WVkDescSetUBOBinding<FramesInFlight>> in_ubos,
        std::vector<WVkDescSetTextureBinding> in_textures
        ){
        WVkRenderPipeline pipeline_info = Super::Pipeline(in_pipeline_id);

        WCORE_DEBUG_ONLY(
            for(auto & ubo : in_ubos) {
                WFLOG("UBO Binding: {}", ubo.binding);
            }
            )

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

 };

