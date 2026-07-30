#pragma once

#include "WVulkan/Vk/WVkBuffer.hpp"
#include "WCore/TSparseSet.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WCore/WDebug.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <vulkan/vulkan_core.h>
#include <variant>
#include <cassert>
#include <algorithm>

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
            std::size_t elements_max_count) :
            device_(device),
            block_size_(block_size),
            elements_max_count_(elements_max_count) {

            // Assert block size alignment as 4 floats
            assert(block_size % (sizeof(float) * 4) == 0);

            VkDeviceSize device_size = 
                block_size * elements_max_count;

            for(auto & vk_buffer : vk_buffers) {
                vk_buffer = wvk::buffer::CreateUBO(
                    device_size,
                    device,
                    physical_device
                    );
            }
        }

        void Remove(std::vector<wcr::wid::WEngId> ids) {
            std::sort(ids.begin(), ids.end(),
                      [this](auto& a, auto& b) {
                          return position_track.DensePosition(a.GetId()) <
                              position_track.DensePosition(b.GetId());
                      });

            auto last = std::unique(ids.begin(), ids.end());
            ids.erase(last, ids.end());

            struct Transaction {
                std::size_t old_pos;
                std::size_t new_pos;
                std::size_t size;
            };

            std::vector<Transaction> transactions;
            transactions.reserve(ids.size());

            auto cpy_track = position_track;

            for(auto it = ids.rbegin(); it != ids.rend(); ++it) {
                assert(position_track.Contains(it->GetId()));

                std::size_t new_pos = position_track.DensePosition(it->GetId());
                position_track.Remove(it->GetId());

                if (new_pos >= position_track.Count()) {
                    // this was the last position item.
                    continue;
                }

                // This can be simplified writting a dedicated sparse set than ensures
                // movements into deletion operations.

                std::size_t moved_id = position_track.IndexInDensePosition(new_pos);
                std::size_t old_pos = cpy_track.DensePosition(moved_id);

                if(transactions.empty() ||
                   transactions.back().new_pos != new_pos + 1 ||
                   transactions.back().old_pos != old_pos + 1)
                {
                    // empty or not consecutive
                    transactions.emplace_back({old_pos, new_pos, 1});
                }
                else {
                    // consecutive just update the last transaction
                    transactions.back().old_pos = old_pos;
                    transactions.back().new_pos = new_pos;
                    transactions.back().size += 1; 
                }
            }

            for(std::uint8_t f=0; f < FramesInFlight ; f++) {
                std::uint8_t * ptr = wvk::buffer::MapUBO(vk_buffers[f].buffer, device_);

                for(auto & trn : transactions) {
                    std::uint8_t * val_ptr = ptr + trn.old_pos * block_size_;
                    wvk::buffer::UpdateUBO(
                        ptr, val_ptr, block_size_ * trn.size, trn.new_pos * block_size_
                        );
                }
                wvk::buffer::UnmapUBO(vk_buffers[f].buffer, device_);
            }
        }

        void Add(std::vector<wcr::wid::WEngId> ids, const void * data) {
            for(auto id : ids) {
                assert(!position_track.Contains(id.GetId()));
                position_track.Insert(id.GetId(), 1);
            }

            std::size_t offset = GetOffset(ids[0]);
            std::size_t size = BlocksSize(ids.size());

            for(std::uint8_t f=0; f<FramesInFlight; f++) {
                void * ptr = wvk::buffer::MapUBO(vk_buffers[f].buffer, device_);

                wvk::buffer::UpdateUBO(
                    ptr,
                    data,
                    size,
                    offset
                    );

                wvk::buffer::UnmapUBO(vk_buffers[f].buffer);
            }
        }

        bool Contains(wcr::wid::WEngId id) const {
            return position_track.Contains(id.GetId());
        }

        void Update(std::uint8_t frame_index, wcr::wid::WEngId id, const void* data) {
            assert(position_track.Contains(id.GetId()));

            std::size_t dense_pos = position_track.IndexInDensePosition(id.GetId());

            // TODO
            
        }

        std::size_t GetPosition(wcr::wid::WEngId id) const {
            assert(position_track.Contains(id.GetId()));
            return position_track.DensePosition(id.GetId());
        }

        std::size_t GetOffset(wcr::wid::WEngId id) const {
            return GetPosition() * block_size_;
        }

        std::size_t BlocksSize(std::size_t blocks_count) const {
            return blocks_count * block_size_;
        }

    private:

        TSparseSet<std::uint8_t> position_track;
        std::array<WVkUBO, FramesInFlight> vk_buffers;

        std::size_t block_size_;
        std::size_t elements_max_count_;
        VkDevice device_;
    };
        

}
