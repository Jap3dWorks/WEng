#pragma once

#include "WCore/WId.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/Pipelines/ParamBindings.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WVulkan/Vk/WVkBuffer.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/RAII/DescriptorPool.hpp"

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>
#include <vector>

namespace wvk::raii::pipelines::postprocess {

    template<std::uint8_t FramesInFlight>
    struct Binding {
        wcr::wid::WEngId pipeline_id;
        std::array<VkDescriptorSet, FramesInFlight> descriptor_set;
    };

    template<std::uint8_t FramesInFlight>
    struct BindingCollection {

        VkDevice device{VK_NULL_HANDLE};
        VkPhysicalDevice physical_device{VK_NULL_HANDLE};

        wvk::raii::DescriptorPool<
            0,
            2 * wct::render::MAX_PIPELINE_ASSINGMENTS,
            16 * wct::render::MAX_PIPELINE_ASSINGMENTS,
            (2 + 16) * wct::render::MAX_PIPELINE_ASSINGMENTS
            > descriptor_pool{};

        struct BuffersContainer {

            ~BuffersContainer() {

                if (VK_NULL_HANDLE == out_self->device) return;

                for(auto & p: buffers) {
                    for(auto b : p.second) {
                        wvk::buffer::Destroy(b, out_self->device);
                    }
                }
            }
            
            BindingCollection * out_self{nullptr};
            std::unordered_map<std::size_t, std::array<WVkBuffer, FramesInFlight>> buffers{};
            
        } ubo_buffers{this, {}};

        // Binding_set_id : Binding data
        std::unordered_map<std::size_t, Binding<FramesInFlight>> bindings{};

        std::unordered_map<
            std::size_t,
            std::unordered_map<
                std::uint8_t,
                std::uint32_t>> pipeline_binding_offset{};

        void CreateBindingSet(
            wcr::wid::WEngId binding_set_id,  // camera component id
            wcr::wid::WEngId pipeline_id,
            wct::render::RPipeParamDescriptorsLayout const & param_descriptors,
            wct::render::RPipeParamList_Ubo const & ubo_params,
            wct::render::RPipeParamList_WAssetId const & texture_params,
            VkDescriptorSetLayout descriptor_set_layout,
            wvk::raii::AssetRenderData & asset_data
            ) {

            auto texture_bindings = wvk::raii::pipelines::param_bindings
                ::CollectTextureBindings(
                    texture_params,
                    asset_data
                    );

            auto ubo_bindings = wvk::raii::pipelines::param_bindings
                ::CollectUBOBindings<FramesInFlight>(
                    descriptor_pool.Creator().device,
                    physical_device,
                    param_descriptors,
                    ubo_params                    
                    );

            if (!ubo_bindings.empty()) {
                ubo_buffers.buffers.insert(
                    {
                        binding_set_id.GetId(),
                        ubo_bindings[0].buffers
                    }
                    );
            }

            auto descriptors = wvk::raii::pipelines::param_bindings::CreateParamsDescriptorSet(
                descriptor_pool.Creator().device,
                descriptor_set_layout,
                *descriptor_pool,
                texture_bindings,
                ubo_bindings
                );

            bindings.insert(
                {
                    binding_set_id.GetId(),
                    Binding<FramesInFlight>{
                        .pipeline_id=pipeline_id,
                        .descriptor_set=std::move(descriptors)
                    }
                }
                );

            if (!pipeline_binding_offset.contains(pipeline_id.GetId())) {
                pipeline_binding_offset[pipeline_id.GetId()] =
                    PipelineOffsets(ubo_bindings);
            }
        }

        template<wvk::raii::pipelines::param_bindings::EUpdateType UpdateType>
        void UpdateBindingSetParameter(
            std::uint8_t frame_index,
            wcr::wid::WEngId render_pipeline_id,
            wcr::wid::WEngId binding_set_id,
            wct::render::RPipeParamUbo const & ubo_param_data
            ) {
            std::uint32_t offset = pipeline_binding_offset
                [render_pipeline_id.GetId()]
                [ubo_param_data.binding];

            wvk::raii::pipelines::param_bindings
                ::template UpdateParameter_Postprocess<UpdateType, FramesInFlight>
                (
                    device,
                    frame_index,
                    ubo_param_data,
                    ubo_buffers.buffers[binding_set_id.GetId()],
                    offset
                    );
        }

    private:

        std::unordered_map<std::uint8_t, std::uint32_t>
        PipelineOffsets(
            std::vector<WVkDescSetUBOBinding<FramesInFlight>> & ubo_bindings
            ) {
            std::unordered_map<std::uint8_t, std::uint32_t> offset_map;
            for(auto & bind : ubo_bindings) {
                offset_map[bind.binding]=bind.offset;
            }
            return offset_map;
        }
    };

}
