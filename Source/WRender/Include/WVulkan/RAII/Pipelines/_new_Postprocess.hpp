#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WVulkan/Vk/WVkDescriptor.hpp"
#include "WVulkan/RAII/Pipelines/_new_Postprocess_lib.hpp"
// #include "BindingCollection.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace wck::raii::pipelines {

    template<std::uint8_t FramesInFlight>
    class _new_Postprocess {

    public:

        _new_Postprocess() = default;

        _new_Postprocess(
            VkDevice in_device,
            VkPhysicalDevice in_physical_device
            ) : vkn_(in_device, in_physical_device)
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

        void CreateBindingSet();

        void UpdateBindingSet();

    private:

        struct {
            VkDevice device{VK_NULL_HANDLE};
            VkPhysicalDevice physical_device{VK_NULL_HANDLE};
        } vkn_;

        TObjectDataBase<VkDescriptorSetLayout, void, std::size_t>
        param_layouts_;

        TObjectDataBase<std::tuple<VkPipeline, VkPipelineLayout>, void, std::size_t>
        pipelines_;

        // wvk::raii::pipelines::BindingCollection<FramesInFlight>
        // collection_{};

        
        

    };
}
