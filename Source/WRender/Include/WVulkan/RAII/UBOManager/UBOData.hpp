#pragma once

#include "WVulkan/Vk/WVkBuffer.hpp"
#include "WCore/TSparseSet.hpp"
#include "WVulkan/WVulkanStructs.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>
#include <variant>

namespace wvk::raii::ubo_manager {
    
    template<std::uint8_t FramesInFlight>
    class UboData {

    public:

        UboData() = delete;
        
        UboData(const UboData&) = default;
        UboData(UboData&&) = default;
        UboData& operator=(const UboData&) = default;
        UboData& operator=(UboData&&) = default;

        ~UboData() {
            if(device_ != VK_NULL_HANDLE) {
                for(auto & vk_buffer: vk_buffers) {
                    wvk::buffer::Destroy(
                        vk_buffer,
                        device_
                        );
                    vk_buffer = {};
                }
                    
                device_ = VK_NULL_HANDLE;
            }
        }

        UboData(
            VkDevice device,
            VkPhysicalDevice physical_device,
            std::size_t block_size,
            std::size_t buffer_count_size) :
            device_(device),
            block_size_(block_size) {

            // Assert block size alignment
            assert(block_size % 16 == 0);

            VkDeviceSize device_size = 
                block_size * buffer_count_size;

            for(auto & vk_buffer : vk_buffers) {
                vk_buffer = wvk::buffer::CreateUBO(
                    device_size,
                    device,
                    physical_device
                    );
            }
        }

        void Remove(std::vector<wcr::wid::WEngId>);

        void Add(std::vector<wcr::wid::WEngId> ids, const void * data) {
            for(auto id : ids) {
                position_track.Insert(id.GetId(), id);
            }

            std::size_t start_pos = position_track.DensePosition(ids[0].GetId());

            for(std::uint8_t f=0; f<FramesInFlight; f++) {
                void * ptr = wvk::buffer::MapUBO(vk_buffers[f].buffer, device_);

                wvk::buffer::UpdateUBO(
                    ptr,
                    data,
                    ids.size() * block_size_,
                    start_pos * block_size_
                    );

                wvk::buffer::UnmapUBO(vk_buffers[f].buffer);
            }
        }

        bool Contains(wcr::wid::WEngId);

        void Update(std::uint8_t frame_index, wcr::wid::WEngId, const void* data);

        std::size_t GetPosition(std::uint8_t frame_index, wcr::wid::WEngId);

        std::size_t GetOffset(std::uint8_t frame_index, wcr::wid::WEngId);

    private:

        TSparseSet<wcr::wid::WEngId> position_track;
        std::array<WVkUBO, FramesInFlight> vk_buffers;

        std::size_t block_size_;
        VkDevice device_;
    };
        

}
