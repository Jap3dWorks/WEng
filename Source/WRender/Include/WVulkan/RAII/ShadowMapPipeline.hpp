#pragma once

#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/RAII/DescriptorPool.hpp"

#include <string_view>
#include <vulkan/vulkan_core.h>

namespace wvk::raii {
    
    template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
    class ShadowMapPipeline {

    public:

        static inline constexpr std::string_view SHADER_PATH{"/Content/Shaders/..."};

    public:

        ShadowMapPipeline() = default;
        ShadowMapPipeline(const ShadowMapPipeline&) = delete;
        ShadowMapPipeline(ShadowMapPipeline&&) = default;
        ShadowMapPipeline& operator=(const ShadowMapPipeline&) = delete;
        ShadowMapPipeline& operator=(ShadowMapPipeline&&) = default;
        ~ShadowMapPipeline() = default;

        ShadowMapPipeline(
            VkDevice device,
            std::string_view shader_path
            ) : device_(device) {
            
        }

    private:

        void Initialize(VkDevice device) {
            
        }

    private:

        VkDevice device_{VK_NULL_HANDLE};

        wvk::raii::DescriptorPool<2, 0, 2> descriptor_pool_{};
        VkDescriptorSetLayout descset_layout_{VK_NULL_HANDLE};
        VkPipeline pipeline_{VK_NULL_HANDLE};
        VkPipeline pipeline_layout_{VK_NULL_HANDLE};

    };
}
