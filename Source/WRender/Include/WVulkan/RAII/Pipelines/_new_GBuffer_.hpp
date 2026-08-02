#pragma once

#include "WAssets/RenderPipelineParams.hpp"
#include "WCore/WCore.hpp"
#include "WCore/WDebug.hpp"
#include "WCore/WId.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WLog.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WVulkan/RAII/UBOManager/BlockSizeUBOs.hpp"
#include "WVulkan/RAII/UBOManager/DynamicUBOManager.hpp"
#include "WVulkan/Vk/WVkDescriptor.hpp"
#include "WVulkan/Vk/WVkShader.hpp"
#include "WVulkan/Vk/WVkPipeline.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WAssets/RenderPipeline.hpp"
#include "WVulkan/RAII/Pipelines/_new_GBuffer_lib.hpp"
#include "WVulkan/RAII/DescriptorSetLayout.hpp"
#include "WVulkan/RAII/Pipelines/_new_PipelineBindings.hpp"

#include <vector>
#include <vulkan/vulkan_core.h>
#include "WVulkan/RAII/Pipelines/WVkPipelinesBase.hpp"


namespace wvk::raii::pipelines {

    /**
     * @brief Graphics Pipelines outputs the GBuffers.
     */
    template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
    class _new_GBuffer
    {

    public:

        static inline constexpr const std::uint8_t MODEL_UBO_BINDING{0};
        static inline constexpr const std::uint8_t MODEL_UBO_DESC_SET{1};
        
        static inline constexpr const std::uint8_t PARAM_UBO_BINDING{0};
        static inline constexpr const std::uint8_t PARAM_UBO_DESC_SET{2};
        
        static inline constexpr const std::uint8_t TEXTURES_DESC_SET{3};

        static inline constexpr const VkDescriptorSetLayoutBinding MODEL_UBO_LAYOUT_BINDING{
            .binding=0,
            .descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .descriptorCount=1,
            .stageFlags=VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers=nullptr,
        };

        _new_GBuffer() noexcept=default;
        ~_new_GBuffer() = default;
        _new_GBuffer(const _new_GBuffer&)=delete;
        _new_GBuffer & operator=(const _new_GBuffer&) = delete;
        _new_GBuffer(_new_GBuffer && other) noexcept =default;
        _new_GBuffer & operator=(_new_GBuffer && other) noexcept =default;

        _new_GBuffer(VkDevice device, VkPhysicalDevice physical_device) :
            vkn_(device, physical_device),
            model_ubo_layout({device},
                             std::array{
                                 VkDescriptorSetLayoutBinding{
                                     MODEL_UBO_LAYOUT_BINDING
                                 }
                             }
                ),
            descriptor_set_layouts(
                [](auto id) {return VK_NULL_HANDLE;},
                [device](auto layout) { wvk::descriptor::Destroy(layout, device); }
                ),
            pipelines(
                [](auto id) {return VK_NULL_HANDLE;},
                [device](auto pipeline) { wvk::descriptor::Destroy(pipeline,device); }
                )
            {}

    public:

        void CreatePipeline(
            const was::RenderPipeline & pipeline_asset,
            VkDescriptorSetLayout global_descset_layout,
            std::size_t collection_id
            ) {

            assert(descriptor_set_layouts.Contains(pipeline_asset.Get_asset_id().GetId()));

            auto shaders_info =
                wvk::shader::ToShaderStageInfo(pipeline_asset.Get_shader_list());

            auto layoyt_binding =
                wvk::descriptor::ToDescriptorSetLayoutBinding(
                    pipeline_asset.Get_descriptor_list()
                    );

            descriptor_set_layouts.CreateAt(
                pipeline_asset.Get_asset_id().GetId(),
                [this, &layoyt_binding](auto id){
                    return wvk::descriptor::CreateDescriptorSetLayout(
                        layoyt_binding,
                        vkn_.device
                        );
                }
                );

            pipelines.CreateAt(
                pipeline_asset.Get_asset_id().GetId(),
                [this, &global_descset_layout, &shaders_info]
                (auto id) {
                    return wvk::raii::pipelines::gbuffer_lib::CreatePipeline(
                        vkn_.device,
                        {
                            global_descset_layout,
                            *model_ubo_layout, // ubo model at desc set 1
                            descriptor_set_layouts.Get(id)
                        },
                        {
                            shaders_info
                        }
                        );
                }
                );

            pipeline_bindings_[pipeline_asset.Get_asset_id().GetId()] = {};
        }

        void CreateBindingSet(
            std::size_t collection_id,
            wcr::wid::WEngId binding_set_id,
            was::RenderPipeline asset_pipeline,
            was::RenderPipelineParams asset_params,
            wcr::wid::WTypeAssetIndexId in_mesh_asset_id,
            wvk::raii::AssetRenderData & asset_data // ,
            // std::vector<WVkDescSetUBOBinding<FramesInFlight>> in_ubos,
            // std::vector<WVkDescSetTextureBinding> in_textures
            ){

            EnsureCollection(collection_id);

            auto texture_params = wvk::raii::pipe_bindings::CollectTextureBindings(
                asset_params,
                asset_data
                );

            auto ubo_params = wvk::raii::pipe_bindings::CollectUBOBindings<FramesInFlight>(
                binding_set_id,
                asset_pipeline,
                asset_params,
                collections[collection_id].ubo_manager
                );

            wcr::wid::WEngId::IdType model_ubo_id = wct::render::ApplyPipeParamType(
                binding_set_id,
                wct::render::ERPipeParamType::UBO_Entity_Dynamic
                ).GetId();

            EnsureModelUboDescriptorSet(collection_id, model_ubo_id);

            // TODO to ensure fn
            if (!collections[collection_id].param_descriptors.contains(
                    asset_params.Get_asset_id())) {
                
                auto descriptors = CreateBindingDescriptorSets(
                    collection_id,
                    binding_set_id,
                    texture_params,
                    ubo_params
                    );

                collections[collection_id].param_descriptors[asset_params.Get_asset_id()] =
                    std::move(descriptors);
            }

            // TODO 
            std::vector<std::uint32_t> ubo_param_offsets=GetUboParamOffset(
                ubo_params
                );

            wvk::raii::pipelines::gbuffer_lib::Binding binding{
                .mesh_id=in_mesh_asset_id,
                .model_ubo_offset = collections[collection_id]
                .ubo_manager.GetOffset<FramesInFlight>(sizeof(wct::render::ModelUBO), binding_set_id),
                .model_ubo_descriptor_set = collections[collection_id].model_ubo_desc_set,
                .param_ubo_offsets={},
                .param_descriptor_set=collections[collection_id].param_descriptors[asset_params.Get_asset_id()]
            };

            bindings[binding_set_id]=std::move(binding);

            pipeline_bindings_[asset_pipeline.Get_asset_id()]
                .Insert(binding_set_id.GetId(), binding_set_id);

        }

    private:

        void EnsureCollection(std::size_t id) {
            if(!collections.contains(id)) {
                collections[id]={
                    .descriptor_pool{{vkn_.device}},
                    .ubo_manager{
                        vkn_.device, 
                        vkn_.physical_device, 
                        wvk::raii::ubo_manager::INITIAL_UBO_COUNT
                    }
                };
            }
        }

        VkDescriptorSet EnsureModelUboDescriptorSet(
            std::size_t collection_id,
            wcr::wid::WEngId model_ubo_id
            ) {
            
            VkDescriptorSet model_ubo_descriptor;

            if(collections[collection_id].model_ubo_desc_set[0] == VK_NULL_HANDLE) {

                auto descriptors =
                    wvk::raii::pipelines::gbuffer_lib::
                    CreateModelUboDescriptorSet(
                        vkn_.device,
                        model_ubo_id,
                        MODEL_UBO_BINDING,
                        collections[collection_id],
                        *model_ubo_layout
                        );

                collections[collection_id].model_ubo_desc_set = descriptors;
            }
        }

        auto CreateBindingDescriptorSets(
            std::size_t collection_id,
            wcr::wid::WEngId binding_set_id,
            was::RenderPipeline const & pipeline_asset,
            std::vector<WVkDescSetTextureBinding> texture_params,
            std::vector<_new_WVkDescSetUBOBinding<FramesInFlight>> ubo_params
            ) {
            
            std::array<VkDescriptorSet, FramesInFlight> descriptor_sets;

            VkDescriptorSetLayout desc_layout =
                descriptor_set_layouts.Get(pipeline_asset.Get_asset_id().GetId());

            for (std::uint32_t f=0; f<FramesInFlight; ++f) {

                VkDescriptorSet descriptor_set;

                VkDescriptorSetAllocateInfo alloc_info =
                    wvk::types::VkDescriptorSetAllocateInfo();

                alloc_info.descriptorPool =
                    collections[collection_id].descriptor_pool;
                alloc_info.descriptorSetCount = 1;
                alloc_info.pSetLayouts = &desc_layout;

                wvk::vulkan::ExecVkProcChecked(
                    vkAllocateDescriptorSets,
                    "Failed to allocate descriptor sets!",
                    vkn_.device,
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
                    vkn_.device,
                    static_cast<std::uint32_t>(write_sets.size()),
                    write_sets.data(),
                    0,
                    nullptr
                    );

                descriptor_sets[f] = descriptor_set;
            }

            return descriptor_sets;
        }


        struct {
            VkDevice device{VK_NULL_HANDLE};
            VkPhysicalDevice physical_device{VK_NULL_HANDLE};
        } vkn_;

        wvk::raii::DescriptorSetLayout<1> model_ubo_layout;
        // Model UBO pool        (by collection, by level)
        // Model UBO descriptors

        // ubo_params and textures layouts
        TObjectDataBase<VkDescriptorSetLayout, void, std::size_t>
        descriptor_set_layouts;
        // param pool       (collections by level)
        // param descriptor (collections by level)

        TObjectDataBase<std::tuple<VkPipeline, VkPipelineLayout>, void, std::size_t>
        pipelines;

        std::unordered_map<wcr::wid::WEngId,
                           wvk::raii::pipelines::gbuffer_lib::Binding<FramesInFlight>>
        bindings;

        std::unordered_map<
            wcr::wid::WAssetId::IdType,
            TSparseSet<wcr::wid::WEngId>> pipeline_bindings_{};

        // collection : pool | buffer | bindings
        std::unordered_map<
            std::size_t,
            wvk::raii::pipelines::gbuffer_lib::Collection<FramesInFlight>>
        collections{};

    };

}

