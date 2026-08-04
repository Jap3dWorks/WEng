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
#include <algorithm>

namespace wvk::raii::pipelines {

    template<std::uint8_t FramesInFlight>
    struct Binding {

        wcr::wid::WEngId renderable_asset_id;   // matching with the render asset

        wcr::wid::WEngId descriptor_bindings_id;
        
        std::array<VkDescriptorSet, FramesInFlight> param_descriptor_set;

        std::vector<std::uint32_t> dynamic_offsets;
    };

    template<std::uint8_t FramesInFlight>
    struct DescriptorCollection {
        
        wvk::raii::DescriptorPool<
            8 * FramesInFlight,
            0,
            30 * FramesInFlight,
            38> descriptor_pool{};

        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> ubo_manager{};

        // Descriptor by each asset_params
        std::unordered_map<
            wcr::wid::WEngId,
            std::array<VkDescriptorSet, FramesInFlight>
            > identifier_descriptors;

        std::unordered_map<
            wcr::wid::WEngId::IdType,
            TSparseSet<Binding<FramesInFlight>>> pipeline_bindings{};

        void CreateBindingSet(
            wcr::wid::WEngId binding_set_id,               // from component id
            wcr::wid::WEngId pipeline_id,                  // id for the pipeline
            wcr::wid::WEngId descriptor_bindings_id,       // usually id from pipeline params
            wcr::wid::WEngId renderable_asset_id,
            wct::render::RPipeParamDescriptorsLayout const & param_descriptors,
            wct::render::RPipeParamList_Ubo const & ubo_params,
            wct::render::RPipeParamList_WAssetId const & texture_params,
            VkDescriptorSetLayout descriptor_set_layout,
            wvk::raii::AssetRenderData & asset_data
            ) {
            
            auto param_dynamic_offsets = EnsureParamDescriptorSet(
                binding_set_id,
                descriptor_bindings_id,
                param_descriptors,
                asset_data,
                descriptor_set_layout
                );

            auto param_descriptor_set = identifier_descriptors[descriptor_bindings_id];

            wvk::raii::pipelines::Binding binding {
                .renderable_asset_id=renderable_asset_id,
                .descriptor_bindings_id=descriptor_bindings_id,
                .dynamic_offsets=std::move(param_dynamic_offsets),
                .param_descriptor_set = param_descriptor_set
            };

            pipeline_bindings[
                pipeline_id
                ].Insert(binding_set_id.GetId(), std::move(binding));            
        }

        void DeleteBindingSet(
            wcr::wid::WEngId pipeline_id,
            wcr::wid::WEngId binding_set_id) {
            pipeline_bindings[pipeline_id.GetId()]
                .Remove(binding_set_id.GetId());
        }

        void UpdateBindingSetParameter(
            std::uint8_t frame_index,
            wcr::wid::WEngId render_pipeline_id,
            wcr::wid::WEngId binding_set_id,
            wct::render::RPipeParamDescriptor param_descriptor,
            wct::render::RPipeParamUbo const & ubo_data
            ) {

            wct::render::ERPipeParamType param_type = param_descriptor.type;

            assert(param_type != wct::render::ERPipeParamType::None);

            wvk::raii::pipelines::desc_bindings
                ::template UpdateParameter <
                    wvk::raii::pipelines::desc_bindings::EUpdateType::DYNAMIC
                    >
                (frame_index,
                 binding_set_id,
                 pipeline_bindings[render_pipeline_id.GetId()].Get(binding_set_id.GetId()),
                 param_descriptor,
                 ubo_data,
                 ubo_manager
                    );
        }

        void UpdateBindingSetParameter(
            wcr::wid::WEngId render_pipeline_id,
            wcr::wid::WEngId binding_set_id,
            wct::render::RPipeParamDescriptor param_layout,
            wct::render::RPipeParamUbo const & ubo_data
            ) {

            wct::render::ERPipeParamType param_type = param_layout.type;

            assert(param_type != wct::render::ERPipeParamType::None);

            wvk::raii::pipelines::desc_bindings
                ::template UpdateParameter <
                    wvk::raii::pipelines::desc_bindings::EUpdateType::STATIC
                    >
                (0,
                 param_layout,
                 pipeline_bindings[render_pipeline_id.GetId()].Get(binding_set_id.GetId()),
                 binding_set_id,
                 ubo_manager,
                 ubo_data
                    );
        }

    private:
        
        std::vector<std::uint32_t> EnsureParamDescriptorSet(
            wcr::wid::WEngId binding_set_id,
            wcr::wid::WEngId descriptor_bindings_id,
            wct::render::RPipeParamDescriptorsLayout const & param_descriptors,
            wct::render::RPipeParamList_Ubo const & ubo_param_list,
            wct::render::RPipeParamList_WAssetId const & texture_param_list,
            wvk::raii::AssetRenderData & asset_data,
            VkDescriptorSetLayout param_layout
            ) {

            auto ubo_params = wvk::raii::pipelines::desc_bindings
                ::CollectUBOBindings<FramesInFlight>(
                    binding_set_id,
                    descriptor_bindings_id,
                    param_descriptors,
                    ubo_param_list,
                    ubo_manager
                    );

            if (!identifier_descriptors.contains(descriptor_bindings_id)) {
                
                auto texture_params = wvk::raii::pipelines::desc_bindings
                    ::CollectTextureBindings(
                        texture_param_list,
                        asset_data
                        );

                auto descriptors =wvk::raii::pipelines::desc_bindings
                    ::CreateParamsDescriptorSet(
                        descriptor_pool.Creator().device,
                        param_layout,
                        descriptor_pool.Value(),
                        texture_params,
                        ubo_params
                        );

                identifier_descriptors[descriptor_bindings_id] =
                    std::move(descriptors);
            }

            std::sort(ubo_params,
                      [](auto a, auto b)
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
