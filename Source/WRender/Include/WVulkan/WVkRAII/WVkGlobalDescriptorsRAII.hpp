#pragma once

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
          camera_ubo_(std::move(other.camera_ubo_))
        {
            other.device_ = VK_NULL_HANDLE;
            other.physical_device_ = VK_NULL_HANDLE;
            other.descpool_info_ = VK_NULL_HANDLE;
            other.descset_layout_info_ = {};
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

            other.device_ = VK_NULL_HANDLE;
            other.physical_device_ = VK_NULL_HANDLE;
            other.descpool_info_ = VK_NULL_HANDLE;
            other.descset_layout_info_ = {};
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
        return descset_layout_info_.descset_layout;
    }

    VkDescriptorSet DescriptorSet(std::uint32_t in_frame_index) const {
        return descset_info_[in_frame_index];
    }

    void UpdateDescriptorSet(
        const wct::render::WCameraUBO & in_camera_struct,
        std::uint32_t in_frame_index
        ) {
        void * ptr = wvk::buffer::MapUBO(
            camera_ubo_[in_frame_index],
            device_
            );
    
        memcpy(
            ptr,
            &in_camera_struct,
            sizeof(wct::render::WCameraUBO)
            );
    
        wvk::buffer::UnmapUBO(
            camera_ubo_[in_frame_index],
            device_
            );
    }


private:

    void Initialize() {
        VkDescriptorSetLayoutBinding camera_binding{};
        camera_binding.binding=0;
        camera_binding.descriptorCount = 1;
        camera_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        camera_binding.pImmutableSamplers = nullptr;
        camera_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; //TODO: vertex and fragment

        // TODO lights here too

        descset_layout_info_.bindings = {
            camera_binding
        };

        wvk::descriptor::Create(
            descset_layout_info_,
            device_
            );

        // Global descriptor pool (camera and lights)
        wvk::descriptor::Create(
            descpool_info_,
            device_
            );

        for (std::uint32_t i=0; i < FramesInFlight; i++) {
            wvk::descriptor::Create(
                descset_info_[i],
                device_,
                descset_layout_info_,
                descpool_info_
                );

        }
        for(uint32_t i=0; i < FramesInFlight; i++) {

            camera_ubo_[i] = wvk::buffer::CreateUBO(
                sizeof(wct::render::WCameraUBO),
                device_,
                physical_device_
                );

            VkWriteDescriptorSet ws = {};
            VkDescriptorBufferInfo buffer_info{};

            buffer_info.buffer = camera_ubo_[i].buffer;
            buffer_info.offset = 0;
            buffer_info.range = camera_ubo_[i].range;

            wvk::descriptor::UpdateWriteDescriptorSet_UBO(
                ws,
                0,
                buffer_info,
                descset_info_[i]
                );

            vkUpdateDescriptorSets(
                device_,
                1,
                &ws,
                0,
                nullptr
                );
        }

    }

    void Destroy() {
        if (descpool_info_ != VK_NULL_HANDLE) {

            wvk::descriptor::Destroy(descpool_info_,
                                     device_);

            for(uint32_t i=0; i<camera_ubo_.size(); i++) {
                wvk::buffer::Destroy(camera_ubo_[i],
                                     device_);
            }

            if (descset_layout_info_.descset_layout)
            {
                wvk::descriptor::Destroy(
                    descset_layout_info_,
                    device_
                    );
            }

            device_ = VK_NULL_HANDLE;
            physical_device_ = VK_NULL_HANDLE;
            descpool_info_ = VK_NULL_HANDLE;
            descset_layout_info_ = {};
            descset_info_ = {};
            camera_ubo_ = {};
        }
    }

    VkDevice device_{VK_NULL_HANDLE};
    VkPhysicalDevice physical_device_{VK_NULL_HANDLE};

    VkDescriptorPool descpool_info_{VK_NULL_HANDLE};
    WVkDescriptorSetLayoutInfo descset_layout_info_{};
    std::array<VkDescriptorSet, FramesInFlight> descset_info_{};
    std::array<WVkUBOInfo, FramesInFlight> camera_ubo_{};

};
