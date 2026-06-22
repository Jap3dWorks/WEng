#pragma once

#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkDescriptor.hpp"
#include "WVulkan/WVk/WVkBuffer.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>

template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkGlobalDescriptorsRAII {

public:

    WVkGlobalDescriptorsRAII() = default;
    WVkGlobalDescriptorsRAII(const WVkGlobalDescriptorsRAII&) = delete;
    
    WVkGlobalDescriptorsRAII(WVkGlobalDescriptorsRAII&& other) noexcept
        : device_(std::move(other.device_)),
          physical_device_(std::move(other.physical_device_)),
          descpool_info_(std::move(other.descpool_info_)),
          descset_layout_info_(std::move(other.descset_layout_info_)),
          descset_info_(std::move(other.descset_info_)),
          camera_ubo_(std::move(other.camera_ubo_)),
          lighting_ubo_(std::move(other.camera_ubo_))
        {
            other.device_ = VK_NULL_HANDLE;
            other.physical_device_ = VK_NULL_HANDLE;
            other.descpool_info_ = VK_NULL_HANDLE;
            other.descset_layout_info_ = VK_NULL_HANDLE;
        }
          

    WVkGlobalDescriptorsRAII& operator=(const WVkGlobalDescriptorsRAII&) = delete;

    WVkGlobalDescriptorsRAII& operator=(WVkGlobalDescriptorsRAII&& other) noexcept {
        if (this != &other) {
            device_ = std::move(other.device_);
            physical_device_ = std::move(other.physical_device_);
            descpool_info_ = std::move(other.descpool_info_);
            descset_layout_info_ = std::move(other.descset_layout_info_);
            descset_info_ = std::move(other.descset_info_);
            camera_ubo_ = std::move(other.camera_ubo_);
            lighting_ubo_ = std::move(other.lighting_ubo_);

            other.device_ = VK_NULL_HANDLE;
            other.physical_device_ = VK_NULL_HANDLE;
            other.descpool_info_ = VK_NULL_HANDLE;
            other.descset_layout_info_ = VK_NULL_HANDLE;
        }
        return *this;
    }

    virtual ~WVkGlobalDescriptorsRAII() {
        Destroy();
    }

    WVkGlobalDescriptorsRAII(VkDevice in_device,
                             VkPhysicalDevice in_physical_device)
        : device_(in_device),
          physical_device_(in_physical_device) {
        Initialize();
    }

    VkDescriptorSetLayout DescriptorSetLayout() const {
        return descset_layout_info_;
    }

    VkDescriptorSet DescriptorSet(std::uint32_t in_frame_index) const {
        return descset_info_[in_frame_index];
    }

    void UpdateCameraUBO(
        const wct::render::WCameraUBO & in_camera_ubo,
        std::uint32_t in_frame_index
        ) {
        void * ptr = wvk::buffer::MapUBO(
            camera_ubo_[in_frame_index],
            device_
            );
    
        memcpy(
            ptr,
            &in_camera_ubo,
            sizeof(wct::render::WCameraUBO)
            );
    
        wvk::buffer::UnmapUBO(
            camera_ubo_[in_frame_index],
            device_
            );
    }

    void UpdateLightingUBO(
        const wct::render::WLightingUBO & in_lighting_ubo,
        std::uint32_t in_frame_index
        ) {

        memcpy(
            wvk::buffer::MapUBO(
                lighting_ubo_[in_frame_index],
                device_
            ),
            &in_lighting_ubo,
            sizeof(wct::render::WLightingUBO)
            );

        wvk::buffer::UnmapUBO(
            lighting_ubo_[in_frame_index],
            device_
            );
    }

    void StaticUpdateLightingUBO(
        const wct::render::WLightingUBO & in_lighting_ubo
        ) {
        for(std::uint8_t i=0; i<FramesInFlight; i++) {
            UpdateLightingUBO(
                in_lighting_ubo,
                i
                );
        }
    }

    // TODO UpdateDescriptorSet using pointers and offsets.

public:

    static constexpr std::uint8_t CAMERA_BINDING{0};
    static constexpr std::uint8_t LIGHTING_BINDING{1};

private:

    void Initialize() {

        std::array<VkDescriptorSetLayoutBinding,2> bindings{};

        bindings[0].binding = CAMERA_BINDING;
        bindings[0].descriptorCount = 1;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].pImmutableSamplers = nullptr;
        bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
                                 VK_SHADER_STAGE_FRAGMENT_BIT;

        bindings[1].binding = LIGHTING_BINDING;
        bindings[1].descriptorCount = 1;
        bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[1].pImmutableSamplers = nullptr;
        bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
                                 VK_SHADER_STAGE_FRAGMENT_BIT; 

        descset_layout_info_ = wvk::descriptor::Create(
            bindings.data(),
            bindings.size(),
            device_
            );

        wvk::descriptor::Create(
            descpool_info_,
            device_
            );

        for (std::uint32_t i=0; i < FramesInFlight; i++) {
            descset_info_[i] = wvk::descriptor::Create(
                device_,
                descset_layout_info_,
                descpool_info_
                );

            camera_ubo_[i] = wvk::buffer::CreateUBO(
                sizeof(wct::render::WCameraUBO),
                device_,
                physical_device_
                );

            lighting_ubo_[i] = wvk::buffer::CreateUBO(
                sizeof(wct::render::WLightingUBO),
                device_,
                physical_device_
                );

            std::array<VkWriteDescriptorSet, 2> write_descriptors{};
            std::array<VkDescriptorBufferInfo, 2> buffer_infos{};
            std::uint8_t j=0;
            for(auto * itm : {&camera_ubo_[i], &lighting_ubo_[i]}) {
                buffer_infos[j].buffer = itm->buffer;
                buffer_infos[j].range = itm->range;
                buffer_infos[j].offset=0;

                wvk::descriptor::UpdateWriteDescriptorSet_UBO(
                    write_descriptors[j],
                    j, // binding 0 and 1
                    buffer_infos[j],
                    descset_info_[i]
                    );

                ++j;
            }

            vkUpdateDescriptorSets(
                device_,
                write_descriptors.size(),
                write_descriptors.data(),
                0,
                nullptr
                );
        }
    }

    void Destroy() {
        if (descpool_info_ != VK_NULL_HANDLE) {

            wvk::descriptor::Destroy(
                descpool_info_,
                device_);

            for(uint32_t i=0; i<camera_ubo_.size(); i++) {
                wvk::buffer::Destroy(camera_ubo_[i],
                                     device_);
            }
            for (std::uint32_t i=0; i<lighting_ubo_.size(); i++) {
                wvk::buffer::Destroy(lighting_ubo_[i], device_);
            }

            if (descset_layout_info_)
            {
                wvk::descriptor::Destroy(
                    descset_layout_info_,
                    device_
                    );
            }

            device_ = VK_NULL_HANDLE;
            physical_device_ = VK_NULL_HANDLE;
            descpool_info_ = VK_NULL_HANDLE;
            descset_layout_info_ = VK_NULL_HANDLE;
            descset_info_ = {};
            camera_ubo_ = {};
            lighting_ubo_ = {};
        }
    }

    VkDevice device_{VK_NULL_HANDLE};
    VkPhysicalDevice physical_device_{VK_NULL_HANDLE};

    VkDescriptorPool descpool_info_{VK_NULL_HANDLE};
    VkDescriptorSetLayout descset_layout_info_{VK_NULL_HANDLE};
    std::array<VkDescriptorSet, FramesInFlight> descset_info_{};

    std::array<WVkUBOInfo, FramesInFlight> camera_ubo_{};
    std::array<WVkUBOInfo, FramesInFlight> lighting_ubo_{};

};
