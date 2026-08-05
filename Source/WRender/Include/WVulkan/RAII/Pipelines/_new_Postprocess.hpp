#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "BindingCollection.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace wck::raii::pipelines {

    template<std::uint8_t FramesInFlight>
    class _new_Postprocess {

    public:

        _new_Postprocess() = default;

        _new_Postprocess(const _new_Postprocess&) = delete;
        _new_Postprocess& operator=(const _new_Postprocess&) = delete;

        _new_Postprocess(_new_Postprocess&&) = default;
        _new_Postprocess& operator=(_new_Postprocess&&) = default;

        ~_new_Postprocess() = default;

        _new_Postprocess(
            VkDevice in_device,
            VkPhysicalDevice in_physical_device
            ) : vkn_(in_device, in_physical_device)
            {}

        void CreatePipeline();

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

        wvk::raii::pipelines::BindingCollection<FramesInFlight> collection_{};

    };
}
