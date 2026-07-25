#pragma once

#include "WVulkan/RAII/VkRAII.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace wvk::raii {

    template<std::uint8_t UniformBuffers, std::uint8_t Images, std::uint8_t MaxSets>
    struct DescriptorPoolCrtr {

        static_assert(UniformBuffers + Images > 0);

    public:

        VkDescriptorPool Create() {

            VkDescriptorPoolCreateInfo pool_info =
                wvk::types::VkDescriptorPoolCreateInfo();

            if constexpr(UniformBuffers > 0 && Images > 0) {
                std::array<VkDescriptorPoolSize, 2> pool_sizes{};
                pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                pool_sizes[0].descriptorCount=UniformBuffers;

                pool_sizes[1].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                pool_sizes[1].descriptorCount=Images;

                pool_info.poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size());
                pool_info.pPoolSizes = pool_sizes.data();
                pool_info.maxSets = MaxSets;
            }
            else if constexpr (UniformBuffers > 0) {
                std::array<VkDescriptorPoolSize, 1> pool_sizes{};
                pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                pool_sizes[0].descriptorCount=UniformBuffers;

                pool_info.poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size());
                pool_info.pPoolSizes = pool_sizes.data();
                pool_info.maxSets = MaxSets;
            }
            else if constexpr (Images > 0) {
                std::array<VkDescriptorPoolSize, 1> pool_sizes{};
                pool_sizes[0].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                pool_sizes[0].descriptorCount=Images;

                pool_info.poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size());
                pool_info.pPoolSizes = pool_sizes.data();
                pool_info.maxSets = MaxSets;
            }
            else {
                return VK_NULL_HANDLE;
            }

            VkDescriptorPool result;
            wvk::vulkan::ExecVkProcChecked(
                vkCreateDescriptorPool,
                "Failed to create descriptor pool!",
                device,
                &pool_info,
                nullptr,
                &result
                );

            return result;
        }

        void Destroy(VkDescriptorPool desc_pool) {

            vkDestroyDescriptorPool(
                device,
                desc_pool,
                nullptr
                );
        }
        
    public:

        VkDevice device{ VK_NULL_HANDLE };

    };

    template<std::uint8_t UniformBuffers, std::uint8_t Images, std::uint8_t MaxSets>
    using DescriptorPool = wvk::raii::VkRAII<
        VkDescriptorPool,
        DescriptorPoolCrtr<UniformBuffers, Images, MaxSets>>;
}
