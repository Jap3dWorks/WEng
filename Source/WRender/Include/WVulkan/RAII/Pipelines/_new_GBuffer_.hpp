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
#include "WVulkan/RAII/Pipelines/DescriptorBindings.hpp"
#include "WVulkan/RAII/Pipelines/WVkPipelinesBase.hpp"

#include <vector>
#include <vulkan/vulkan_core.h>


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
            param_layouts(
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

            param_layouts.CreateAt(
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
                            param_layouts.Get(id)
                        },
                        {
                            shaders_info
                        }
                        );
                }
                );
        }

        void CreateBindingSet(
            std::size_t collection_id,
            wcr::wid::WEngId binding_set_id,
            was::RenderPipeline asset_pipeline,
            was::RenderPipelineParams asset_params,
            wcr::wid::WTypeAssetIndexId in_mesh_asset_id,
            wvk::raii::AssetRenderData & asset_data // ,
            ){

            EnsureCollection(collection_id);

            wcr::wid::WEngId::IdType model_ubo_id = wct::render::ApplyPipeParamType(
                binding_set_id,
                wct::render::ERPipeParamType::UBO_Entity_Dynamic
                ).GetId();

            EnsureModelUboDescriptorSet(collection_id, model_ubo_id);

            auto param_dynamic_offsets = EnsureParamDescriptorSet(
                collection_id,
                binding_set_id,
                asset_pipeline,
                asset_params,
                asset_data
                );

            auto model_ubo_offset = collections[collection_id]
                .ubo_manager
                .GetOffset<FramesInFlight>(
                    sizeof(wct::render::ModelUBO),
                    binding_set_id);

            auto param_descriptor_sets = collections[collection_id]
                .param_descriptors[asset_params.Get_asset_id()];

            wvk::raii::pipelines::gbuffer_lib::Binding binding{
                .mesh_id=in_mesh_asset_id,
                .model_ubo_offset = model_ubo_offset,
                .model_ubo_descriptor_set = collections[collection_id].model_ubo_desc_set,
                .param_ubo_offsets=std::move(param_dynamic_offsets),
                .param_descriptor_set = param_descriptor_sets
            };

            collections[collection_id].pipeline_bindings_[
                asset_pipeline.Get_asset_id().GetId()
                ].Insert(binding_set_id.GetId(), std::move(binding));
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

        void EnsureModelUboDescriptorSet(
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
                        collections[collection_id].ubo_manager,
                        collections[collection_id].descriptor_pool,
                        *model_ubo_layout
                        );

                collections[collection_id].model_ubo_desc_set = descriptors;
            }
        }

        std::vector<std::uint32_t> EnsureParamDescriptorSet(
            std::size_t collection_id,
            wcr::wid::WEngId binding_set_id,
            was::RenderPipeline const & asset_pipeline,
            was::RenderPipelineParams const & asset_params,
            wvk::raii::AssetRenderData & asset_data
            ) {

            auto ubo_params = wvk::raii::pipelines::desc_bindings
                ::CollectUBOBindings<FramesInFlight>(
                    binding_set_id,
                    asset_pipeline,
                    asset_params,
                    collections[collection_id].ubo_manager
                    );

            if (!collections[collection_id].param_descriptors.contains(
                    asset_params.Get_asset_id())) {
                
                auto texture_params = wvk::raii::pipelines::desc_bindings
                    ::CollectTextureBindings(
                        asset_params,
                        asset_data
                        );

                auto descriptors =wvk::raii::pipelines::desc_bindings
                    ::CreateParamsDescriptorSet(
                        vkn_.device,
                        param_layouts.Get(asset_pipeline.Get_asset_id().GetId()),
                        collections[collection_id].descriptor_pool,
                        texture_params,
                        ubo_params
                        );

                collections[collection_id].param_descriptors[asset_params.Get_asset_id()] =
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

        struct {
            VkDevice device{VK_NULL_HANDLE};
            VkPhysicalDevice physical_device{VK_NULL_HANDLE};
        } vkn_;

        wvk::raii::DescriptorSetLayout<1> model_ubo_layout;

        TObjectDataBase<VkDescriptorSetLayout, void, std::size_t>
        param_layouts;

        TObjectDataBase<std::tuple<VkPipeline, VkPipelineLayout>, void, std::size_t>
        pipelines;

        std::unordered_map<
            std::size_t,
            wvk::raii::pipelines::gbuffer_lib::DescriptorCollection<FramesInFlight>>
        collections{};

    };

}

