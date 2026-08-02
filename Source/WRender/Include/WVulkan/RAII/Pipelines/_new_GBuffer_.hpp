#pragma once

#include "WAssets/RenderPipelineParams.hpp"
#include "WCore/WCore.hpp"
#include "WCore/WDebug.hpp"
#include "WCore/WId.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WLog.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WVulkan/RAII/UBOManager/DynamicUBOManager.hpp"
#include "WVulkan/Vk/WVkDescriptor.hpp"
#include "WVulkan/Vk/WVkShader.hpp"
#include "WVulkan/Vk/WVkPipeline.hpp"
#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WAssets/RenderPipeline.hpp"
#include "WVulkan/RAII/Pipelines/_new_GBuffer_lib.hpp"
#include "WVulkan/RAII/DescriptorSetLayout.hpp"


#include <vector>
#include <vulkan/vulkan_core.h>
#include "WVulkan/RAII/Pipelines/WVkPipelinesBase.hpp"


namespace wvk::raii::pipelines {

    /**
     * @brief Graphics Pipelines outputs the GBuffers.
     */
    template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
    class GBuffer : public WVkPipelinesBase<wcr::wid::WAssetId,
                                            wcr::wid::WEntityComponentId,
                                            FramesInFlight>
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

        GBuffer() noexcept=default;
        ~GBuffer() override = default;
        GBuffer(const GBuffer&)=delete;
        GBuffer & operator=(const GBuffer&) = delete;
        GBuffer(GBuffer && other) noexcept =default;
        GBuffer & operator=(GBuffer && other) noexcept =default;

        GBuffer(VkDevice device, VkPhysicalDevice physical_device) :
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
            // wcr::wid::WEntityComponentId binding_set_id,
            wcr::wid::WAssetId in_pipeline_id,
            was::RenderPipelineParams pipeline_params,
            wcr::wid::WTypeAssetIndexId in_mesh_asset_id,
            wvk::raii::AssetRenderData & asset_data,
            
            // std::vector<WVkDescSetUBOBinding<FramesInFlight>> in_ubos,
            // std::vector<WVkDescSetTextureBinding> in_textures
            ){

            EnsureCollection(collection_id);
            
            wvk::raii::pipelines::gbuffer_lib::Binding<FramesInFlight> binding{
                .mesh_id{in_mesh_asset_id},

            };

            

            // TODO 1 local descriptor by pipeline_id x param_asset_id x ubo buffers (binding order)
            // store in a tree structure

            WVkRenderPipeline pipeline_info = Super::Pipeline(in_pipeline_id);

            WCORE_DEBUG_ONLY(
                for(auto & ubo : in_ubos) {
                    WFLOG("UBO Binding: {}", ubo.binding);
                }
                )

                Super::pipelines_db_.pipe_bindings.Insert(
                    binding_set_id,
                    WVkPipelineBinding{
                        .pipeline_id = in_pipeline_id,
                        .mesh_asset_id = in_mesh_asset_id,
                        .ubos = std::move(in_ubos),
                        .textures = std::move(in_textures)
                    }
                    );

            Super::pipeline_bindings_[in_pipeline_id]
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

        std::unordered_map<
            wcr::wid::WAssetId::IdType,
            TSparseSet<wcr::wid::WEntityComponentId>> pipeline_bindings_{};


        // collection : pool | buffer | bindings
        std::unordered_map<
            std::size_t,
            wvk::raii::pipelines::gbuffer_lib::Collection<FramesInFlight>>
        collections{};

    };

}

