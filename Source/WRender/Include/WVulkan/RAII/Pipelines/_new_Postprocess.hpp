#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/Pipelines/ParamBindings.hpp"
#include "WVulkan/Vk/WVkDescriptor.hpp"
#include "WVulkan/RAII/Pipelines/_new_Postprocess_lib.hpp"
#include "WVulkan/RAII/Pipelines/_new_PostprocessBindings.hpp"
#include "WVulkan/Vk/WVkPipeline.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
// #include "GBufferBindings.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace wvk::raii::pipelines {

    template<std::uint8_t FramesInFlight>
    class _new_Postprocess {

    public:

        _new_Postprocess() = default;

        _new_Postprocess(
            VkDevice device,
            VkPhysicalDevice physical_device
            ) :
            vkn_(device, physical_device),
            binding_collection_{
                .device=device,
                .physical_device={physical_device},
                .descriptor_pool={{device}}
            },
            param_layouts_(
                [] (auto id) { return VK_NULL_HANDLE; },
                [device] (auto layout) { wvk::descriptor::Destroy(layout, device); }
                ),
            pipelines_(
                [] (auto id) -> std::tuple<VkPipeline, VkPipelineLayout> { return {}; },
                [device] (auto & pipeline__layout) {
                    wvk::pipeline::Destroy(std::get<0>(pipeline__layout), device);
                    wvk::pipeline::Destroy(std::get<1>(pipeline__layout), device);
                }
                )
            {}

        _new_Postprocess(const _new_Postprocess&) = delete;
        _new_Postprocess& operator=(const _new_Postprocess&) = delete;

        _new_Postprocess(_new_Postprocess&&) = default;
        _new_Postprocess& operator=(_new_Postprocess&&) = default;

        ~_new_Postprocess() = default;

        void CreatePipeline(
            wcr::wid::WEngId pipeline_id,
            wct::render::ShaderList const & shader_list,
            wct::render::RPipeParamDescriptorsLayout all_param_descriptors,
            VkDescriptorSetLayout global_descset_layout,
            VkDescriptorSetLayout global_ppcess_descset_layout
            ) {

            assert(!pipelines_.Contains(pipeline_id.GetId()));

            auto shaders_info =
                wvk::shader::ToShaderStageInfo(shader_list);

            auto layout_binding =
                wvk::descriptor::ToDescriptorSetLayoutBinding(
                    all_param_descriptors,
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER  // use common uniform buffers for ppcess
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
                [this, &global_descset_layout, &global_ppcess_descset_layout, &shaders_info]
                (auto id) {
                    return wvk::raii::pipelines::postprocess_lib
                        ::CreatePipeline(
                            vkn_.device,
                            {
                                global_descset_layout,
                                param_layouts_.Get(id),
                                global_ppcess_descset_layout
                            },
                            shaders_info
                            );
                }
                );
        }

        void DeletePipeline(
            wcr::wid::WEngId pipeline_id
            ) {
            // TODO Remove pipeline and all related bindings.
            
        }

        std::tuple<VkPipeline, VkPipelineLayout> GetPipeline(wcr::wid::WEngId pipeline_id) const {
            return pipelines_.Get(pipeline_id.GetId());
        }

        void CreateBindingSet(
            wcr::wid::WEngId binding_set_id,
            wcr::wid::WEngId pipeline_id,
            wct::render::RPipeParamDescriptorsLayout descriptors_layout,
            wct::render::RPipeParamList_Ubo ubo_params,
            wct::render::RPipeParamList_WAssetId const & texture_params,
            wvk::raii::AssetRenderData & asset_data
            ) {
            assert(pipelines_.Contains(pipeline_id.GetId()));

            binding_collection_.CreateBindingSet(
                binding_set_id,
                pipeline_id,
                descriptors_layout,
                ubo_params,
                texture_params,
                param_layouts_.Get(pipeline_id.GetId()),
                asset_data
                );

            bindings_order_.push_back(binding_set_id);
        }

        void DeleteBindingSet(
            wcr::wid::WEngId pipeline_id,
            wcr::wid::WEngId binding_set_id
            ) {
            // TODO
        }

        void UpdateBindingSetParameter(
            std::uint8_t frame_index,
            wcr::wid::WEngId render_pipeline_id,
            wcr::wid::WEngId binding_set_id,
            wct::render::RPipeParamUbo const & ubo_param_data
            ) {
            
            binding_collection_.
                template UpdateBindingSetParameter
                <wvk::raii::pipelines::param_bindings::EUpdateType::DYNAMIC>
                (
                    frame_index,
                    render_pipeline_id,
                    binding_set_id,
                    ubo_param_data
                );
        }

        void UpdateBindingSetParameter(
            wcr::wid::WEngId render_pipeline_id,
            wcr::wid::WEngId binding_set_id,
            wct::render::RPipeParamUbo const & ubo_param_data
            ) {
            
            binding_collection_.
                template UpdateBindingSetParameter
                <wvk::raii::pipelines::param_bindings::EUpdateType::STATIC>
                (
                    0,
                    render_pipeline_id,
                    binding_set_id,
                    ubo_param_data
                );
        }
        
        wvk::raii::pipelines::postprocess::Binding<FramesInFlight>
        GetBinding(
            wcr::wid::WEngId binding_set_id
            ) const {
            return binding_collection_.bindings.at(binding_set_id.GetId());
        }

        void ClearBindingsOrder() {
            bindings_order_.clear();
        }

        std::vector<wcr::wid::WEngId> const & BindingsOrder() const{
            return bindings_order_;
        }

    private:

        struct {
            VkDevice device{VK_NULL_HANDLE};
            VkPhysicalDevice physical_device{VK_NULL_HANDLE};
        } vkn_;

        TObjectDataBase<VkDescriptorSetLayout, void, std::size_t>
        param_layouts_;

        TObjectDataBase<std::tuple<VkPipeline, VkPipelineLayout>, void, std::size_t>
        pipelines_;

        wvk::raii::pipelines::postprocess::BindingCollection<FramesInFlight>
        binding_collection_;

        std::vector<wcr::wid::WEngId> bindings_order_;

    };
}
