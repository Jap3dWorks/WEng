#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/WId.hpp"
#include "WVulkan/RAII/UBOManager/BlockSizeUBOs.hpp"
#include "WVulkan/RAII/UBOManager/DynamicUBOManager.hpp"
#include "WVulkan/Vk/WVkBuffer.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WCore/TVisitor.hpp"
#include "WVulkan/RAII/UBOManager/DynamicUBOManager.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"

#include "WCoreTypes/WRenderTypes.hpp"

#include <algorithm>
#include <vulkan/vulkan_core.h>

namespace wvk::raii::pipelines::desc_bindings {

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

    WNODISCARD inline std::vector<WVkDescSetTextureBinding> CollectTextureBindings(
        wct::render::RPipeParamList_WAssetId const & texture_params,
        wvk::raii::AssetRenderData & asset_render_data
        ) {
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
        wcr::wid::WEngId param_id,
        std::uint8_t binding)
    {
        wcr::wid::WAssetTypeId assettype;
        wcr::wid::WAssetId assetid;
        wcr::wid::WSubIdxId idx;

        // binding as index
        param_id.AsAssetIndexId().ExtractWIds(assettype, assetid, idx);

        return wcr::wid::WEngId::FromAsset(
            wcr::wid::WTypeAssetIndexId(
                assettype,
                assetid,
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
                wcr::wid::nullid,
                wcr::wid::nullid}
            );
    }
    

    template<std::uint8_t FramesInFlight, std::uint8_t UBOFrames>
    inline constexpr auto UBOManFrameFlag() {
        if constexpr (UBOFrames == 1) {
            return wvk::raii::ubo_manager
                ::DynamicUBOManager<FramesInFlight>::STATIC_FRAME_FLAG;
        }
        else {
            return wvk::raii::ubo_manager
                ::DynamicUBOManager<FramesInFlight>::DYNAMIC_FRAME_FLAG;
        }
    };


    template<typename Params, CIterable<Params> ParamsCollection>
    inline auto GetBindingParamsMap(ParamsCollection const & params) {
        std::unordered_map<
            std::uint8_t,
            Params const *> binding_params{};

        for (auto & param : params) {
            binding_params.insert(
                {param.binding, &param}
                );
        }

        return binding_params;
    }


    template<std::uint8_t FramesInFlight, std::uint8_t UBOFrames>
    constexpr auto ubo_frame_flag_v=UBOManFrameFlag<FramesInFlight, UBOFrames>();

    template<std::uint8_t FramesInFlight>
    WNODISCARD inline std::vector<_new_WVkDescSetUBOBinding<FramesInFlight>>
    CollectUBOBindings_Dynamic(
        wcr::wid::WEngId binding_set_id,
        wcr::wid::WEngId parameter_bindings_id,
        wct::render::RPipeParamDescriptorsLayout const & param_descriptors,
        wct::render::RPipeParamList_Ubo const & ubo_params,
        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> & dynamic_ubo_man
        ) {

        auto binding_param =  GetBindingParamsMap<wct::render::RPipeParamUbo>(
            ubo_params
            );

        auto CreateUBOBinding =
            [&]
            <std::uint8_t UBOFrames>
            (wct::render::RPipeParamDescriptor const & desc, wcr::wid::WEngId wid)
            -> _new_WVkDescSetUBOBinding<FramesInFlight>
            {
                constexpr auto frame_flag=ubo_frame_flag_v<FramesInFlight, UBOFrames>;

                if (!dynamic_ubo_man.template Contains<frame_flag>(
                        desc.size, wid) ) {
                    void const * ptr = binding_param.contains(desc.binding)
                        ? GetUboPtrData(*binding_param[desc.binding])
                        : nullptr;

                    dynamic_ubo_man.template Add<frame_flag>(desc.size, {wid}, ptr);
                }
                
                _new_WVkDescSetUBOBinding<FramesInFlight> result {
                    .binding=desc.binding,
                    .offset=dynamic_ubo_man.template GetOffset<frame_flag>(
                        desc.size, wid
                        ),
                    .ubo_type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
                };

                for(std::uint8_t f=0; f<FramesInFlight; ++f) {
                    std::uint8_t ubo_f_index =
                        std::min(static_cast<std::uint8_t>(UBOFrames-1),f);

                    result.buffers[f]=dynamic_ubo_man.template GetUBO<frame_flag>(
                                desc.size, ubo_f_index
                        ).buffer;
                    result.range=desc.size;
                }
                return result;
                        
            };

        std::vector<_new_WVkDescSetUBOBinding<FramesInFlight>> result{};
        result.reserve(ubo_params.size());

        for (auto & descriptor : param_descriptors) {

            switch(descriptor.type) {

            case wct::render::ERPipeParamType::UBO_Static:
                result.push_back(
                    CreateUBOBinding
                    .template operator()<1> (
                        descriptor,
                        ToAssetParamBindingId(
                            parameter_bindings_id,
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
                            parameter_bindings_id,
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


    template<std::uint8_t FramesInFlight>
    WNODISCARD std::vector<_new_WVkDescSetUBOBinding<FramesInFlight>>
    CollectUBOBindings (
        VkDevice device,
        VkPhysicalDevice physical_device,
        // wcr::wid::WEngId binding_set_id,
        // wcr::wid::WEngId parameter_bindings_id,  // pipeline params
        wct::render::RPipeParamDescriptorsLayout const & param_descriptors,
        wct::render::RPipeParamList_Ubo const & ubo_params
        ){

        auto binding_param =
            GetBindingParamsMap<wct::render::RPipeParamUbo>(
                ubo_params
                );

        std::uint32_t buffer_size = std::accumulate(
            param_descriptors.begin(),
            param_descriptors.end(),
            0,
            [](auto & desc) ->std::uint32_t {
                return desc.type != wct::render::ERPipeParamType::None
                    ? desc.size
                    : 0; });

        std::array<WVkBuffer, FramesInFlight> ubo_buffers;
        for(std::uint32_t i=0; i<FramesInFlight; ++i) {
            ubo_buffers = wvk::buffer::CreateUBOBuffer(
            buffer_size,
            device,
            physical_device
            );
        }

        std::vector<_new_WVkDescSetUBOBinding<FramesInFlight>> result;
        result.reserve(param_descriptors.size());

        std::uint32_t offset=0;
        for(auto & param_desc : param_descriptors) {
            result.push_back(
                {
                    .binding=param_desc.binding,
                    .buffers=ubo_buffers,
                    .offset=offset,
                    .range=param_desc.size,
                    .ubo_type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
                }
                );

            offset += param_desc.size;

            if (binding_param.contains(param_desc.binding)) {
                for(std::uint32_t f=0; f<FramesInFlight; ++f) {
                    wvk::buffer::UpdateBuffer(
                        wvk::buffer::MapBuffer(
                            ubo_buffers[f],
                            device
                            ),
                        GetUboPtrData(*binding_param[param_desc.binding]),
                        param_desc.size,
                        offset
                        );

                    wvk::buffer::UnmapBuffer(ubo_buffers[f]);
                }
            }

        }
        return result;
    }

    template<std::uint8_t FramesInFlight>
    auto CreateParamsDescriptorSet(
        VkDevice device,
        VkDescriptorSetLayout desc_layout,
        VkDescriptorPool desc_pool,
        std::vector<WVkDescSetTextureBinding> const & texture_bindings,
        std::vector<_new_WVkDescSetUBOBinding<FramesInFlight>> const & ubo_bindings
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
            write_sets.reserve(texture_bindings.size() + ubo_bindings.size());
            std::vector<VkDescriptorBufferInfo> buffer_infos;
            buffer_infos.reserve(ubo_bindings.size());

            for(auto & p : ubo_bindings) {
                
                VkDescriptorBufferInfo buffer_info {
                    .buffer=p.buffers[f],
                    .offset = (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC == p.ubo_type)
                        ? 0
                        : p.offset,
                    .range=p.range
                };

                buffer_infos.push_back(buffer_info);

                VkWriteDescriptorSet ubo_write = wvk::types::VkWriteDescriptorSet();
                ubo_write.dstBinding = p.binding;
                ubo_write.dstSet = descriptor_set;
                ubo_write.dstArrayElement = 0;
                ubo_write.descriptorType = p.ubo_type;
                ubo_write.descriptorCount = 1;
                ubo_write.pBufferInfo = &buffer_infos.back();
            
                write_sets.push_back(std::move(ubo_write));
            }

            for (auto & t : texture_bindings) {

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
        wcr::wid::WEngId binding_set_id,
        wcr::wid::WEngId descriptor_bindings_id,
        wct::render::RPipeParamDescriptor param_descriptor,
        wct::render::RPipeParamUbo const & ubo_data,
        wvk::raii::ubo_manager::DynamicUBOManager<FramesInFlight> & ubo_man
        ) {

        wvk::raii::ubo_manager::BlockSizeIntT block_size =
            static_cast<wvk::raii::ubo_manager::BlockSizeIntT>(param_descriptor.size);

        auto update_dynamic_param =
            [&ubo_man, &frame_index, &block_size, &ubo_data]
            (wcr::wid::WEngId ubo_param_id ) {
                if constexpr (UpdateType == EUpdateType::STATIC) {
                    for(std::uint8_t f=0; f<FramesInFlight; ++f) {
                        ubo_man.template Update<DYNAMIC_FLAG<FramesInFlight>>(
                            block_size ,
                            f ,
                            std::vector{ubo_param_id},
                            GetUboPtrData(ubo_data)
                            );
                    
                    } 
                } else {
                    ubo_man.template Update<DYNAMIC_FLAG<FramesInFlight>>(
                        block_size ,
                        frame_index ,
                        std::vector{ubo_param_id},
                        GetUboPtrData(ubo_data)
                        );
                }
            } ;
                    

        switch (param_descriptor.type) {
        case wct::render::ERPipeParamType::UBO_Static:
            ubo_man.template Update<STATIC_FLAG<FramesInFlight>>(
                block_size,
                0,
                std::vector{ToAssetParamBindingId(descriptor_bindings_id, param_descriptor.binding)},
                GetUboPtrData(ubo_data)
                );
            break;

        case wct::render::ERPipeParamType::UBO_Dynamic:
            update_dynamic_param(ToAssetParamBindingId(descriptor_bindings_id, param_descriptor.binding));
            break;

        case wct::render::ERPipeParamType::UBO_Entity_Static:
            ubo_man.template Update<STATIC_FLAG<FramesInFlight>>(
                block_size ,
                0,
                std::vector{ToEntityParamId(binding_set_id)},
                GetUboPtrData(ubo_data)
                );
            break;

        case wct::render::ERPipeParamType::UBO_Entity_Dynamic:
            update_dynamic_param(ToEntityParamId(binding_set_id));
            break;

        case wct::render::ERPipeParamType::UBO_Component_Static:
            ubo_man.template Update<DYNAMIC_FLAG<FramesInFlight>>(
                block_size ,
                0,
                std::vector{binding_set_id},
                GetUboPtrData(ubo_data)
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
