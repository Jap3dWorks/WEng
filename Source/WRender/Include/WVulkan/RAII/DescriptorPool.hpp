#pragma once

#include "WVulkan/RAII/VkRAII.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace wvk::raii {

    template<std::uint8_t UniformBuffers, std::uint8_t Images, std::uint8_t MaxSets>
    struct DescriptorPoolCrtr {

    public:

        VkDescriptorPool Create() {
            std::array<VkDescriptorPoolSize, 2> pool_sizes{};

            pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            pool_sizes[0].descriptorCount=UniformBuffers;

            pool_sizes[1].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            pool_sizes[1].descriptorCount=Images;

            VkDescriptorPoolCreateInfo pool_info =
                wvk::types::VkDescriptorPoolCreateInfo();

            pool_info.poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size());
            pool_info.pPoolSizes = pool_sizes.data();
            pool_info.maxSets = MaxSets;

            VkDescriptorPool result;
            wvk::vulkan::ExecVkProcChecked(
                vkCreateDescriptorPool,
                "Failed to create descriptor pool!",
                device_,
                &pool_info,
                nullptr,
                &result
                );

            return result;
        }

        void Destroy(VkDescriptorPool desc_pool) {

            vkDestroyDescriptorPool(
                device_,
                desc_pool,
                nullptr
                );

            device_ = VK_NULL_HANDLE;

        }
        
    private:

        VkDevice device_{ VK_NULL_HANDLE };

    };

    template<std::uint8_t UniformBuffers, std::uint8_t Images, std::uint8_t MaxSets>
    using DescriptorPool = wvk::raii::VkRAII<
        VkDescriptorPool,
        DescriptorPoolCrtr<UniformBuffers, Images, MaxSets>>;
}
