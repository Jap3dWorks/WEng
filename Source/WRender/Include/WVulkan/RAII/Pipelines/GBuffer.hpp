#pragma once

#include "WCore/WCore.hpp"
#include "WAssets/RenderPipelineParams.hpp"
#include "WCore/WId.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WCore/TIterator.hpp"
#include "WLog.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WVulkan/RAII/Pipelines/BindingCollection.hpp"
#include "WVulkan/Vk/WVkDescriptor.hpp"
#include "WVulkan/Vk/WVkShader.hpp"
#include "WVulkan/WVkConfig.hpp"
#include "WAssets/RenderPipeline.hpp"
#include "WVulkan/RAII/Pipelines/GBuffer_lib.hpp"
#include "WVulkan/RAII/DescriptorSetLayout.hpp"

#include <vulkan/vulkan_core.h>

namespace wvk::raii::pipelines {

    /**
     * @brief Graphics Pipelines outputs the GBuffers.
     */
    template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
    class GBuffer
    {

        using CollectionsMap = std::unordered_map<
            std::size_t,
            wvk::raii::pipelines::DescriptorCollection<FramesInFlight>>;

        template<typename ValueFn, typename IncrFn>
        using CollectionsIterator = TIterator<
            typename CollectionsMap::value_type const,
            typename CollectionsMap::const_iterator,
            std::size_t, ValueFn, IncrFn
            >;

    public:

        static inline constexpr const std::uint8_t PARAM_DESC_SET{1};
        
        static inline constexpr const wcr::wid::WEngId MODEL_UBO_DESCRIPTOR_BINDING_ID
          {wcr::wid::nullid};
        
        static inline constexpr const VkDescriptorSetLayoutBinding MODEL_UBO_LAYOUT_BINDING{
            .binding=wct::render::CommonBindings::MODEL_UBO,
            .descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .descriptorCount=1,
            .stageFlags=VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers=nullptr,
        };

        // static inline wct::render::RPipeParamDescriptor GetModelDescriptor(
        //     wct::render::RPipeParamDescriptorsLayout const & descriptors
        //     ) {
        //     for(auto & desc : descriptors ) {
        //         if (desc.set == wct::render::CommonBindings::MODEL_SET &&
        //             desc.binding == wct::render::CommonBindings::MODEL_UBO) {
        //             return desc;
        //         }
        //     }

        //     return wct::render::ModelParamDescriptor_Dynamic();
        // }

        static inline constexpr wct::render::RPipeParamUbo GetDefaultModelParam() {
            return {
                .set=wct::render::CommonBindings::MODEL_SET,
                .binding=wct::render::CommonBindings::MODEL_UBO,
                .data=wct::render::ToUBOData(wct::render::ModelUBO())
            };
        }

        WNODISCARD static inline constexpr
        auto SplitDescriptors(wct::render::RPipeParamDescriptorsLayout param_descriptors) {
            std::int32_t index=-1;
            std::int32_t last=param_descriptors.size()-1;

            wct::render::RPipeParamDescriptor model_descriptor =
                wct::render::ModelParamDescriptor_Dynamic();

            for(std::int32_t i=0; i < param_descriptors.size(); ++i ) {
                if (param_descriptors[i].type == wct::render::ERPipeParamType::None) {
                    last = i-1;
                    break;
                }
                if (param_descriptors[i].set == wct::render::CommonBindings::MODEL_SET &&
                    param_descriptors[i].binding == wct::render::CommonBindings::MODEL_UBO
                    ) {
                    index=i;
                }
            }

            if (index > -1) {
                model_descriptor = param_descriptors[index];
                param_descriptors[index] = param_descriptors[last];
                param_descriptors[last] = { .type=wct::render::ERPipeParamType::None };
            }

            return std::tuple{std::move(param_descriptors), std::move(model_descriptor)};
        }

        WNODISCARD static inline constexpr
        auto SplitUBOParams(wct::render::RPipeParamList_Ubo ubo_params) {
            std::int32_t index=-1;

            wct::render::RPipeParamUbo model_ubo=GetDefaultModelParam();

            for(std::int32_t i=0; i < ubo_params.size(); ++i ) {
                if (ubo_params[i].set == wct::render::CommonBindings::MODEL_SET &&
                    ubo_params[i].binding == wct::render::CommonBindings::MODEL_UBO
                    ) {
                    index=i;
                }
            }

            if (index > -1) {
                model_ubo = std::move(ubo_params[index]);
                ubo_params[index]=ubo_params.back();
                ubo_params.resize(ubo_params.size()-1);
            }

            return std::tuple{std::move(ubo_params), std::move(model_ubo)};
        }

        GBuffer() noexcept=default;
        ~GBuffer() = default;
        GBuffer(const GBuffer&)=delete;
        GBuffer & operator=(const GBuffer&) = delete;
        GBuffer(GBuffer && other) noexcept =default;
        GBuffer & operator=(GBuffer && other) noexcept =default;

        GBuffer(VkDevice device, VkPhysicalDevice physical_device) :
            vkn_(device, physical_device),
            model_ubo_layout_(
                {device},
                std::array{MODEL_UBO_LAYOUT_BINDING}
                ),
            param_layouts_(
                [](auto id) {return VK_NULL_HANDLE;},
                [device](auto layout) { wvk::descriptor::Destroy(layout, device); }
                ),
            pipelines_(
                [](auto id) -> std::tuple<VkPipeline, VkPipelineLayout> {return {};},
                [device](auto & pipeline__layout) {
                    wvk::pipeline::Destroy(std::get<0>(pipeline__layout), device);
                    wvk::pipeline::Destroy(std::get<1>(pipeline__layout), device);
                }
                ),
            collections_()
            {}

    public:

        void CreatePipeline(
            wcr::wid::WEngId pipeline_id,
            wct::render::ShaderList const & shader_list, 
            wct::render::RPipeParamDescriptorsLayout all_param_descriptors,
            VkDescriptorSetLayout global_descset_layout
            ) {

            assert(!param_layouts_.Contains(pipeline_id.GetId()));

            auto shaders_info =
                wvk::shader::ToShaderStageInfo(shader_list);

            auto [param_descriptors, model_descriptor] =
                SplitDescriptors(std::move(all_param_descriptors));

            auto layout_binding =
                wvk::descriptor::ToDescriptorSetLayoutBinding(
                    param_descriptors
                    );

            param_layouts_.CreateAt(
                pipeline_id.GetId(),
                [this, &layout_binding](auto id){
                    return wvk::descriptor::CreateDescriptorSetLayout(
                        layout_binding,
                        vkn_.device
                        );
                }
                );

            pipelines_.CreateAt(
                pipeline_id.GetId(),
                [this, &global_descset_layout, &shaders_info]
                (auto id) {
                    return wvk::raii::pipelines::gbuffer_lib::CreatePipeline(
                        vkn_.device,
                        {
                            global_descset_layout,
                            param_layouts_.Get(id),
                            *model_ubo_layout_,
                        },
                        {
                            shaders_info
                        }
                        );
                }
                );
        }

        void DeletePipeline(
            wcr::wid::WEngId pipeline_id
            ) {
            // TODO delete all bindings and descriptors associated with pipeline_id
        }

        void CreateBindingSet(
            std::size_t collection_id,               // usually the level id
            wcr::wid::WEngId binding_set_id,         // usually the subcomponent id
            wcr::wid::WEngId pipeline_id,            // the asset pipeline id
            wcr::wid::WEngId descriptor_bindings_id, // usually the asset param id
            wcr::wid::WEngId renderable_asset_id,
            wct::render::RPipeParamDescriptorsLayout param_descriptors,
            wct::render::RPipeParamList_Ubo pipe_ubo_params,
            wct::render::RPipeParamList_WAssetId const & texture_params,
            wvk::raii::AssetRenderData & asset_data
            ){

            EnsureCollection(collection_id);

            auto [param_desc, model_desc] =
                SplitDescriptors(std::move(param_descriptors));

            auto [param_ubos, model_ubo] =
                SplitUBOParams(pipe_ubo_params);

            model_collections_[collection_id].CreateBindingSet(
                binding_set_id,
                pipeline_id,
                MODEL_UBO_DESCRIPTOR_BINDING_ID,   // <- all model ubos in the same descriptor
                renderable_asset_id,
                {model_desc},
                {model_ubo},
                {},
                *model_ubo_layout_,
                asset_data
                );


            collections_[collection_id].CreateBindingSet(
                binding_set_id,
                pipeline_id,
                descriptor_bindings_id,
                renderable_asset_id,
                param_desc,
                param_ubos,
                texture_params,
                param_layouts_.Get(pipeline_id.GetId()),
                asset_data
                );
        }

        void DeleteBindingSet(
            std::size_t collection_id,
            wcr::wid::WEngId render_pipeline_id,
            wcr::wid::WEngId binding_set_id
            ) {
            collections_[collection_id].DeleteBindingSet(
                render_pipeline_id,
                binding_set_id);

            model_collections_[collection_id].DeleteBindingSet(
                render_pipeline_id,
                binding_set_id
                );
        }

        void DeleteBindingSetCollection(
            std::size_t collection_id
            ) {
            collections_.erase(collection_id);
            model_collections_.erase(collection_id);
        }

        void UpdateBindingSetParameter(
            std::size_t collection_id,
            wcr::wid::WEngId render_pipeline_id,
            wcr::wid::WEngId binding_set_id,
            wct::render::RPipeParamDescriptor const & param_descriptor,
            wct::render::RPipeParamUbo const & ubo_data
            ) {
            if (param_descriptor.set == wct::render::CommonBindings::MODEL_SET) {
                model_collections_[collection_id]
                    .UpdateBindingSetParameter(
                        render_pipeline_id,
                        binding_set_id,
                        param_descriptor,
                        ubo_data
                        );
            } else {
                collections_[collection_id]
                    .UpdateBindingSetParameter(
                        render_pipeline_id,
                        binding_set_id,
                        param_descriptor,
                        ubo_data
                        );
            }
        }

        void UpdateBindingSetParameter(
            std::uint8_t frame_index,
            std::size_t collection_id,
            wcr::wid::WEngId render_pipeline_id,
            wcr::wid::WEngId binding_set_id,
            wct::render::RPipeParamDescriptor const & param_descriptor,
            wct::render::RPipeParamUbo const & ubo_data
            ) {
            if (param_descriptor.set == wct::render::CommonBindings::MODEL_SET) {
                model_collections_[collection_id]
                    .UpdateBindingSetParameter(
                        frame_index,
                        render_pipeline_id,
                        binding_set_id,
                        param_descriptor,
                        ubo_data
                        );
            } else {
                collections_[collection_id]
                    .UpdateBindingSetParameter(
                        frame_index,
                        render_pipeline_id,
                        binding_set_id,
                        param_descriptor,
                        ubo_data
                        );
            }
        }
        
        auto IterPipelines() const {
            return pipelines_.IterIndexes();
        }

        WNODISCARD std::tuple<VkPipeline, VkPipelineLayout> 
        GetPipeline(wcr::wid::WEngId pipeline_id) const {
            return pipelines_.Get(pipeline_id.GetId());
        }

        auto IterCollections() const {
            return CollectionsIterator {
                collections_.cbegin(),
                collections_.cend(),
                [](auto it, std::size_t incr) { return (*it).first; },
                [](auto it, std::size_t incr) { it++; return it; }
            };
        }

        auto Bindings(
            std::size_t collection_id,
            wcr::wid::WEngId pipeline_id
            ) const
            {
                return collections_.at(collection_id).pipeline_bindings.at(pipeline_id.GetId());
            }

        auto GetUboBinding(
            std::size_t collection_id,
            wcr::wid::WEngId pipeline_id,
            wcr::wid::WEngId binding_set_id
            ) const {
            return model_collections_.at(collection_id)
                .pipeline_bindings.at(pipeline_id.GetId())
                .Get(binding_set_id.GetId());
        }

    private:


        void EnsureCollection(std::size_t id) {
            if(!collections_.contains(id)) {
                collections_[id]={
                    .descriptor_pool={{vkn_.device}},
                    .ubo_manager = {
                        vkn_.device, 
                        vkn_.physical_device, 
                        wvk::raii::ubo_manager::INITIAL_UBO_COUNT
                    }
                };
                model_collections_[id] = {
                    .descriptor_pool={{vkn_.device}},
                    .ubo_manager={
                        vkn_.device, 
                        vkn_.physical_device, 
                        wvk::raii::ubo_manager::INITIAL_UBO_COUNT
                    }
                };
            }
        }

        struct {
            VkDevice device{VK_NULL_HANDLE};
            VkPhysicalDevice physical_device{VK_NULL_HANDLE};
        } vkn_;

        wvk::raii::DescriptorSetLayout<1> model_ubo_layout_;

        TObjectDataBase<VkDescriptorSetLayout, void, std::size_t>
        param_layouts_;

        TObjectDataBase<std::tuple<VkPipeline, VkPipelineLayout>, void, std::size_t>
        pipelines_;

        CollectionsMap collections_{};

        // TODO simpler class for model ubos.
        // with faster associative access.
        CollectionsMap model_collections_{};

    };

}

