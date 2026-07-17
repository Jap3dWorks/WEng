#pragma once

#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVkDescriptor.hpp"
#include "WVulkan/WVk/WVkBuffer.hpp"
#include "WVulkan/WVk/WVulkan.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>

template<std::uint8_t FramesInFlight=WENG_MAX_FRAMES_IN_FLIGHT>
class WVkGlobalDescriptorsRAII {

public:

    WVkGlobalDescriptorsRAII() = default;
    WVkGlobalDescriptorsRAII(const WVkGlobalDescriptorsRAII&) = delete;
    WVkGlobalDescriptorsRAII& operator=(const WVkGlobalDescriptorsRAII&) = delete;

    WVkGlobalDescriptorsRAII(WVkGlobalDescriptorsRAII&& other) noexcept
        : device_(std::move(other.device_)),
          descriptor_pool_(std::move(other.descriptor_pool_)),
          descset_layout_info_(std::move(other.descset_layout_info_)),
          descriptors_(std::move(other.descriptors_)),
          camera_ubo_(std::move(other.camera_ubo_)),
          lighting_ubo_(std::move(other.camera_ubo_))
        {
            other.device_ = VK_NULL_HANDLE;
            other.descriptor_pool_ = VK_NULL_HANDLE;
            other.descset_layout_info_ = VK_NULL_HANDLE;
        }
          

    WVkGlobalDescriptorsRAII & operator=(WVkGlobalDescriptorsRAII && other) noexcept {
        if (this != &other) {
            Destroy();

            device_ = std::move(other.device_);
            descriptor_pool_ = std::move(other.descriptor_pool_);
            descset_layout_info_ = std::move(other.descset_layout_info_);
            descriptors_ = std::move(other.descriptors_);
            camera_ubo_ = std::move(other.camera_ubo_);
            lighting_ubo_ = std::move(other.lighting_ubo_);

            other.device_ = VK_NULL_HANDLE;
            other.descriptor_pool_ = VK_NULL_HANDLE;
            other.descset_layout_info_ = VK_NULL_HANDLE;
        }
        return *this;
    }

    ~WVkGlobalDescriptorsRAII() {
        Destroy();
    }

    WVkGlobalDescriptorsRAII(VkDevice in_device,
                             VkPhysicalDevice in_physical_device)
        : device_(in_device) {
        Initialize(in_physical_device);
    }

    VkDescriptorSetLayout DescriptorSetLayout() const {
        return descset_layout_info_;
    }

    VkDescriptorSet DescriptorSet(std::uint32_t in_frame_index) const {
        return descriptors_[in_frame_index];
    }

    void UpdateCameraUBO(
        std::uint8_t in_frame_index,
        const wct::render::CameraUBO & in_camera_ubo
        ) {
        void * ptr = wvk::buffer::MapUBO(
            camera_ubo_[in_frame_index],
            device_
            );
    
        std::memcpy(
            ptr,
            &in_camera_ubo,
            sizeof(wct::render::CameraUBO)
            );
    
        wvk::buffer::UnmapUBO(
            camera_ubo_[in_frame_index],
            device_
            );
    }

    void UpdateCameraUBO(
        std::uint8_t in_frame_index,
        void * in_data,
        std::size_t in_size
        ) {
        memcpy(
            wvk::buffer::MapUBO(
                camera_ubo_[in_frame_index],
                device_
                ),
            in_data,
            in_size
            );
    
        wvk::buffer::UnmapUBO(
            camera_ubo_[in_frame_index],
            device_
            );
    }

    void UpdateLightingUBO(
        std::uint8_t in_frame_index,
        const wct::render::LightingUBO & in_lighting_ubo
        ) {
        memcpy(
            wvk::buffer::MapUBO(
                lighting_ubo_[in_frame_index],
                device_
                ),
            &in_lighting_ubo,
            sizeof(wct::render::LightingUBO)
            );

        wvk::buffer::UnmapUBO(
            lighting_ubo_[in_frame_index],
            device_
            );
    }

    void UpdateLightingUBO(
        std::uint8_t in_frame_index,
        void * data,
        std::size_t size
        )
        {
        memcpy(
            wvk::buffer::MapUBO(
                lighting_ubo_[in_frame_index],
                device_
                ),
            data,
            size
            );

        wvk::buffer::UnmapUBO(
            lighting_ubo_[in_frame_index],
            device_
            );
    }

    void StaticUpdateLightingUBO(
        const wct::render::LightingUBO & in_lighting_ubo
        ) {
        for(std::uint8_t i=0; i<FramesInFlight; i++) {
            UpdateLightingUBO(
                i,
                in_lighting_ubo
                );
        }
    }

    void StaticUpdateLightingUBO(
        void * data,
        std::size_t size,
        std::size_t offset
        )
        {
            for(std::uint8_t i=0; i<FramesInFlight; i++) {
                UpdateLightingUBO(
                    i,
                    data,
                    size
                    );
            }
        }

    // TODO UpdateDescriptorSet using pointers and offsets.

public:

    static constexpr std::uint8_t CAMERA_BINDING{0};
    static constexpr std::uint8_t LIGHTING_BINDING{1};

private:

    void Initialize(VkPhysicalDevice in_physical_device) {

        descset_layout_info_ = CreateDescrSetLayout(device_);

        descriptor_pool_ = CreateDescriptorPool(device_);

        for (std::uint32_t i=0; i < FramesInFlight; i++) {
            descriptors_[i] = wvk::descriptor::CreateDescriptor(
                device_,
                descset_layout_info_,
                descriptor_pool_
                );

            camera_ubo_[i] = wvk::buffer::CreateUBO(
                sizeof(wct::render::CameraUBO),
                device_,
                in_physical_device
                );

            lighting_ubo_[i] = wvk::buffer::CreateUBO(
                sizeof(wct::render::LightingUBO),
                device_,
                in_physical_device
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
                    &buffer_infos[j],
                    descriptors_[i]
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

    WNODISCARD VkDescriptorPool CreateDescriptorPool(
        VkDevice in_device
        )
        {
            VkDescriptorPool result;
            std::array<VkDescriptorPoolSize,1> pool_sizes;

            pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            pool_sizes[0].descriptorCount =
                static_cast<uint32_t>(FramesInFlight) * 4;

            return wvk::descriptor::CreateDescriptorPool<1>(
                in_device,
                pool_sizes,
                pool_sizes[0].descriptorCount
                );
        }
    

    WNODISCARD VkDescriptorSetLayout CreateDescrSetLayout(VkDevice in_device) {
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

        return wvk::descriptor::Create(
            bindings.data(),
            bindings.size(),
            in_device
            );
    }

    void Destroy() {
        if (descriptor_pool_ != VK_NULL_HANDLE) {

            wvk::descriptor::Destroy(
                descriptor_pool_,
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
            descriptor_pool_ = VK_NULL_HANDLE;
            descset_layout_info_ = VK_NULL_HANDLE;
            descriptors_ = {};
            camera_ubo_ = {};
            lighting_ubo_ = {};
        }
    }

    VkDevice device_{VK_NULL_HANDLE};

    VkDescriptorPool descriptor_pool_{VK_NULL_HANDLE};
    VkDescriptorSetLayout descset_layout_info_{VK_NULL_HANDLE};
    std::array<WVkUBOInfo, FramesInFlight> camera_ubo_{};
    std::array<WVkUBOInfo, FramesInFlight> lighting_ubo_{};

    std::array<VkDescriptorSet, FramesInFlight> descriptors_{};
};
