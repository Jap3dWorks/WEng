#pragma once

#include "WVulkan/RAII/VkRAII.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/Vk/WVkTypes.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace wvk::raii {

    template<
        std::uint32_t DynamicUniformCount,
        std::uint32_t StaticUniformCount,
        std::uint32_t ImageSamplerCount,
        std::uint32_t MaxSets>
    struct DescriptorPoolCrtr {

        static_assert(DynamicUniformCount + StaticUniformCount + ImageSamplerCount > 0);

    public:

        VkDescriptorPool Create() {

            VkDescriptorPoolCreateInfo pool_info =
                wvk::types::VkDescriptorPoolCreateInfo();

            auto update_pool_info = [&pool_info]
                (auto & data ) {
                pool_info.poolSizeCount = static_cast<std::uint32_t>(data.size());
                pool_info.pPoolSizes = data.data();
                pool_info.maxSets = MaxSets;
            };

            if constexpr(DynamicUniformCount > 0 && StaticUniformCount > 0 && ImageSamplerCount > 0) {
                std::array<VkDescriptorPoolSize, 3> pool_sizes{};
                pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                pool_sizes[0].descriptorCount=DynamicUniformCount;

                pool_sizes[1].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                pool_sizes[1].descriptorCount=StaticUniformCount;

                pool_sizes[2].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                pool_sizes[2].descriptorCount=ImageSamplerCount;

                update_pool_info(pool_sizes);

            }
            else if constexpr (DynamicUniformCount>0 && StaticUniformCount > 0) {
                std::array<VkDescriptorPoolSize, 2> pool_sizes{};
                pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                pool_sizes[0].descriptorCount=DynamicUniformCount;

                pool_sizes[1].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                pool_sizes[1].descriptorCount=StaticUniformCount;

                update_pool_info(pool_sizes);
            }
            else if constexpr (DynamicUniformCount>0 && ImageSamplerCount > 0) {
                std::array<VkDescriptorPoolSize, 2> pool_sizes{};
                pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                pool_sizes[0].descriptorCount=DynamicUniformCount;

                pool_sizes[1].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                pool_sizes[1].descriptorCount=ImageSamplerCount;

                update_pool_info(pool_sizes);
            }
            else if constexpr (DynamicUniformCount>0) {
                std::array<VkDescriptorPoolSize, 1> pool_sizes{};
                pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                pool_sizes[0].descriptorCount=StaticUniformCount;

                update_pool_info(pool_sizes);
            }
            else if constexpr (StaticUniformCount > 0) {
                std::array<VkDescriptorPoolSize, 1> pool_sizes{};
                pool_sizes[0].type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                pool_sizes[0].descriptorCount=StaticUniformCount;

                update_pool_info(pool_sizes);
            }
            else if constexpr (ImageSamplerCount > 0) {
                std::array<VkDescriptorPoolSize, 1> pool_sizes{};
                pool_sizes[0].type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                pool_sizes[0].descriptorCount=ImageSamplerCount;

                update_pool_info(pool_sizes);
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

    template<
        std::uint32_t DynamicUniformCount,
        std::uint32_t StaticUniformCount,
        std::uint32_t ImageSamplerCount,
        std::uint32_t MaxSets>
    using DescriptorPool = wvk::raii::VkRAII<
        VkDescriptorPool,
        DescriptorPoolCrtr<DynamicUniformCount, StaticUniformCount, ImageSamplerCount, MaxSets>>;
}
