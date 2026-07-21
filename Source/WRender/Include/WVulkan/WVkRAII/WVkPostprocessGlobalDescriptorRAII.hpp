#pragma once

#include "WCore/WCoreMacros.hpp"
#include "WVulkan/WVk/WVkDescriptor.hpp"
#include "WVulkan/WVk/WVkTypes.hpp"

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>

template<std::uint8_t FramesInFlight>
class WVkPostprocessGlobalDescriptorRAII {
public:

    static inline const std::uint8_t PREV_BINDING{0}; // Previous render
    static inline const std::uint8_t COLOR_BINDING{1};

    static inline const std::uint8_t ALBEDO_BINDING{2};
    static inline const std::uint8_t EMISSION_BINDING{3};    
    static inline const std::uint8_t NORMAL_BINDING{4};
    // static inline const std::uint8_t WS_POSITION_BINDING{5};
    static inline const std::uint8_t MRAO_BINDING{5};
    static inline const std::uint8_t DEPTH_BINDING{6};
    static inline const std::uint8_t EXTRA01_BINDING{7};

    static inline const std::uint8_t BINDING_COUNT{8};

public:    

    WVkPostprocessGlobalDescriptorRAII() = default;

    WVkPostprocessGlobalDescriptorRAII(
        const WVkPostprocessGlobalDescriptorRAII &
        ) = delete;
    WVkPostprocessGlobalDescriptorRAII& operator=(
        const WVkPostprocessGlobalDescriptorRAII &
        ) = delete;

    WVkPostprocessGlobalDescriptorRAII(WVkPostprocessGlobalDescriptorRAII && other) noexcept :
        device_(other.device_),
        descriptor_pool_(other.descriptor_pool_),
        descriptor_layout_(other.descriptor_layout_),
        descriptor_sets_(std::move(other.descriptor_sets_))
        {
        other.device_=VK_NULL_HANDLE;
        other.descriptor_pool_=VK_NULL_HANDLE;
        other.descriptor_layout_=VK_NULL_HANDLE;
    }

    WVkPostprocessGlobalDescriptorRAII& operator=(WVkPostprocessGlobalDescriptorRAII && other) noexcept {
        if(this != &other) {
            Destroy();

            device_=other.device_;
            descriptor_pool_ = other.descriptor_pool_;
            descriptor_layout_ = other.descriptor_layout_;
            descriptor_sets_ = std::move(other.descriptor_sets_);

            other.device_ = VK_NULL_HANDLE;
            other.descriptor_pool_ = VK_NULL_HANDLE;
            other.descriptor_layout_ = VK_NULL_HANDLE;
        }
        return *this;
    }


    virtual ~WVkPostprocessGlobalDescriptorRAII() {
        Destroy();
    }

    WVkPostprocessGlobalDescriptorRAII(VkDevice in_device) :
        device_(in_device)
        {
        Initialize(device_);
    }

public:

    void UpdateDescriptorSet(
        std::uint8_t in_frame_index,
        const std::array<VkDescriptorImageInfo, BINDING_COUNT> & in_descriptor_images
        ) {
        
        std::array<VkWriteDescriptorSet, BINDING_COUNT> write_ds;

        for(std::uint32_t i=0; i<write_ds.size(); i++) {
            wvk::descriptor::UpdateWriteDescriptorSet_Texture(
                write_ds[i],
                i,
                in_descriptor_images[i],
                descriptor_sets_[in_frame_index]
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

    void UpdateDescriptorBinding(
        std::uint8_t in_binding,
        std::uint8_t frame_index,
        VkDescriptorImageInfo in_descriptor_image_info
        ) {
        VkWriteDescriptorSet write_ds=wvk::types::VkWriteDescriptorSet();
        
        wvk::descriptor::UpdateWriteDescriptorSet_Texture(
            write_ds,
            in_binding,
            in_descriptor_image_info,
            descriptor_sets_[frame_index]
            );

        vkUpdateDescriptorSets(
            device_,
            1,
            &write_ds,
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
        if (device_ != VK_NULL_HANDLE) {
            descriptor_sets_ = {};

            wvk::descriptor::Destroy(
                descriptor_pool_,
                device_
                );

            wvk::descriptor::Destroy(
                descriptor_layout_,
                device_
                );

            device_=VK_NULL_HANDLE;
            descriptor_pool_=VK_NULL_HANDLE;
            descriptor_layout_=VK_NULL_HANDLE;
        }
    }

    VkDescriptorSetLayout CreateDescLayout(VkDevice in_device) {

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

        // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE

        std::array bindings {
            buildbinding(PREV_BINDING),
            buildbinding(COLOR_BINDING),
            buildbinding(ALBEDO_BINDING),
            buildbinding(EMISSION_BINDING),
            buildbinding(NORMAL_BINDING),
            // buildbinding(WS_POSITION_BINDING),
            buildbinding(MRAO_BINDING),
            buildbinding(DEPTH_BINDING),
            buildbinding(EXTRA01_BINDING)
        };

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
