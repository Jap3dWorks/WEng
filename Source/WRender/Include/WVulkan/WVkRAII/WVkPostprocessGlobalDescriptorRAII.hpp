#pragma once

#include "WCore/WCoreMacros.hpp"
#include "WVulkan/WVk/WVkDescriptor.hpp"

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>

template<std::uint8_t FramesInFlight>
class WVkPostprocessGlobalDescriptorRAII {
public:

    static inline const std::uint8_t COLOR_BINDING{0};
    static inline const std::uint8_t ALBEDO_BINDING{1};
    static inline const std::uint8_t NORMAL_BINDING{2};
    static inline const std::uint8_t WS_POSITION_BINDING{3};
    static inline const std::uint8_t DEPTH_BINDING{4};

    static inline const std::uint8_t BINDING_COUNT{5};

public:    

    WVkPostprocessGlobalDescriptorRAII() = default;

    WVkPostprocessGlobalDescriptorRAII(
        const WVkPostprocessGlobalDescriptorRAII &
        ) = delete;
    WVkPostprocessGlobalDescriptorRAII& operator=(
        const WVkPostprocessGlobalDescriptorRAII &
        ) = delete;

    WVkPostprocessGlobalDescriptorRAII(WVkPostprocessGlobalDescriptorRAII &&) = default;

    WVkPostprocessGlobalDescriptorRAII& operator=(WVkPostprocessGlobalDescriptorRAII &&) = default;
    virtual ~WVkPostprocessGlobalDescriptorRAII() = default;

    void UpdateBindings(std::uint8_t frame_index,
                        std::array<VkDescriptorImageInfo, BINDING_COUNT> in_descriptor_images) {

        std::array<VkWriteDescriptorSet,BINDING_COUNT> write_ds;

        for(std::uint32_t i=0; i<write_ds.size(); i++) {
            wvk::descriptor::UpdateWriteDescriptorSet_Texture(
                write_ds[i],
                i,
                in_descriptor_images[i],
                descriptor_sets_[frame_index]
                );
        }

        vkUpdateDescriptorSets(
            device_,
            static_cast<std::uint32_t>(write_ds.size()),
            write_ds.data(),
            0,
            nullptr
            );
    }

    WNODISCARD VkDescriptorSetLayout DescriptorSetLayout() const {
        return descriptor_layout_;
    }

    WNODISCARD VkDescriptorSet DescriptorSet(std::uint8_t in_frame_index) const {
        return descriptor_sets_[in_frame_index];
    }

private:

    void Initialize(VkDevice in_device) {
        descriptor_layout_ = CreateDescLayout(in_device);
        descriptor_pool_ = CreateDescPool(in_device);
        
        for(std::uint32_t i=0; i<FramesInFlight; i++) {
            descriptor_sets_[i] = wvk::descriptor::CreateDescriptor(
                device_,
                descriptor_layout_,
                descriptor_pool_
                );
        }
        

    }

    void Destroy() {
        // TODO
    }

    VkDescriptorSetLayout CreateDescLayout(VkDevice in_device) {
        std::array<VkDescriptorSetLayoutBinding, 5> bindings{};

        auto buildbinding = [] (std::uint8_t _binding) -> auto {
            return VkDescriptorSetLayoutBinding{
                .binding=_binding,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount=1,
                .stageFlags=VK_SHADER_STAGE_VERTEX_BIT |
                            VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers=nullptr
            };
        };

        bindings[0] = buildbinding(COLOR_BINDING);
        bindings[1] = buildbinding(ALBEDO_BINDING);
        bindings[2] = buildbinding(NORMAL_BINDING);
        bindings[3] = buildbinding(NORMAL_BINDING);
        bindings[4] = buildbinding(WS_POSITION_BINDING);
        bindings[5] = buildbinding(DEPTH_BINDING);

        return wvk::descriptor::Create(
            bindings.data(),
            bindings.size(),
            in_device);
    }

    VkDescriptorPool CreateDescPool(VkDevice in_device) {

        std::array<VkDescriptorPoolSize, 1> pool_sizes;

        pool_sizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[0].descriptorCount = BINDING_COUNT * FramesInFlight;

        VkDescriptorPoolCreateInfo pool_info =
            wvk::types::VkDescriptorPoolCreateInfo();

        return wvk::descriptor::CreateDescriptorPool<1>(
            in_device,
            pool_sizes,
            pool_sizes[0].descriptorCount
            );
    }

private:

    VkDevice device_{VK_NULL_HANDLE};

    VkDescriptorSetLayout descriptor_layout_{VK_NULL_HANDLE};

    VkDescriptorPool descriptor_pool_{VK_NULL_HANDLE};

    std::array<VkDescriptorSet, FramesInFlight> descriptor_sets_{};

};
