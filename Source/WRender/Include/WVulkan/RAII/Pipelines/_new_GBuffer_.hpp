#pragma once

#include "WAssets/RenderPipelineParams.hpp"
#include "WCore/WCore.hpp"
#include "WCore/WDebug.hpp"
#include "WCore/WId.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WCore/TIterator.hpp"
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

        using CollectionsMap = std::unordered_map<
            std::size_t,
            wvk::raii::pipelines::gbuffer_lib::DescriptorCollection<FramesInFlight>>;

        template<typename ValueFn, typename IncrFn>
        using CollectionsIterator = TIterator<
            typename CollectionsMap::value_type const,
            typename CollectionsMap::const_iterator,
            std::size_t, ValueFn, IncrFn
            >;


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
            const was::RenderPipeline & pipeline_asset,
            VkDescriptorSetLayout global_descset_layout
            ) {

            assert(descriptor_set_layouts.Contains(pipeline_asset.Get_asset_id().GetId()));

            auto shaders_info =
                wvk::shader::ToShaderStageInfo(pipeline_asset.Get_shader_list());

            auto layoyt_binding =
                wvk::descriptor::ToDescriptorSetLayoutBinding(
                    pipeline_asset.Get_descriptor_list()
                    );

            param_layouts_.CreateAt(
                pipeline_asset.Get_asset_id().GetId(),
                [this, &layoyt_binding](auto id){
                    return wvk::descriptor::CreateDescriptorSetLayout(
                        layoyt_binding,
                        vkn_.device
                        );
                }
                );

            pipelines_.CreateAt(
                pipeline_asset.Get_asset_id().GetId(),
                [this, &global_descset_layout, &shaders_info]
                (auto id) {
                    return wvk::raii::pipelines::gbuffer_lib::CreatePipeline(
                        vkn_.device,
                        {
                            global_descset_layout,
                            *model_ubo_layout_, // ubo model at desc set 1
                            param_layouts_.Get(id)
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
            wvk::raii::AssetRenderData & asset_data,
            wcr::wid::WTypeAssetIndexId in_mesh_asset_id
            ){

            EnsureCollection(collection_id);

            wcr::wid::WEngId::IdType model_ubo_id = wct::render::ApplyPipeParamType(
                binding_set_id,
                wct::render::ERPipeParamType::UBO_Entity_Dynamic
                ).GetId();

            collections_[collection_id].CreateBindingSet(
                binding_set_id,
                model_ubo_id,
                *model_ubo_layout_,
                MODEL_UBO_BINDING,
                asset_pipeline,
                asset_params,
                param_layouts_.Get(asset_pipeline.Get_asset_id().GetId()),
                asset_data,
                in_mesh_asset_id
                );
        }

        void DeleteBindingSet(
            std::size_t collection_id,
            wcr::wid::WEngId binding_set_id
            ) {
            // TODO
        }

        void DeleteBindingSetCollection(
            std::size_t collection_id
            ) {
            // TODO
        }

        void UpdateBindingSetParameter(
            std::size_t collection_id,
            wcr::wid::WEngId binding_set_id,
            was::RenderPipeline const & pipeline,
            wct::render::RPipeParamUbo const & ubo_pipe_param,
            std::uint8_t frame_index
            ) {
            // TODO
            
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
            return collections_.at(collection_id).pipeline_bindings.at(pipeline_id);
        }

        template<typename Fn>
        auto ForEachBinding(wcr::wid::WAssetId pipeline_id, Fn && fn) {
            
        }

        // TODO Destroy collection

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

    };

}

