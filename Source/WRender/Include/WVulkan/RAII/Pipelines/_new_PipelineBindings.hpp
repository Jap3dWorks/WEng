#pragma once

#include "WCore/WId.hpp"
#include "WVulkan/RAII/UBOManager/DynamicUBOManager.hpp"
#include "WVulkan/RAII/Pipelines/GBuffer.hpp"
#include "WVulkan/RAII/Pipelines/Postprocess.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WCore/TVisitor.hpp"
#include "WVulkan/RAII/UBOManager/DynamicUBOManager.hpp"

#include "WAssets/RenderPipelineParams.hpp"
#include "WAssets/RenderPipeline.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/Vk/WVkBuffer.hpp"

#include <optional>
#include <ranges>
#include <iterator>
#include <algorithm>

namespace wvk::raii::pipe_bindings {

    WNODISCARD inline std::vector<WVkDescSetTextureBinding> CollectTextureBindings(
        was::RenderPipelineParams const & parameters,
        wvk::raii::AssetRenderData & asset_render_data
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

    template<std::uint8_t FramesInFlight, std::uint8_t UBOFrames>
    constexpr auto UBOManFrameFlag() {
        if constexpr (UBOFrames == 1) {
            return wvk::raii::ubo_manager
                ::DynamicUBOManager<FramesInFlight>::STATIC_FRAME_FLAG;
        }
        else {
            return wvk::raii::ubo_manager
                ::DynamicUBOManager<FramesInFlight>::DYNAMIC_FRAME_FLAG;
        }
    };

    template<std::uint8_t FramesInFlight, std::uint8_t UBOFrames>
    constexpr auto ubo_frame_flag_v=UBOManFrameFlag<FramesInFlight, UBOFrames>();

    template<std::uint8_t FramesInFlight>
    WNODISCARD std::vector<_new_WVkDescSetUBOBinding<FramesInFlight>> CollectUBOBindings(
        wcr::wid::WEntityComponentId entity_component_id,
        was::RenderPipeline const & pipeline,
        was::RenderPipelineParams const & parameters,
        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> & ubo_man
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
            <std::uint8_t UBOFrames>
            (wct::render::RPipeParamDescLayInfo const & desc, wcr::wid::WEngId wid)
            -> _new_WVkDescSetUBOBinding<FramesInFlight>
            {
                constexpr auto frame_flag=ubo_frame_flag_v<FramesInFlight, UBOFrames>;

                if (!ubo_man.template Contains<frame_flag>(
                        desc.size, wid) ) {
                    void const * ptr = binding_param.contains(desc.binding)
                        ? get_ubo_data_ptr(binding_param[desc.binding])
                        : nullptr;

                    ubo_man.template Add<frame_flag>(desc.size, {wid}, ptr);
                }
                
                _new_WVkDescSetUBOBinding<FramesInFlight> result{};
                result.binding = desc.binding;
                result.dynamic_offset = ubo_man.template GetOffset<frame_flag>(
                    desc.size, wid
                    );

                for(std::uint8_t f=0; f<FramesInFlight; ++f) {
                    std::uint8_t ubo_f_index =
                        std::min(static_cast<std::uint8_t>(UBOFrames-1),f);

                    result.buffers[f]=ubo_man.template GetUBO<frame_flag>(
                                desc.size, ubo_f_index
                        ).buffer;
                    result.range=desc.size;
                }
                return result;
                        
            };

        std::vector<_new_WVkDescSetUBOBinding<FramesInFlight>> result{};
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
                        ));
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
                        wct::render::ApplyPipeParamType(
                            wcr::wid::WEngId::FromEntityComponent(entity_component_id),
                            wct::render::ERPipeParamType::UBO_Entity_Static
                            )
                        )
                    );

                break;

            case wct::render::ERPipeParamType::UBO_Entity_Dynamic:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<FramesInFlight> (
                        descriptor,
                        wct::render::ApplyPipeParamType(
                            wcr::wid::WEngId::FromEntityComponent(entity_component_id),
                            wct::render::ERPipeParamType::UBO_Entity_Static
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

    // template<std::uint8_t FramesInFlight>
    // inline void CreateBindingSet(
    //     wct::render::ERPipeType pipeline_type,
    //     wcr::wid::WEntityComponentId binding_set_id,
    //     wcr::wid::WTypeAssetIndexId renderable_asset_id,
    //     wcr::wid::WAssetId pipeline_id,
    //     wvk::raii::pipelines::GBuffer<FramesInFlight> & gbuffers_pipelines,
    //     wvk::raii::pipelines::Postprocess & postprocess_pipelines,
    //     std::vector<WVkDescSetUBOBinding<FramesInFlight>> ubo_bindings,
    //     std::vector<WVkDescSetTextureBinding> texture_bindings
    //     ) {
    //     switch(pipeline_type) {
    //     case wct::render::ERPipeType::Postprocess:
    //         postprocess_pipelines.CreateBindingSet(
    //             binding_set_id,
    //             pipeline_id,
    //             std::move(ubo_bindings),
    //             std::move(texture_bindings)
    //             );
    //         break;

    //     default:
    //         gbuffers_pipelines.CreateBindingSet(
    //             binding_set_id,
    //             pipeline_id,
    //             renderable_asset_id,
    //             ubo_bindings,
    //             texture_bindings
    //             );
    //     }
    // }

    template<std::uint8_t FramesInFlight>
    inline WVkDescSetUBOBinding<FramesInFlight> GetUboBinding(
        wcr::wid::WEntityComponentId binding_set_id,
        std::uint8_t binding,
        wct::render::ERPipeType pipe_type,
        wvk::raii::pipelines::GBuffer<FramesInFlight> const & gbuffers_pipelines,
        wvk::raii::pipelines::Postprocess const & postprocess_pipelines
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

    inline auto GetUboPtrData(wct::render::RPipeParamUbo const & ubo_param) {
        return std::visit(
            wcr::TVisitor(
                [&ubo_param](auto const & ubodata) -> void const * {
                    return ubodata.data();
                }
                ),
            ubo_param.data
            );
    }

    inline WVkDescSetUBOWrite GetUboWrite(wct::render::RPipeParamUbo const & ubo_pipe_param) {
        return std::visit(
            wcr::TVisitor(
                [&ubo_pipe_param](auto const & ubodata) -> WVkDescSetUBOWrite {
                    return {
                        .binding = ubo_pipe_param.binding,
                        .data = ubodata.data(),
                        .size = ubodata.size(),
                        .offset = 0 //  ubo_pipe_param.offset
                    };
                }
                ),
            ubo_pipe_param.data
            );
    }

    template<std::uint8_t FramesInFlight>
    inline void UpdateParamStatic(
        WVkDescSetUBOBinding<FramesInFlight> & ubo_binding,
        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> & ubo_man,
        void const * data
        ) {

        if (ubo_binding.ubo_desc[0].desc_buffer.buffer ==
            ubo_binding.ubo_desc[1].desc_buffer.buffer) {
            ubo_man.template Update<ubo_man.STATIC_FRAME_FLAG>(
                ubo_binding.ubo_desc[0].desc_buffer.range,
                0,
                {ubo_binding.ubo_desc[0].index},
                data
                );
        }
        else {
            for(std::uint32_t f=0; f<FramesInFlight; f++) {
                ubo_man.template Update<ubo_man.DYNAMIC_FRAME_FLAG>(
                    ubo_binding.ubo_desc[f].desc_buffer.range,
                    f,
                    {ubo_binding.ubo_desc[f].index},
                    data
                    );
            }
        }
    }

    template<std::uint8_t FramesInFlight>
    inline void UpdateParamDynamic(
        WVkDescSetUBOBinding<FramesInFlight> & ubo_binding,
        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> & ubo_man,
        std::uint8_t frame_index,
        void const * data
        ) {
        ubo_man.template Update<ubo_man.DYNAMIC_FRAME_FLAG>(
            ubo_binding.ubo_desc[frame_index].desc_buffer.range,
            frame_index,
            {ubo_binding.ubo_desc[frame_index].index},
            data
            );
    }


}
