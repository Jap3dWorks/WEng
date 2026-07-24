#pragma once

#include "WVulkan/RAII/VkRAII.hpp"

#include <cstdint>

#include <vulkan/vulkan_core.h>


namespace wvk::raii {

    template<std::uint8_t UniformBuffers, std::uint8_t Images, std::uint8_t MaxSets>
    struct DescriptorPoolCrtr {

    public:

        VkDescriptorPool Create();

        void Destroy(VkDescriptorPool desc_pool) {
            
        }
        
    private:

        VkDevice device_{ VK_NULL_HANDLE };

    };
}
