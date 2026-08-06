#pragma once

#include "WCore/WId.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/Pipelines/DescriptorBindings.hpp"
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
        
        std::unordered_map<std::size_t, Binding<FramesInFlight>> bindings{};

        void CreateBindingSet(
            wcr::wid::WEngId binding_set_id,  // camera component id
            wcr::wid::WEngId pipeline_id,
            wct::render::RPipeParamDescriptorsLayout const & param_descriptors,
            wct::render::RPipeParamList_Ubo const & ubo_params,
            wct::render::RPipeParamList_WAssetId const & texture_params,
            VkDescriptorSetLayout descriptor_set_layout,
            wvk::raii::AssetRenderData & asset_data
            ) {

            auto texture_bindings = wvk::raii::pipelines::desc_bindings
                ::CollectTextureBindings(
                    texture_params,
                    asset_data
                    );

            auto ubo_bindings = wvk::raii::pipelines::desc_bindings
                ::CollectUBOBindings<FramesInFlight>(
                    descriptor_pool.Creator().device,
                    physical_device,
                    param_descriptors,
                    ubo_params                    
                    );

            if (!ubo_bindings.empty()) {
                ubo_buffers.buffers.insert(
                    binding_set_id.GetId(),
                    ubo_bindings[0].buffers
                    );
            }

            auto descriptors = wvk::raii::pipelines::desc_bindings::CreateParamsDescriptorSet(
                descriptor_pool.Creator().device,
                descriptor_set_layout,
                *descriptor_pool,
                texture_bindings,
                ubo_bindings
                );

            bindings.insert(
                binding_set_id.GetId(),
                Binding{
                    .pipeline_id=pipeline_id,
                    .descriptor_set=std::move(descriptors)
                }
                );
        }

    private:

        
    };
}
