#pragma once

#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/DescriptorPool.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WVulkan/RAII/Pipelines/DescriptorBindings.hpp"
#include "WVulkan/RAII/UBOManager/DynamicUBOManager.hpp"

#include <cstdint>
#include <algorithm>
#include <array>

namespace wvk::raii::pipelines::gbuffer {

    template<std::uint8_t FramesInFlight>
    struct Binding {

        wcr::wid::WEngId binding_set_id;

        wcr::wid::WEngId renderable_asset_id;     // matching with the render asset

        wcr::wid::WEngId descriptor_bindings_id;  // usually matches with the render param asset id
        
        std::array<VkDescriptorSet, FramesInFlight> descriptor_set;

        std::vector<std::uint32_t> dynamic_offsets;
    };

    template<std::uint8_t FramesInFlight>
    struct BindingCollection {
        
        wvk::raii::DescriptorPool<
            8 * FramesInFlight,
            0,
            30 * FramesInFlight,
            112> descriptor_pool{};

        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight>
        dynamic_ubo_manager{};

        // Descriptor by each asset_params
        std::unordered_map<
            wcr::wid::WEngId,
            std::array<VkDescriptorSet, FramesInFlight>
            > identifier_descriptors{};

        std::unordered_map<
            wcr::wid::WEngId::IdType,         // Pipeline id
            TSparseSet<Binding<FramesInFlight>>
            > pipeline_bindings{};
        // TODO : could pipeline_bindings be a simpler container? like a DenseVector.

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
            
            auto dynamic_offsets = EnsureParamDescriptorSet(
                binding_set_id,
                descriptor_bindings_id,
                param_descriptors,
                ubo_params,
                texture_params,
                descriptor_set_layout,
                asset_data
                );

            auto param_descriptor_set = identifier_descriptors[descriptor_bindings_id];

            wvk::raii::pipelines::gbuffer::Binding<FramesInFlight> binding {
                .binding_set_id=binding_set_id,
                .renderable_asset_id=renderable_asset_id,
                .descriptor_bindings_id=descriptor_bindings_id,
                .descriptor_set = param_descriptor_set,
                .dynamic_offsets=std::move(dynamic_offsets)
            };

            pipeline_bindings[
                pipeline_id.GetId()
                ].Insert(binding_set_id.GetId(), std::move(binding));

        }

        void DeleteBindingSet(
            wcr::wid::WEngId pipeline_id,
            wcr::wid::WEngId binding_set_id
            ) {
            assert(pipeline_bindings.contains(pipeline_id.GetId()));
            
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
                 pipeline_bindings[render_pipeline_id.GetId()]
                     .Get(binding_set_id.GetId()).descriptor_bindings_id,
                 param_descriptor,
                 ubo_data,
                 dynamic_ubo_manager
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
                 // param_layout,
                 binding_set_id,
                 pipeline_bindings[render_pipeline_id.GetId()]
                     .Get(binding_set_id.GetId()).renderable_asset_id,
                 param_layout,
                 ubo_data,
                 dynamic_ubo_manager
                    );
        }

    private:
        
        std::vector<std::uint32_t> EnsureParamDescriptorSet(
            wcr::wid::WEngId binding_set_id,
            wcr::wid::WEngId descriptor_bindings_id,
            wct::render::RPipeParamDescriptorsLayout const & param_descriptors,
            wct::render::RPipeParamList_Ubo const & ubo_param_list,
            wct::render::RPipeParamList_WAssetId const & texture_param_list,
            VkDescriptorSetLayout param_layout,
            wvk::raii::AssetRenderData & asset_data
            ) {

            auto ubo_params = wvk::raii::pipelines::desc_bindings
                ::CollectUBOBindings_Dynamic<FramesInFlight>(
                    binding_set_id,
                    descriptor_bindings_id,
                    param_descriptors,
                    ubo_param_list,
                    dynamic_ubo_manager
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

            std::sort(ubo_params.begin(), ubo_params.end(),
                      [](auto const & a, auto const & b)
                          {
                              return a.binding < b.binding;
                          }
                );

            std::vector<std::uint32_t> dynamic_offsets;

            std::ranges::transform(ubo_params,
                                   std::back_inserter(dynamic_offsets),
                                   [](auto b) {return b.offset; });

            return dynamic_offsets;
        }
    };

    template<std::uint8_t FramesInFlight>
    using BindingCollectionsMap = std::unordered_map<
        std::size_t,
        BindingCollection<FramesInFlight>>;

    template<std::uint8_t FramesInFlight, typename ValueFn, typename IncrFn>
    using BindingCollectionsIterator = ::TIterator<
        typename BindingCollectionsMap<FramesInFlight>::value_type const,
        typename BindingCollectionsMap<FramesInFlight>::const_iterator,
        std::size_t, ValueFn, IncrFn
        >;


    template<std::uint8_t FramesInFlight>
    inline constexpr auto IterCollectionsIds(
        BindingCollectionsMap<FramesInFlight> const & collections_map
        ) {
        auto valueFn = [](auto it, std::size_t incr) { return (*it).first; };
        auto incrFn  = [](auto it, std::size_t incr) { it++; return it; };

        using ValueFn_t = decltype(valueFn);
        using IncrFn_t  = decltype(incrFn);

        return BindingCollectionsIterator<
            FramesInFlight, ValueFn_t, IncrFn_t
            >
            {
                collections_map.cbegin(),
                collections_map.cend(),
                std::move(valueFn),
                std::move(incrFn)
            };

    }

}
