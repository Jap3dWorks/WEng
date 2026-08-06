#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/Vk/WVkDescriptor.hpp"
#include "WVulkan/RAII/Pipelines/_new_Postprocess_lib.hpp"
#include "WVulkan/RAII/DescriptorPool.hpp"
#include "WVulkan/Vk/WVkPipeline.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
// #include "GBufferBindings.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace wck::raii::pipelines {

    template<std::uint8_t FramesInFlight>
    class _new_Postprocess {

    public:

        _new_Postprocess() = default;

        _new_Postprocess(
            VkDevice device,
            VkPhysicalDevice physical_device
            ) :
            vkn_(device, physical_device),
            descriptor_pool_({device}),
            param_layouts_(
                [] (auto id) { return VK_NULL_HANDLE; },
                [device] (auto layout) { wvk::descriptor::Destroy(layout, device); }
                ),
            pipelines_(
                [] (auto id) -> std::tuple<VkPipeline, VkPipelineLayout> { return {}; },
                [device] (auto & pipeline__layout) {
                    wvk::pipeline::Destroy(std::get<0>(pipeline__layout, device));
                    wvk::pipeline::Destroy(std::get<1>(pipeline__layout, device));
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
            VkDescriptorSetLayout global_descset_layout
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
                [this, &global_descset_layout, &shaders_info]
                (auto id) {
                    return wvk::raii::pipelines::postprocess_lib
                        ::CreatePipeline(
                            vkn_.device,
                            {
                                global_descset_layout,
                                param_layouts_.Get(id)
                            },
                            shaders_info
                            );
                }
                );
        }

        void DeletePipeline(
            wcr::wid::WEngId pipeline_id
            ) {
            // TODO
        }

        void CreateBindingSet(
            wcr::wid::WEngId binding_set_id,
            wcr::wid::WEngId pipeline_id,
            wcr::wid::WEngId descriptor_bindings_id,
            wct::render::RPipeParamDescriptorsLayout param_descriptors,
            wct::render::RPipeParamList_Ubo pipe_ubo_params,
            wct::render::RPipeParamList_WAssetId const & texture_params,
            wvk::raii::AssetRenderData & asset_data
            ) {
            assert(pipelines_.Contains(pipeline_id.GetId()));

            

        }

        void UpdateBindingSet();

    private:

        struct {
            VkDevice device{VK_NULL_HANDLE};
            VkPhysicalDevice physical_device{VK_NULL_HANDLE};
        } vkn_;

        wvk::raii::DescriptorPool<
            0,
            2 * wct::render::MAX_PIPELINE_ASSINGMENTS,
            16 * wct::render::MAX_PIPELINE_ASSINGMENTS,
            (2 + 16) * wct::render::MAX_PIPELINE_ASSINGMENTS
            > descriptor_pool_{};

        TObjectDataBase<VkDescriptorSetLayout, void, std::size_t>
        param_layouts_;

        TObjectDataBase<std::tuple<VkPipeline, VkPipelineLayout>, void, std::size_t>
        pipelines_;

        // wvk::raii::pipelines::gbuffer::BindingCollection<FramesInFlight>
        // collection_{};

        

    };
}
