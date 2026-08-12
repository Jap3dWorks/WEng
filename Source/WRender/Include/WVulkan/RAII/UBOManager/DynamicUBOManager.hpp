#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/UBOManager/BlockSizeUBOs.hpp"
#include "WCore/Execution.hpp"
#include "WCore/Memory.hpp"

#include <limits>
#include <type_traits>
#include <utility>
#include <vulkan/vulkan_core.h>
#include <unordered_map>
#include <cstdint>
#include <algorithm>

namespace wvk::raii::ubo_manager {

    static inline constexpr std::uint32_t INITIAL_UBO_COUNT{1024};

    template<std::uint8_t Frames, BlockSizeIntT BlockSize>
    struct BlockSizeUBOs_Impl {

        using BlockSizeUBOsT = BlockSizeUBOs<Frames, BlockSize>;

        static inline constexpr BlockSizeUBOsT * CastBlockSizeUBOs(void * ptr) {
            return reinterpret_cast<BlockSizeUBOsT*>(
                ptr
                );
        }

        static inline constexpr BlockSizeUBOsT const * CastBlockSizeUBOsConst(void const * ptr) {
            return reinterpret_cast<BlockSizeUBOsT const *>(
                ptr
                );
        }
            
        static inline void Deleter(void * ptr) {
            BlockSizeUBOsT* ubo_ptr = reinterpret_cast<BlockSizeUBOsT *>(ptr);
            delete ubo_ptr;
        }

        static inline wcr::memory::UniqueVoidPtr Create(
            VkDevice device, VkPhysicalDevice phy_device, std::size_t count) {
            return wcr::memory::UniqueVoidPtr::MakeUnique<BlockSizeUBOsT>(
                Deleter,
                device,
                phy_device,
                count
                );
        }

        static inline void Add(
            void * block_size_ubos,
            std::vector<wcr::wid::WEngId> ids,
            void const * data) {
            std::span<BlockSizeT<BlockSize> const > blck_dt{
                reinterpret_cast<BlockSizeT<BlockSize> const *>(data),
                ids.size()};
                
            CastBlockSizeUBOs(block_size_ubos)->Add(std::move(ids), blck_dt);
        }

        static inline void Update(
            void * block_size_ubos,
            std::uint8_t frame_index, 
            std::vector<wcr::wid::WEngId> ids,
            void const * data) {
            std::span<BlockSizeT<BlockSize> const> blck_dt {
                reinterpret_cast<BlockSizeT<BlockSize> const *>(data),
                ids.size()};
                
            CastBlockSizeUBOs(block_size_ubos)->Update(
                std::min(static_cast<std::uint8_t>(Frames - 1), frame_index),
                std::move(ids), 
                blck_dt
                );
        }

        static inline std::vector<wcr::wid::WEngId> Remove(
            void * block_size_ubos,
            std::vector<wcr::wid::WEngId> ids
            ) {
            return CastBlockSizeUBOs(block_size_ubos)->Remove(std::move(ids));
        }

        static inline bool Contains(
            void const * block_size_ubos,
            wcr::wid::WEngId id
            ) {

            return CastBlockSizeUBOsConst(block_size_ubos)->Contains(id);
        }

        static inline std::uint32_t GetOffset(
            void const * block_size_ubos,
            wcr::wid::WEngId id) {
            return CastBlockSizeUBOsConst(block_size_ubos)->GetOffset(id);
        }

        static inline WVkBuffer GetBuffer(
            void const * block_size_ubos,
            std::uint8_t frame_index) {
            return CastBlockSizeUBOsConst(block_size_ubos)->GetBuffer(frame_index);
        }
            
    };
    
    /** Dynamic UBOs Manager */
    template<std::uint8_t FramesInFlight>
    class DynamicUBOManager{

    public:

        static inline constexpr std::uint8_t STATIC_FRAME_FLAG{1};
        static inline constexpr std::uint8_t DYNAMIC_FRAME_FLAG{FramesInFlight};

        /** Creates a container flor UBOS by UBO size */
        using CreateFnT = wcr::memory::UniqueVoidPtr(*) (VkDevice,
                                                         VkPhysicalDevice,
                                                         std::size_t count);

        using AddFnT = void(*)(void * ubosize_container,
                               std::vector<wcr::wid::WEngId>,
                               void const * data);
        
        using UpdateFnT = void(*)(void * ubosize_container,
                                  std::uint8_t frame_index,
                                  std::vector<wcr::wid::WEngId>,
                                  void const * data);

        /** returns moved ids if any after remove operation */
        using RemoveFnT = std::vector<wcr::wid::WEngId> (*)(void * ubosize_container,
                                                            std::vector<wcr::wid::WEngId>);

        using ContainsFnT = bool(*)(void const * ubosize_container,
                                    wcr::wid::WEngId);

        using GetOffsetFnT = std::uint32_t (*)(void const * ubosize_container,
                                               wcr::wid::WEngId);

        using GetBufferFnT = WVkBuffer(*)(void const * ubosize_container,
                                       std::uint8_t frame_index);

        struct TERegister {
            CreateFnT create;            
            AddFnT add;
            UpdateFnT update;
            RemoveFnT remove;
            ContainsFnT contains;
            GetOffsetFnT get_offset;
            GetBufferFnT get_buffer;
        };

        using FnMap = std::unordered_map<BlockSizeIntT, TERegister>;

        using UBOControllerMap = std::unordered_map<BlockSizeIntT,
                                              wcr::memory::UniqueVoidPtr>;

    private:
        
        template<std::uint8_t FramesFlag>
        constexpr static FnMap & GetDefaultFnMap() {
            if constexpr(FramesFlag == STATIC_FRAME_FLAG) {
                static FnMap default_static_fn_map{};
                return default_static_fn_map;
            }
            else {
                static FnMap default_dynamic_fn_map{};
                return default_dynamic_fn_map;
            }
        }

        static void InitDefaultFnMap() {

            auto reg_erasure_fn = [] <std::uint8_t FramesFlag, BlockSizeIntT ... Ints>
                (std::integer_sequence<BlockSizeIntT, Ints...>) {

                ((GetDefaultFnMap<FramesFlag>()[Ints].create =
                  BlockSizeUBOs_Impl<FramesFlag, Ints>::Create), ...);

                ((GetDefaultFnMap<FramesFlag>()[Ints].add=
                  BlockSizeUBOs_Impl<FramesFlag, Ints>::Add), ...);
                
                ((GetDefaultFnMap<FramesFlag>()[Ints].update=
                  BlockSizeUBOs_Impl<FramesFlag, Ints>::Update), ...);
                
                ((GetDefaultFnMap<FramesFlag>()[Ints].remove=
                  BlockSizeUBOs_Impl<FramesFlag, Ints>::Remove), ...);

                ((GetDefaultFnMap<FramesFlag>()[Ints].contains=
                  BlockSizeUBOs_Impl<FramesFlag, Ints>::Contains), ...);

                ((GetDefaultFnMap<FramesFlag>()[Ints].get_offset=
                  BlockSizeUBOs_Impl<FramesFlag, Ints>::GetOffset), ...);

                ((GetDefaultFnMap<FramesFlag>()[Ints].get_buffer=
                  BlockSizeUBOs_Impl<FramesFlag, Ints>::GetBuffer), ...);
            };

            WCORE_STATIC_EXECUTE_ONCE(

                reg_erasure_fn.template operator()<STATIC_FRAME_FLAG> (
                    wvk::raii::ubo_manager::BlockSizesSequence()
                    );

                reg_erasure_fn.template operator()<DYNAMIC_FRAME_FLAG> (
                    wvk::raii::ubo_manager::BlockSizesSequence()
                    );
                );
        }

    public:

        DynamicUBOManager() = default;
        DynamicUBOManager(const DynamicUBOManager&) = delete;
        DynamicUBOManager(DynamicUBOManager&&) = default;
        DynamicUBOManager& operator=(const DynamicUBOManager&) = delete;
        DynamicUBOManager& operator=(DynamicUBOManager&&) = default;
        ~DynamicUBOManager() = default;

        DynamicUBOManager(
            VkDevice device,
            VkPhysicalDevice physical_device,
            std::size_t initial_ubo_count
            ) : vkn_(device, physical_device),
                ubo_elements(initial_ubo_count)
            {
                InitDefaultFnMap();

                VkPhysicalDeviceProperties physical_properties;
                vkGetPhysicalDeviceProperties(physical_device, &physical_properties);

                // minUniformBufferOffsetAlignment is in bytes
                InitLocalFnMap(physical_properties.limits.minUniformBufferOffsetAlignment);
            }

    public:

        template<std::uint8_t FrameFlag>
        void Add(BlockSizeIntT block_size,
                 std::vector<wcr::wid::WEngId> ids,
                 void const * data) {

            // asserts that the ubo size is correctly aligned
            assert(GetFnMap<FrameFlag>().contains(block_size));
            
            EnsureUBOController<FrameFlag>(block_size);

            GetFnMap<FrameFlag>().at(block_size)
                .add(GetUBOControllerMap<FrameFlag>().at(block_size).get(),
                     std::move(ids),
                     data);
        }

        template<std::uint8_t FrameFlag>
        void Update(BlockSizeIntT block_size,
                    std::uint8_t frame_index,
                    std::vector<wcr::wid::WEngId> ids,
                    void const * data) {
            
            GetFnMap<FrameFlag>().at(block_size)
                .update(GetUBOControllerMap<FrameFlag>().at(block_size).get(),
                        frame_index,
                        std::move(ids),
                        data);
        }

        template<std::uint8_t FrameFlag>
        std::vector<wcr::wid::WEngId> Remove(BlockSizeIntT block_size,
                                             std::vector<wcr::wid::WEngId> ids) {
            return GetFnMap<FrameFlag>().at(block_size).remove(
                GetUBOControllerMap<FrameFlag>().at(block_size),
                std::move(ids)
                );
        }

        template<std::uint8_t FrameFlag>
        bool Contains(BlockSizeIntT block_size, wcr::wid::WEngId id) const {

            return GetUBOControllerMap<FrameFlag>().contains(block_size) &&
                GetFnMap<FrameFlag>().at(block_size).contains(
                    GetUBOControllerMap<FrameFlag>().at(block_size).get(),
                    id);
        }

        template<std::uint8_t FrameFlag>
        std::uint32_t GetOffset(BlockSizeIntT block_size, wcr::wid::WEngId id) const {
            return GetFnMap<FrameFlag>().at(block_size).get_offset(
                GetUBOControllerMap<FrameFlag>().at(block_size).get(),
                id);
        }

        template<std::uint8_t FrameFlag>
        WVkBuffer GetBuffer(BlockSizeIntT block_size,
                            std::uint8_t frame_index) const {
            return GetFnMap<FrameFlag>().at(block_size).get_buffer(
                GetUBOControllerMap<FrameFlag>().at(block_size).get(),
                frame_index);
        }

    private:

        void InitLocalFnMap(BlockSizeIntT min_physical_alignment) {
            auto init_local_map = [this, &min_physical_alignment]
                <std::uint8_t FrameFlag>
                () {
                for(auto reg : GetDefaultFnMap<FrameFlag>()) {
                    if (0 == reg.first % min_physical_alignment) {
                        GetFnMap<FrameFlag>()
                        .insert(std::move(reg));
                    }
                    else {
                        BlockSizeIntT next_align =
                            ((reg.first / min_physical_alignment) + 1) * min_physical_alignment;
                        
                        GetFnMap<FrameFlag>()
                            .insert({reg.first, GetDefaultFnMap<FrameFlag>()[next_align]});
                    }
                }
            };

            init_local_map.template operator()<STATIC_FRAME_FLAG>();
            init_local_map.template operator()<DYNAMIC_FRAME_FLAG>();
        }

        template<std::uint8_t FrameFlag>
        FnMap const & GetFnMap() const {
            return __GetFnMap<FrameFlag>(*this);
        }

        template<std::uint8_t FrameFlag>
        FnMap & GetFnMap() {
            return __GetFnMap<FrameFlag>(*this);
        }

        template<std::uint8_t FramesFlag>
        UBOControllerMap & GetUBOControllerMap() {
            return __GetUBOControllerMap<FramesFlag>(*this);
        }

        template<std::uint8_t FramesFlag>
        UBOControllerMap const & GetUBOControllerMap() const {
            return __GetUBOControllerMap<FramesFlag>(*this);
        }

        template<std::uint8_t FrameFlag, typename T>
        requires requires{
            {FrameFlag == STATIC_FRAME_FLAG || FrameFlag == DYNAMIC_FRAME_FLAG};
            {std::is_same_v<std::decay_t<T>, DynamicUBOManager>};            
        }
        static inline auto & __GetFnMap(T && self) {
            if constexpr(FrameFlag == STATIC_FRAME_FLAG) {
                return std::forward<T>(self).static_frame_.fn_map;
            }
            else {
                return std::forward<T>(self).dynamic_frame_.fn_map;
            }
        }

        template<std::uint8_t FrameFlag, typename T>
        requires requires{
            {FrameFlag == STATIC_FRAME_FLAG || FrameFlag == DYNAMIC_FRAME_FLAG};
            {std::is_same_v<std::decay_t<T>, DynamicUBOManager>};
        }
        static inline constexpr auto & __GetUBOControllerMap(T && self) {
            if constexpr(FrameFlag == STATIC_FRAME_FLAG) {
                return std::forward<T>(self).static_frame_.ubo_ctrlr_map;
            }
            else {
                return std::forward<T>(self).dynamic_frame_.ubo_ctrlr_map;
            }
        }

        template<std::uint8_t FrameFlag>
        void EnsureUBOController(BlockSizeIntT block_size) {
            if(!GetUBOControllerMap<FrameFlag>().contains(block_size)) {
                GetUBOControllerMap<FrameFlag>().insert(
                    {block_size,
                     GetFnMap<FrameFlag>().at(block_size).create(vkn_.device,
                                                                 vkn_.physical_device,
                                                                 ubo_elements)}
                    );
            }
        }

    private:
        
        struct Vkn{
            VkDevice device;
            VkPhysicalDevice physical_device;
        } vkn_;

        std::size_t ubo_elements;

        struct UBOData {
            UBOControllerMap ubo_ctrlr_map{};
            FnMap fn_map{};
        };

        UBOData static_frame_{};
        UBOData dynamic_frame_{};

    };
    
}
