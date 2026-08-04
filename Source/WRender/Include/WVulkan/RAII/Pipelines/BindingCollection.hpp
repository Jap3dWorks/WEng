#pragma once

#include "WAssets/RenderPipeline.hpp"
#include "WAssets/RenderPipelineParams.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/DescriptorPool.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WVulkan/RAII/Pipelines/DescriptorBindings.hpp"
#include "WVulkan/RAII/UBOManager/DynamicUBOManager.hpp"
#include "WVulkan/RAII/Pipelines/_new_GBuffer_lib.hpp"

#include <cstdint>

namespace wvk::raii::pipelines {

    template<std::uint8_t FramesInFlight>
    struct Binding {
        

        wcr::wid::WTypeAssetIndexId mesh_id;

        std::array<VkDescriptorSet, FramesInFlight> model_ubo_descriptor_set;

        wcr::wid::WAssetId param_descriptor_set_id;
        
        std::vector<std::uint32_t> dynamic_offsets;

        std::array<VkDescriptorSet, FramesInFlight> param_descripor_set;
    };

    template<std::uint8_t FramesInFlight>
    struct DescriptorCollection {
        
        wvk::raii::DescriptorPool<
            8 * FramesInFlight,
            0,
            30 * FramesInFlight,
            38> descriptor_pool{};

        std::array<VkDescriptorSet, FramesInFlight> model_ubo_desc_set{VK_NULL_HANDLE};

        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> ubo_manager{};

        // Descriptor by each asset_params
        std::unordered_map<wcr::wid::WAssetId,
                           std::array<VkDescriptorSet, FramesInFlight>> param_descriptors;

        std::unordered_map<
            wcr::wid::WAssetId::IdType,
            TSparseSet<Binding<FramesInFlight>>> pipeline_bindings{};

        void CreateBindingSet(
            wcr::wid::WEngId binding_set_id,
            wcr::wid::WEngId model_ubo_id,
            VkDescriptorSetLayout model_ubo_layout,
            std::uint8_t model_ubo_binding,
            was::RenderPipeline asset_pipeline,
            was::RenderPipelineParams asset_params,
            VkDescriptorSetLayout param_ds_layout,
            wvk::raii::AssetRenderData & asset_data,
            wcr::wid::WTypeAssetIndexId in_mesh_asset_id
            ) {
            
            EnsureModelUboDescriptorSet(
                model_ubo_id,
                model_ubo_layout,
                model_ubo_binding
                );

            auto param_dynamic_offsets = EnsureParamDescriptorSet(
                binding_set_id,
                asset_pipeline,
                asset_params,
                asset_data,
                param_ds_layout
                );

            std::vector<std::uint32_t> dyn_offsets = {
                ubo_manager
                .GetOffset<FramesInFlight>(
                    sizeof(wct::render::ModelUBO),
                    binding_set_id)
            };
            
            dyn_offsets.append_range(std::move(param_dynamic_offsets));

            auto param_descriptor_set = param_descriptors[asset_params.Get_asset_id()];

            wvk::raii::pipelines::Binding binding {
                .mesh_id=in_mesh_asset_id,
                .model_ubo_descriptor_set = model_ubo_desc_set,
                .param_descriptor_set_id=asset_params.Get_asset_id(),
                .dynamic_offsets=std::move(dyn_offsets),
                .param_descriptor_set = param_descriptor_set
            };

            pipeline_bindings[
                asset_pipeline.Get_asset_id().GetId()
                ].Insert(binding_set_id.GetId(), std::move(binding));            
        }

        void DeleteBindingSet(
            was::RenderPipeline const & render_pipeline,
            wcr::wid::WEngId binding_set_id) {
            pipeline_bindings[render_pipeline.Get_asset_id().GetId()]
                .Remove(binding_set_id.GetId());
        }

        void UpdateBindingSetParameter(
            std::uint8_t frame_index,
            wct::render::RPipeParamDescriptor param_layout,
            wcr::wid::WAssetId pipeline_params_id,
            wcr::wid::WEngId binding_set_id,
            wct::render::RPipeParamUbo const & ubo_pipe_param
            ) {

            wct::render::ERPipeParamType param_type = param_layout.type;

            assert(param_type != wct::render::ERPipeParamType::None);

            wvk::raii::pipelines::desc_bindings
                ::template UpdateParameter
                <wvk::raii::pipelines::desc_bindings::EUpdateType::DYNAMIC>
                (frame_index,
                 param_layout,
                 pipeline_params_id,
                 binding_set_id,
                 ubo_manager,
                 ubo_pipe_param
                    );
        }

        void UpdateBindingSetParameter(
            wct::render::RPipeParamDescriptor param_layout,
            wcr::wid::WAssetId pipeline_params_id,
            wcr::wid::WEngId binding_set_id,
            wct::render::RPipeParamUbo const & ubo_pipe_param
            ) {

            wct::render::ERPipeParamType param_type = param_layout.type;

            assert(param_type != wct::render::ERPipeParamType::None);

            wvk::raii::pipelines::desc_bindings
                ::template UpdateParameter
                <wvk::raii::pipelines::desc_bindings::EUpdateType::STATIC>
                (0,
                 param_layout,
                 pipeline_params_id,
                 binding_set_id,
                 ubo_manager,
                 ubo_pipe_param
                    );
        }

    private:
        
        void EnsureModelUboDescriptorSet(
            wcr::wid::WEngId model_ubo_id,
            VkDescriptorSetLayout model_ubo_layout,
            std::uint8_t model_ubo_binding
            ) {
            
            VkDescriptorSet model_ubo_descriptor;

            if(model_ubo_desc_set[0] == VK_NULL_HANDLE) {

                auto descriptors =
                    wvk::raii::pipelines::gbuffer_lib::
                    CreateModelUboDescriptorSet(
                        descriptor_pool.Creator().device,
                        model_ubo_id,
                        model_ubo_binding,
                        ubo_manager,
                        *descriptor_pool,
                        model_ubo_layout
                        );

                model_ubo_desc_set = descriptors;
            }
        }

        std::vector<std::uint32_t> EnsureParamDescriptorSet(
            wcr::wid::WEngId binding_set_id,
            was::RenderPipeline const & asset_pipeline,
            was::RenderPipelineParams const & asset_params,
            wvk::raii::AssetRenderData & asset_data,
            VkDescriptorSetLayout param_layout
            ) {

            auto ubo_params = wvk::raii::pipelines::desc_bindings
                ::CollectUBOBindings<FramesInFlight>(
                    binding_set_id,
                    asset_pipeline,
                    asset_params,
                    ubo_manager
                    );

            if (!param_descriptors.contains(
                    asset_params.Get_asset_id())) {
                
                auto texture_params = wvk::raii::pipelines::desc_bindings
                    ::CollectTextureBindings(
                        asset_params,
                        asset_data
                        );

                auto descriptors =wvk::raii::pipelines::desc_bindings
                    ::CreateParamsDescriptorSet(
                        descriptor_pool.Creator().device,
                        param_layout,
                        descriptor_pool,
                        texture_params,
                        ubo_params
                        );

                param_descriptors[asset_params.Get_asset_id()] =
                    std::move(descriptors);
            }

            std::sort(ubo_params, [](auto a, auto b)
                {
                    return a.binding < b.binding;
                }
                );

            std::vector<std::uint32_t> dynamic_offsets;

            std::ranges::transform(ubo_params,
                                   std::back_inserter(dynamic_offsets),
                                   [](auto b) {return b.dynamic_offset; });

            return dynamic_offsets;
        }       

        
    };


}
