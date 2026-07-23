#pragma once

#include "WCore/WId.hpp"
#include "WVulkan/WVkRAII/WVkGBufferPipelinesRAII.hpp"
#include "WVulkan/WVkRAII/WVkPostprocessPipelinesRAII.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVkRAII/WVkAssetRenderDataRAII.hpp"
#include "WCore/TVisitor.hpp"

#include "WAssets/RenderPipelineParams.hpp"
#include "WAssets/RenderPipeline.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/WVk/WVkBuffer.hpp"

#include <ranges>
#include <iterator>
#include <algorithm>

namespace wvk::render::pipe_bindings {

    WNODISCARD inline std::vector<WVkDescSetTextureBinding> CollectTextureBindings(
        was::RenderPipelineParams const & parameters,
        WVkAssetRenderDataRAII & asset_render_data
        ) {
        auto texture_params = parameters.Get_texture_list();

        std::vector<WVkDescSetTextureBinding> textures{};
        textures.resize(texture_params.size());

        for(std::uint32_t i=0; i < texture_params.size(); i++) {
            const auto & tx = asset_render_data.TextureInfo(
                texture_params[i].value
                );
        
            textures[i] = {
                .binding = texture_params[i].binding,
                .image_info = {
                    .sampler = tx.sampler,
                    .imageView = tx.view,
                    .imageLayout = tx.layout
                }
            };
        }

        return textures;
    }

    template<std::uint8_t FramesInFlight>
    WNODISCARD std::vector<WVkDescSetUBOBinding<FramesInFlight>> CollectUBOBindings(
        wcr::wid::WEntityComponentId entity_component_id,
        was::RenderPipeline const & pipeline,
        was::RenderPipelineParams const & parameters,
        WVkAssetRenderDataRAII & asset_render_data        
        ) {

        auto & ubo_params = parameters.Get_ubo_list();

        std::unordered_map<
            std::uint8_t,
            wct::render::RPipeParamUbo const *> binding_param{};

        for (auto & ubo : ubo_params) {
            binding_param.insert(
                {ubo.binding, &ubo}
                );
        }

        auto get_entity_id = [](wcr::wid::WEntityComponentId component_id) {
            wcr::wid::WAssetId level;
            wcr::wid::WEntityId entity;
            wcr::wid::WComponentTypeId component;
            wcr::wid::WSubIdxId indx;

            component_id.ExtractWIds(level, entity, component, indx);

            return wcr::wid::WEntityComponentId {level,
                                                 entity,
                                                 wcr::wid::null_id,
                                                 wcr::wid::null_id};
        };

        auto get_ubo_data_ptr = [](wct::render::RPipeParamUbo const * ubo_param) -> void const * {
            return std::visit<void const *> (
                wcr::TVisitor([](auto const& data) -> void const * {
                    return static_cast<void const *>(data.data());
                }),
                ubo_param->data
                );
        };        

        auto CreateUBOBinding =
            [&]
            <std::uint8_t UBOs>
            (wct::render::RPipeParamDescLayInfo const & desc, wcr::wid::WEngId wid)
            -> WVkDescSetUBOBinding<FramesInFlight>
            {

                std::array<std::size_t, UBOs> indexes{};

                if (!asset_render_data.ContainsUBOs(wid)) {

                    void const * ptr = binding_param.contains(desc.binding)
                        ? get_ubo_data_ptr(binding_param[desc.binding])
                        : nullptr;

                    for (std::uint8_t i=0; i<UBOs; i++) {

                        indexes[i] = asset_render_data.CreateUBO(wid,
                                                                 desc.size,
                                                                 ptr);
                    }
                }
                else {
                    std::ranges::copy(
                        asset_render_data.GetUBOs(wid) | std::views::take(indexes.size()),
                        indexes.begin());
                }

                WVkDescSetUBOBinding<FramesInFlight> result{};
                result.binding = desc.binding;

                for (std::size_t i=0; i<FramesInFlight; i++) {
                    std::size_t minidx = std::min(i, indexes.size()-1);
                    WVkUBO const & ubo = asset_render_data.GetUBO(indexes[minidx]);

                    

                    result.ubo_desc[i] = {
                        .index = indexes[minidx],
                        .desc_buffer = {
                            .buffer = ubo.buffer,
                            .offset = 0,
                            .range = ubo.range
                        }
                    };
                }

                return result;
            };

        std::vector<WVkDescSetUBOBinding<FramesInFlight>> result{};
        result.reserve(ubo_params.size());

        for (auto & descriptor : pipeline.Get_descriptor_list()) {

            switch(descriptor.type) {

            case wct::render::ERPipeParamType::UBO_Static:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<1> (
                        descriptor,
                        wcr::wid::WEngId::FromAsset(
                            wcr::wid::WTypeAssetIndexId(
                                wcr::wid::null_id,
                                parameters.Get_asset_id(),
                                descriptor.binding
                                ))
                        )
                    );
                
                break;

            case wct::render::ERPipeParamType::UBO_Dynamic:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<FramesInFlight> (
                        descriptor,
                        wcr::wid::WEngId::FromAsset(
                            wcr::wid::WTypeAssetIndexId(
                                wcr::wid::null_id,
                                parameters.Get_asset_id(),
                                descriptor.binding
                                ))
                        )
                    );

                break;

            case wct::render::ERPipeParamType::UBO_Entity_Static: 
                
                result.push_back(
                    CreateUBOBinding
                    .template operator()<1> (
                        descriptor,
                        wcr::wid::WEngId::FromEntityComponent(
                            get_entity_id(entity_component_id)
                            )
                        )
                    );

                break;

            case wct::render::ERPipeParamType::UBO_Entity_Dynamic:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<FramesInFlight> (
                        descriptor,
                        wcr::wid::WEngId::FromEntityComponent(
                            get_entity_id(entity_component_id)
                            )
                        )
                    );

                break;
                
            case wct::render::ERPipeParamType::UBO_Component_Static:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<1> (
                        descriptor,
                        wcr::wid::WEngId::FromEntityComponent(
                            entity_component_id
                            )
                        )
                    );

                break;

            case wct::render::ERPipeParamType::UBO_Component_Dynamic:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<FramesInFlight> (
                        descriptor,
                        wcr::wid::WEngId::FromEntityComponent(
                            entity_component_id
                            )
                        )
                    );

                break;

            default:
                continue;
                
            }
        }

        return result;
    }

    template<std::uint8_t FramesInFlight>
    inline void CreateBindingSet(
        wct::render::ERPipeType pipeline_type,
        wcr::wid::WEntityComponentId binding_set_id,
        wcr::wid::WTypeAssetIndexId renderable_asset_id,
        wcr::wid::WAssetId pipeline_id,
        WVkGBufferPipelinesRAII<FramesInFlight> & gbuffers_pipelines,
        WVkPostprocessPipelinesRAII & postprocess_pipelines,
        std::vector<WVkDescSetUBOBinding<FramesInFlight>> ubo_bindings,
        std::vector<WVkDescSetTextureBinding> texture_bindings
        ) {
        switch(pipeline_type) {
        case wct::render::ERPipeType::Postprocess:
            postprocess_pipelines.CreateBindingSet(
                binding_set_id,
                pipeline_id,
                std::move(ubo_bindings),
                std::move(texture_bindings)
                );
            break;

        default:
            gbuffers_pipelines.CreateBindingSet(
                binding_set_id,
                pipeline_id,
                renderable_asset_id,
                ubo_bindings,
                texture_bindings
                );
        }
    }

    template<std::uint8_t FramesInFlight>
    inline WVkDescSetUBOBinding<FramesInFlight> GetUboBinding(
        wcr::wid::WEntityComponentId binding_set_id,
        std::uint8_t binding,
        wct::render::ERPipeType pipe_type,
        WVkGBufferPipelinesRAII<FramesInFlight> const & gbuffers_pipelines,
        WVkPostprocessPipelinesRAII const & postprocess_pipelines
        ) {
        switch(pipe_type) {
        case wct::render::ERPipeType::Postprocess:
            return postprocess_pipelines
                .GetUBOBinding(binding_set_id, binding);
            break;
        default:
            return gbuffers_pipelines
                .GetUBOBinding(binding_set_id, binding);
        }

    }

    inline WVkDescSetUBOWrite GetUboWrite(wct::render::RPipeParamUbo const & ubo_pipe_param) {
        return std::visit(
            wcr::TVisitor(
                [&ubo_pipe_param](auto const & ubodata) -> WVkDescSetUBOWrite {
                    return {
                        .binding = ubo_pipe_param.binding,
                        .data = ubodata.data(),
                        .size = ubodata.size(),
                        .offset = ubo_pipe_param.offset
                    };
                }
                ),
            ubo_pipe_param.data
            );
    }

    template<std::uint8_t FramesInFlight>
    inline void UpdateParamStatic(
        WVkDescSetUBOBinding<FramesInFlight> & ubo_binding,
        WVkDescSetUBOWrite & ubo_write,
        WVkAssetRenderDataRAII & asset_render_data,
        VkDevice device
        ) {

        auto ubos = ubo_binding.ubo_desc;

        std::ranges::sort(ubos, [](auto & a, auto & b)
            {return a.index < b.index;}
            );
        
        auto [first, last] = std::ranges::unique(ubos, [](auto & a, auto & b)
            {return a.index == b.index; }
            );

        for(auto & uboidx : std::ranges::subrange(ubos.begin(), first))
        {
            WVkUBO ubo = asset_render_data.GetUBO(uboidx.index);

            void * ptr = wvk::buffer::MapUBO(ubo, device);
            wvk::buffer::UpdateUBO(ptr, ubo_write.data, ubo_write.size, ubo_write.offset);
            wvk::buffer::UnmapUBO(ubo, device);
        }
    }

    template<std::uint8_t FramesInFlight>
    inline void UpdateParamDynamic(
        WVkDescSetUBOBinding<FramesInFlight> & ubo_binding,
        WVkDescSetUBOWrite & ubo_write,
        WVkAssetRenderDataRAII & asset_render_data,
        VkDevice device,
        std::uint8_t frame_index
        ) {
            WVkUBO ubo = asset_render_data.GetUBO(ubo_binding.ubo_desc[frame_index].index);

            void * ptr = wvk::buffer::MapUBO(ubo, device);
            wvk::buffer::UpdateUBO(ptr, ubo_write.data, ubo_write.size, ubo_write.offset);
            wvk::buffer::UnmapUBO(ubo, device);
    }


}
