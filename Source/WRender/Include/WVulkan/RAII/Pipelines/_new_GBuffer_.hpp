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
#include "WVulkan/RAII/Pipelines/_new_GBuffer_lib.hpp"
#include "WVulkan/RAII/DescriptorSetLayout.hpp"

#include <vulkan/vulkan_core.h>

namespace wvk::raii::pipelines {

    /**
     * @brief Graphics Pipelines outputs the GBuffers.
     */
    template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
    class _new_GBuffer
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
        
        static inline constexpr const wcr::wid::WEngId MODEL_UBO_DESCRIPTOR_BINIDNG_ID
          {wcr::wid::null_id};
        
        static inline constexpr const VkDescriptorSetLayoutBinding MODEL_UBO_LAYOUT_BINDING{
            .binding=wct::render::MODEL_UBO_BINDING,
            .descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .descriptorCount=1,
            .stageFlags=VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers=nullptr,
        };

        static inline wct::render::RPipeParamDescriptor GetModelDescriptor(
            wct::render::RPipeParamDescriptorsLayout const & descriptors
            ) {
            for(auto & desc : descriptors ) {
                if (desc.set == wct::render::MODEL_UBO_DESC_SET &&
                    desc.binding == wct::render::MODEL_UBO_BINDING) {
                    return desc;
                }
            }

            return wct::render::ModelParamDescriptor_Dynamic();
        }

        static inline constexpr wct::render::RPipeParamUbo GetDefaultModelParam() {
            return {
                .set=wct::render::MODEL_UBO_DESC_SET,
                .binding=wct::render::MODEL_UBO_BINDING,
                .data=wct::render::ToUBOData(wct::render::ModelUBO())
            };
        }

        _new_GBuffer() noexcept=default;
        ~_new_GBuffer() = default;
        _new_GBuffer(const _new_GBuffer&)=delete;
        _new_GBuffer & operator=(const _new_GBuffer&) = delete;
        _new_GBuffer(_new_GBuffer && other) noexcept =default;
        _new_GBuffer & operator=(_new_GBuffer && other) noexcept =default;

        _new_GBuffer(VkDevice device, VkPhysicalDevice physical_device) :
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
                [](auto id) {return VK_NULL_HANDLE;},
                [device](auto pipeline) { wvk::descriptor::Destroy(pipeline,device); }
                ),
            collections_()
            {}

    public:

        void CreatePipeline(
            wcr::wid::WEngId pipeline_id,
            wct::render::ShaderList const & shader_list, 
            wct::render::RPipeParamDescriptorsLayout const & descriptor_layout_list,
            VkDescriptorSetLayout global_descset_layout
            ) {

            assert(descriptor_set_layouts.Contains(pipeline_asset.Get_asset_id().GetId()));

            auto shaders_info =
                wvk::shader::ToShaderStageInfo(shader_list);

            auto layoyt_binding =
                wvk::descriptor::ToDescriptorSetLayoutBinding(
                    descriptor_layout_list
                    );

            param_layouts_.CreateAt(
                pipeline_id.GetId(),
                [this, &layoyt_binding](auto id){
                    return wvk::descriptor::CreateDescriptorSetLayout(
                        layoyt_binding,
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
            wct::render::RPipeParamDescriptor const & model_descriptor,
            wct::render::RPipeParamUbo const & model_ubo_param,
            wct::render::RPipeParamDescriptorsLayout const & param_descriptors,
            wct::render::RPipeParamList_Ubo const & ubo_params,
            wct::render::RPipeParamList_WAssetId const & texture_params,
            wvk::raii::AssetRenderData & asset_data
            ){

            EnsureCollection(collection_id);

            model_collections_[collection_id].CreateBindingSet(
                binding_set_id,
                pipeline_id,
                MODEL_UBO_DESCRIPTOR_BINIDNG_ID,
                {model_descriptor},
                {model_ubo_param}, {},
                *model_ubo_layout_,
                asset_data
                );

            collections_[collection_id].CreateBindingSet(
                binding_set_id,
                pipeline_id,
                descriptor_bindings_id,
                param_descriptors,
                ubo_params,
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
            if (param_descriptor.set == wct::render::MODEL_UBO_DESC_SET) {
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
            if (param_descriptor.set == wct::render::MODEL_UBO_DESC_SET) {
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
        GetPipeline(wcr::wid::WAssetId pipeline_id) const {
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

        auto Bindings(wcr::wid::WAssetId pipeline_id, std::size_t collection_id) const {
            return std::tuple {
                collections_.at(collection_id).pipeline_bindings.at(pipeline_id),
                model_collections_.at(collection_id).pipeline_bindings.at(pipeline_id)
            };
        }

        // template<typename Fn>
        // auto ForEachBinding(wcr::wid::WAssetId pipeline_id, Fn && fn) {
            
        // }

    private:

        void EnsureCollection(std::size_t id) {
            if(!collections_.contains(id)) {
                collections_[id]={
                    .descriptor_pool{{vkn_.device}},
                    .ubo_manager{
                        vkn_.device, 
                        vkn_.physical_device, 
                        wvk::raii::ubo_manager::INITIAL_UBO_COUNT
                    }
                };
                model_collections_[id] = {
                    .descriptor_pool{{vkn_.device}},
                    .ubo_manager{
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
        CollectionsMap model_collections_{};

    };

}

