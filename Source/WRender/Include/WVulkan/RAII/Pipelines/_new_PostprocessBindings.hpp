#pragma once

#include "WCore/WId.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/AssetRenderData.hpp"

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>
#include <vector>

namespace wvk::raii::pipelines::postprocess {

    template<std::uint8_t FramesInFlight>
    struct Binding {
        // wcr::wid::WEngId binding_set_id;

        wcr::wid::WEngId pipeline_id;
        std::array<VkDescriptorSet, FramesInFlight> descriptor_set;

    };

    template<std::uint8_t FramesInFlight>
    struct BindingCollection {


        std::vector<wcr::wid::WEngId> binding_order;
        std::unordered_map<std::size_t, Binding<FramesInFlight>> bindings;


        void CreateBindingSet(
            wcr::wid::WEngId binding_set_id,  // camera component id
            wcr::wid::WEngId pipeline_id,
            wct::render::RPipeParamDescriptorsLayout const & param_descriptors,
            wct::render::RPipeParamList_Ubo const & ubo_params,
            wct::render::RPipeParamList_WAssetId const & texture_params,
            VkDescriptorSetLayout descriptor_set_layout,
            wvk::raii::AssetRenderData & asset_data
            ) {

            

        }

        
    };
}
