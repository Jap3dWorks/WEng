#pragma once

#include "WCore/WId.hpp"
#include "WVulkan/RAII/UBOManager/BlockSizeUBOs.hpp"
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

namespace wvk::raii::pipelines::desc_bindings {

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

    inline constexpr wcr::wid::WEngId ToAssetParamBindingId(
        wcr::wid::WAssetId asset_param_id,
        std::uint8_t binding)
    {
        return wcr::wid::WEngId::FromAsset(
            wcr::wid::WTypeAssetIndexId(
                wcr::wid::null_id,
                asset_param_id,
                binding
                ));
    }

    constexpr inline wcr::wid::WEngId ToEntityParamId(
        wcr::wid::WEngId id) noexcept {

        wcr::wid::WAssetId level;
        wcr::wid::WEntityId entity;
        wcr::wid::WComponentTypeId component;
        wcr::wid::WSubIdxId indx;

        id.AsEntityComponentId().ExtractWIds(level, entity, component, indx);
            
        return wcr::wid::WEngId::FromEntityComponent(
            wcr::wid::WEntityComponentId{
                level,
                entity,
                wcr::wid::null_id,
                wcr::wid::null_id}
            );
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
        wcr::wid::WEngId binding_set_id,
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
            (wct::render::RPipeParamDescriptor const & desc, wcr::wid::WEngId wid)
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

            // TODO required binidng id

            case wct::render::ERPipeParamType::UBO_Static:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<1> (
                        descriptor,
                        ToAssetParamBindingId(
                            parameters.Get_asset_id(), 
                            descriptor.binding
                            )
                        ));
                break;

            case wct::render::ERPipeParamType::UBO_Dynamic:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<FramesInFlight> (
                        descriptor,
                        ToAssetParamBindingId(
                            parameters.Get_asset_id(), 
                            descriptor.binding
                            )
                        )
                    );

                break;

            case wct::render::ERPipeParamType::UBO_Entity_Static: 
                result.push_back(
                    CreateUBOBinding
                    .template operator()<1> (
                        descriptor,
                        ToEntityParamId(
                            binding_set_id
                            )
                        )
                    );

                break;

            case wct::render::ERPipeParamType::UBO_Entity_Dynamic:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<FramesInFlight> (
                        descriptor,
                        ToEntityParamId(
                            binding_set_id
                            )
                        )
                    );

                break;
                
            case wct::render::ERPipeParamType::UBO_Component_Static:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<1> (
                        descriptor,
                        binding_set_id
                        )
                    );

                break;

            case wct::render::ERPipeParamType::UBO_Component_Dynamic:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<FramesInFlight> (
                        descriptor,
                        binding_set_id
                        )
                    );

                break;

            default:
                continue;
                
            }
        }

        return result;
    }

    inline void const * GetUboPtrData(wct::render::RPipeParamUbo const & ubo_param) {
        return std::visit(
            wcr::TVisitor(
                [&ubo_param](auto const & ubodata) -> void const * {
                    return ubodata.data();
                }
                ),
            ubo_param.data
            );
    }

    template<std::uint8_t FramesInFlight>
    auto CreateParamsDescriptorSet(
        VkDevice device,
        VkDescriptorSetLayout desc_layout,
        VkDescriptorPool desc_pool,
        std::vector<WVkDescSetTextureBinding> const & texture_params,
        std::vector<_new_WVkDescSetUBOBinding<FramesInFlight>> const & ubo_params
        ) {
            
        std::array<VkDescriptorSet, FramesInFlight> descriptor_sets;

        for (std::uint32_t f=0; f<FramesInFlight; ++f) {

            VkDescriptorSet descriptor_set;

            VkDescriptorSetAllocateInfo alloc_info =
                wvk::types::VkDescriptorSetAllocateInfo();

            alloc_info.descriptorPool = desc_pool;
            alloc_info.descriptorSetCount = 1;
            alloc_info.pSetLayouts = &desc_layout;

            wvk::vulkan::ExecVkProcChecked(
                vkAllocateDescriptorSets,
                "Failed to allocate descriptor sets!",
                device,
                &alloc_info,
                &descriptor_set
                );

            std::vector<VkWriteDescriptorSet> write_sets;
            write_sets.reserve(texture_params.size() + ubo_params.size());
            std::vector<VkDescriptorBufferInfo> buffer_infos;
            buffer_infos.reserve(ubo_params.size());

            for(auto & p : ubo_params) {
                VkDescriptorBufferInfo buffer_info {
                    .buffer=p.buffers[f],
                    .offset=0,
                    .range=p.range
                };

                buffer_infos.push_back(buffer_info);

                VkWriteDescriptorSet ubo_write = wvk::types::VkWriteDescriptorSet();
                ubo_write.dstBinding = p.binding;
                ubo_write.dstSet = descriptor_set;
                ubo_write.dstArrayElement = 0;
                ubo_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                ubo_write.descriptorCount = 1;
                ubo_write.pBufferInfo = &buffer_infos.back();
            
                write_sets.push_back(std::move(ubo_write));
            }

            for (auto & t : texture_params) {

                VkWriteDescriptorSet tex_write = wvk::types::VkWriteDescriptorSet();

                tex_write.dstBinding = t.binding;
                tex_write.dstSet = descriptor_set;
                tex_write.dstArrayElement = 0;
                tex_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                tex_write.descriptorCount = 1;
                tex_write.pImageInfo = &t.image_info;
                tex_write.pBufferInfo = VK_NULL_HANDLE;

                write_sets.push_back(std::move(tex_write));
            }

            vkUpdateDescriptorSets(
                device,
                static_cast<std::uint32_t>(write_sets.size()),
                write_sets.data(),
                0,
                nullptr
                );

            descriptor_sets[f] = descriptor_set;
        }

        return descriptor_sets;
    }

    // inline WVkDescSetUBOWrite GetUboWrite(wct::render::RPipeParamUbo const & ubo_pipe_param) {
    //     return std::visit(
    //         wcr::TVisitor(
    //             [&ubo_pipe_param](auto const & ubodata) -> WVkDescSetUBOWrite {
    //                 return {
    //                     .binding = ubo_pipe_param.binding,
    //                     .data = ubodata.data(),
    //                     .size = ubodata.size(),
    //                     .offset = 0 //  ubo_pipe_param.offset
    //                 };
    //             }
    //             ),
    //         ubo_pipe_param.data
    //         );
    // }

    // template<std::uint8_t FramesInFlight>
    // inline void UpdateParamStatic(
    //     WVkDescSetUBOBinding<FramesInFlight> & ubo_binding,
    //     wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> & ubo_man,
    //     void const * data
    //     ) {

    //     if (ubo_binding.ubo_desc[0].desc_buffer.buffer ==
    //         ubo_binding.ubo_desc[1].desc_buffer.buffer) {
    //         ubo_man.template Update<ubo_man.STATIC_FRAME_FLAG>(
    //             ubo_binding.ubo_desc[0].desc_buffer.range,
    //             0,
    //             {ubo_binding.ubo_desc[0].index},
    //             data
    //             );
    //     }
    //     else {
    //         for(std::uint32_t f=0; f<FramesInFlight; f++) {
    //             ubo_man.template Update<ubo_man.DYNAMIC_FRAME_FLAG>(
    //                 ubo_binding.ubo_desc[f].desc_buffer.range,
    //                 f,
    //                 {ubo_binding.ubo_desc[f].index},
    //                 data
    //                 );
    //         }
    //     }
    // }

    namespace {
    
        template<std::uint8_t Frames>
        constexpr auto STATIC_FLAG = wvk::raii::ubo_manager
            ::DynamicUBOManager<Frames>::STATIC_FRAME_FLAG;

        template<std::uint8_t Frames>
        constexpr auto DYNAMIC_FLAG = wvk::raii::ubo_manager
            ::DynamicUBOManager<Frames>::DYNAMIC_FRAME_FLAG;
    }

    enum class EUpdateType {
        STATIC,
        DYNAMIC
    };

    template<EUpdateType UpdateType, std::uint8_t FramesInFlight>
    inline void UpdateParameter(
        std::uint8_t frame_index,
        wct::render::RPipeParamDescriptor param_descriptor,
        wcr::wid::WAssetId params_id,
        wcr::wid::WEngId binding_set_id,
        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> & ubo_man,
        wct::render::RPipeParamUbo const & ubo_pipe_param
        ) {

        wvk::raii::ubo_manager::BlockSizeIntT block_size =
            static_cast<wvk::raii::ubo_manager::BlockSizeIntT>(param_descriptor.size);

        auto update_dynamic_param =
            [&ubo_man, &frame_index, &block_size, &ubo_pipe_param]
            (wcr::wid::WEngId ubo_param_id ) {
                if constexpr (UpdateType == EUpdateType::STATIC) {
                    for(std::uint8_t f=0; f<FramesInFlight; ++f) {
                        ubo_man.template Update<DYNAMIC_FLAG<FramesInFlight>>(
                            block_size ,
                            f ,
                            std::vector{ubo_param_id},
                            GetUboPtrData(ubo_pipe_param)
                            );
                    
                    } 
                } else {
                    ubo_man.template Update<DYNAMIC_FLAG<FramesInFlight>>(
                        block_size ,
                        frame_index ,
                        std::vector{ubo_param_id},
                        GetUboPtrData(ubo_pipe_param)
                        );
                }
            } ;
                    

        switch (param_descriptor.type) {
        case wct::render::ERPipeParamType::UBO_Static:
            ubo_man.template Update<STATIC_FLAG<FramesInFlight>>(
                block_size,
                0,
                std::vector{ToAssetParamBindingId(params_id, param_descriptor.binding)},
                GetUboPtrData(ubo_pipe_param)
                );
            break;

        case wct::render::ERPipeParamType::UBO_Dynamic:
            update_dynamic_param(ToAssetParamBindingId(params_id, param_descriptor.binding));
            break;

        case wct::render::ERPipeParamType::UBO_Entity_Static:
            ubo_man.template Update<STATIC_FLAG<FramesInFlight>>(
                block_size ,
                0,
                std::vector{ToEntityParamId(binding_set_id)},
                GetUboPtrData(ubo_pipe_param)
                );
            break;

        case wct::render::ERPipeParamType::UBO_Entity_Dynamic:
            update_dynamic_param(ToEntityParamId(binding_set_id));
            break;

        case wct::render::ERPipeParamType::UBO_Component_Static:
            ubo_man.template Update<DYNAMIC_FLAG<FramesInFlight>>(
                block_size ,
                0,
                binding_set_id,
                GetUboPtrData(ubo_pipe_param)
                );
            break;
        case wct::render::ERPipeParamType::UBO_Component_Dynamic:
            update_dynamic_param(binding_set_id);

            break;
        default:
            break;
                    
        }

    }

}
