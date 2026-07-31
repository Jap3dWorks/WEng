#pragma once

#include "WVulkan/Vk/WVkBuffer.hpp"
#include "WCore/TSparseSet.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WCore/WDebug.hpp"
#include "WCore/Lists.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <vulkan/vulkan_core.h>
#include <variant>
#include <cassert>
#include <algorithm>
#include <numeric>

// TODO Reescale Buffer size.
 
namespace wvk::raii::ubo_manager {

    using BlockSizeIntT = std::uint8_t;

    template<BlockSizeIntT N>
    struct BlockSizeT : std::false_type {};

    template<BlockSizeIntT N>
    requires (N % 16 == 0)
    struct BlockSizeT<N> : std::true_type {
        static constexpr BlockSizeIntT Size{N};
        std::uint8_t block_size[N];
    };

    static_assert(sizeof(BlockSizeT<16>)== 16);
    static_assert(sizeof(BlockSizeT<32>)== 32);
    static_assert(sizeof(BlockSizeT<64>)== 64);
    static_assert(sizeof(BlockSizeT<128>)== 128);
    static_assert(sizeof(BlockSizeT<240>) == 240);

    template<typename T, T N>
    concept _CBlockSizeIntT = requires {(N % 16 == 0);};

    template<std::uint8_t FramesInFlight, BlockSizeIntT BlockSize> 
    requires (BlockSizeT<BlockSize>::value)
    class UBOData {

    public:

        UBOData() = delete;
        
        UBOData(const UBOData&) = delete;
        UBOData(UBOData&&) = default;
        UBOData& operator=(const UBOData&) = delete;
        UBOData& operator=(UBOData&&) = default;

        ~UBOData() {
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

        UBOData(
            VkDevice device,
            VkPhysicalDevice physical_device,
            std::size_t elements_max_count) :
            device_(device),
            elements_max_count_(elements_max_count) {

            VkDeviceSize device_size = 
                BlockSize * elements_max_count;

            for(auto & vk_buffer : vk_buffers) {
                vk_buffer = wvk::buffer::CreateUBO(
                    device_size,
                    device,
                    physical_device
                    );
            }
        }

        void Add(std::vector<wcr::wid::WEngId> ids,
                 std::span<BlockSizeT<BlockSize>> & data) {

            for(auto id : ids) {
                assert(!position_track.Contains(id.GetId()));
                position_track.Insert(id.GetId(), 1);
            }

            // Assuming that new elements are located at the end of the sparse set.
            std::size_t offset = GetOffset(ids[0]);
            std::size_t size = GetSize(ids.size());

            for(std::uint8_t f=0; f<FramesInFlight; f++) {
                void * ptr = wvk::buffer::MapUBO(vk_buffers[f], device_);

                wvk::buffer::UpdateUBO(
                    ptr,
                    data.data(),
                    size,
                    offset
                    );

                wvk::buffer::UnmapUBO(vk_buffers[f], device_);
            }
        }

        void Update(std::uint8_t frame_index,
                    std::vector<wcr::wid::WEngId> ids,
                    std::span<BlockSizeT<BlockSize>> & data) {
            assert(position_track.Contains(id.GetId()));

            std::vector<std::uint32_t> permutation{};
            permutation.resize(ids.size());

            std::iota(permutation.begin(), permutation.end(), 0);
            
            std::sort(permutation.begin(), permutation.end(), 
                      [&ids, this](auto a, auto b) {
                          return position_track.DensePosition(ids[a].GetId()) <
                              position_track.DensePosition(ids[b].GetId());
                      });

            auto last = std::unique(permutation.begin(), permutation.end(),
                [&ids, this](auto a, auto b) {
                    return position_track.DensePosition(ids[a].GetId()) ==
                        position_track.DensePosition(ids[b].GetId());
                });

            permutation.erase(last, permutation.end());
            wcr::lists::ApplyPermutation(
                ids, permutation
                );

            ids.resize(permutation.size());

            wcr::lists::ApplyPermutation(
                data, permutation
                );

            struct Transaction {
                std::size_t pos;
                std::size_t size;
                BlockSizeT<BlockSize> * data;
            };
            
            std::vector<Transaction> transactions;
            transactions.reserve(ids.size());

            for(std::uint32_t i=0; i<ids.size(); ++i) {
                std::size_t pos = position_track.DensePosition(ids[i].GetId());

                if (transactions.empty() || 
                    transactions.back().pos + 1 != pos) {
                    transactions.emplace_back(pos, 1, &data[i]);
                }
                else {
                    transactions.back().size++;
                }
            }

            if (transactions.empty()) return;

            void * ptr = wvk::buffer::MapUBO(
                vk_buffers[frame_index], device_
                );

            for(auto trn : transactions) {
                wvk::buffer::UpdateUBO(
                    ptr, trn.data, GetSize(trn.size), trn.pos * BlockSize
                    );
            }

            wvk::buffer::UnmapUBO(vk_buffers[frame_index], device_);
        }

        void Remove(std::vector<wcr::wid::WEngId> ids)  {
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
                    transactions.emplace_back(old_pos, new_pos, 1);
                }
                else {
                    // consecutive just update the last transaction
                    transactions.back().old_pos = old_pos;
                    transactions.back().new_pos = new_pos;
                    transactions.back().size += 1; 
                }
            }

            for(std::uint8_t f=0; f < FramesInFlight ; f++) {
                void * ptr = wvk::buffer::MapUBO(vk_buffers[f], device_);

                for(auto & trn : transactions) {
                    std::byte * val_ptr =
                        reinterpret_cast<std::byte*>(ptr) + trn.old_pos * BlockSize;
                    
                    wvk::buffer::UpdateUBO(
                        ptr, val_ptr, GetSize(trn.size), trn.new_pos * BlockSize
                        );
                }
                wvk::buffer::UnmapUBO(vk_buffers[f], device_);
            }
        }

        bool Contains(wcr::wid::WEngId id) const  {
            return position_track.Contains(id.GetId());
        }

        std::size_t GetPosition(wcr::wid::WEngId id) const  {
            assert(position_track.Contains(id.GetId()));
            return position_track.DensePosition(id.GetId());
        }

        std::size_t GetOffset(wcr::wid::WEngId id) const  {
            return GetPosition(id) * BlockSize;
        }

        std::size_t Count() const  {
            return position_track.Count();
        }

        WVkUBO GetUBO(std::uint8_t frame_index) const  {
            return vk_buffers[frame_index];
        }

    private:

        std::size_t GetSize(std::size_t blocks_count) const {
            return blocks_count * BlockSize;
        }

    private:

        // std::uint8_t could contain some useful flags for each UBO
        TSparseSet<std::uint8_t> position_track;
        std::array<WVkUBO, FramesInFlight> vk_buffers;

        std::size_t elements_max_count_;
        VkDevice device_;
    };
        

}
