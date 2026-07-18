#pragma once

#include "WCore/WId.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVkRAII/WVkAssetRenderDataRAII.hpp"
#include "WCore/TVisitor.hpp"

#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include <ranges>
#include <iterator>
#include <algorithm>

namespace wvk::render::pipe_bindings {

    inline
    std::vector<WVkDescSetTextureBinding> CollectPipelineBindings(
        WRenderPipelineParametersAsset const & parameters,
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
    std::vector<WVkDescSetUBOBinding<FramesInFlight>> CollectUBOBindings(
        wcr::wid::WEntityComponentId entity_component_id,
        WRenderPipelineAsset const & pipeline,
        WRenderPipelineParametersAsset const & parameters,
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

        auto get_ubo_data_ptr = [](const wct::render::RPipeParamUbo& ubo_param) -> const void* {
            return std::visit(
                wcr::TVisitor([](const auto& data) -> const void* {
                    return static_cast<const void*>(data.data());
                }),
                ubo_param.data
                );
        };        

        auto CreateUBO =
            [&]
            <std::uint8_t Frames, std::uint8_t UBOS>
            (wct::render::RPipeParamDescLayInfo const & desc, wcr::wid::WEngId wid)
            -> WVkDescSetUBOBinding<FramesInFlight>
            {

                std::array<WVkUBO, UBOS> vk_ubos{};

                if (!asset_render_data.ContainsUBOs(wid)) {

                    void * ptr = get_ubo_data_ptr(binding_param[desc.binding]);

                    for (std::uint8_t i=0; i<UBOS; i++) {
                        
                        std::size_t uboindx = asset_render_data.CreateUBO(
                            wid,
                            desc.size,
                            ptr
                            );

                        vk_ubos[i] = asset_render_data.GetUBO(uboindx);
                    }
                }
                else {
                    std::ranges::copy(
                        asset_render_data.GetUBOs(wid) | std::views::take(vk_ubos.size()),
                        vk_ubos.begin());
                }

                WVkDescSetUBOBinding<FramesInFlight> result{};
                result.binding = desc.binding;

                for (std::uint8_t i=0; i<Frames; i++) {
                    result.ubo_info[i] = {
                        .buffer=vk_ubos[std::min(i,UBOS)].buffer,
                        .offset=0,
                        .range=vk_ubos[std::min(i,UBOS)].range
                    };
                }

                return result;
            };

        std::vector<WVkDescSetUBOBinding<FramesInFlight>> result{};
        result.reserve(ubo_params.size());


        for (auto & desc : pipeline.Get_descriptor_list()) {

            switch(desc.type) {
                
            case wct::render::ERPipeParamType::UBO_Static:

                auto assetid = wcr::wid::WTypeAssetIndexId(
                    wcr::wid::null_id,
                    parameters.Get_asset_id(),
                    desc.binding
                    );

                result.push_back(
                    CreateUBO.operator()<FramesInFlight, 1> (
                        desc,
                        wcr::wid::WEngId::FromAsset(assetid)
                        )
                    );
                
                break;

            case wct::render::ERPipeParamType::UBO_Dynamic:
                assetid = wcr::wid::WTypeAssetIndexId(
                    wcr::wid::null_id,
                    parameters.Get_asset_id(),
                    desc.binding
                    );

                result.push_back(
                    CreateUBO.operator()<FramesInFlight, FramesInFlight> (
                        desc,
                        wcr::wid::WEngId::FromAsset(assetid)
                        )
                    );


                break;

                // Create Frames UBO by param asset

            case wct::render::ERPipeParamType::UBOEntity_Static:
                
                result.push_back(
                    CreateUBO.operator()<FramesInFlight, 1> (
                        desc,
                        wcr::wid::WEngId::FromEntityComponent(
                            entity_component_id
                            )
                        )
                    );

                break;

            case wct::render::ERPipeParamType::UBOEntity_Dynamic:

                result.push_back(
                    CreateUBO.operator()<FramesInFlight, FramesInFlight> (
                        desc,
                        wcr::wid::WEngId::FromEntityComponent(
                            entity_component_id
                            )
                        )
                    );
            default:
                continue;
            }

        }

        return result;
    }

}
