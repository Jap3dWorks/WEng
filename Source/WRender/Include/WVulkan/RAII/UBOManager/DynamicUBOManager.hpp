#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/UBOManager/UBOData.hpp"

#include <utility>
#include <vulkan/vulkan_core.h>
#include <unordered_map>
#include <cstdint>
#include <algorithm>

namespace wvk::raii::ubo_manager {

    static inline constexpr std::uint32_t INITIAL_BUFFER_SIZE{1024};


    /** Dynamic UBOs Manager */
    template<std::uint8_t FramesInFlight>
    class DynamicUBOManager{

    public:

        static inline constexpr std::uint8_t STATIC_FRAME_FLAG{1};
        static inline constexpr std::uint8_t DYNAMIC_FRAME_FLAG{FramesInFlight};

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
            std::size_t in_ubo_elements
            ) : vkn_(device, physical_device),
                ubo_elements(in_ubo_elements)
            {}

    public:

        template<std::uint8_t FrameFlag, std::uint8_t BlockSize>
        void Add(std::vector<wcr::wid::WEngId> ids, std::span<BlockSizeType<BlockSize>> & data) {
            GetUboData<FrameFlag, BlockSize>(*this)->Add(std::move(ids), data);
        }

        template<std::uint8_t FrameFlag, std::uint8_t BlockSize>
        void Update(std::uint8_t frame_index, 
                    std::vector<wcr::wid::WEngId> ids, 
                    std::span<BlockSizeType<BlockSize>> & data) {
            GetUboData<FrameFlag, BlockSize>(*this)->Update(
                std::min(static_cast<std::uint8_t>(FrameFlag - 1), frame_index),
                std::move(ids), 
                data
                );
        }

        template<std::uint8_t FrameFlag, std::uint8_t BlockSize>
        void Remove(std::vector<wcr::wid::WEngId> ids) {
            GetUboData<FrameFlag, BlockSize>(*this)->Remove(std::move(ids));
        }

        template<std::uint8_t FrameFlag, std::uint8_t BlockSize>
        bool Contains(wcr::wid::WEngId id) const {
            return GetUboData<FrameFlag, BlockSize>(*this)->Contains(id);
        }

        template<std::uint8_t FrameFlag, std::uint8_t BlockSize>
        std::size_t GetOffset(wcr::wid::WEngId id) const {
            return GetUboData<FrameFlag, BlockSize>(*this)->GetOffset();
        }

        template<std::uint8_t FrameFlag, std::uint8_t BlockSize>
        WVkUBO GetUBO(std::uint8_t frame_index) const {
            return GetUboData<FrameFlag, BlockSize>(*this)->GetUBO(frame_index);
        }

    private:

        struct Vkn{
            VkDevice device;
            VkPhysicalDevice physical_device;
        } vkn_;

        std::size_t ubo_elements;
        
        // Data block size . SpareSet
        std::unordered_map<std::size_t,
                           std::unique_ptr<UboDataBase>> static_frame_ubo{};

        // Reserves one block for each frame in flight continously.
        std::unordered_map<std::size_t,
                           std::unique_ptr<UboDataBase>> dynamic_frame_ubo{};

        template<std::uint8_t FrameFlag>
        requires (FrameFlag == STATIC_FRAME_FLAG || FrameFlag == DYNAMIC_FRAME_FLAG)
        static inline constexpr auto & GetContainer(DynamicUBOManager & self) {
            if constexpr(FrameFlag == STATIC_FRAME_FLAG) {
                return self.static_frame_ubo;
            }
            else {
                return self.dynamic_frame_ubo;
            }
        }

        template<std::uint8_t FrameFlag, std::uint8_t BlockSize>
        requires (FrameFlag == STATIC_FRAME_FLAG || FrameFlag == DYNAMIC_FRAME_FLAG)
        static inline UboData<FrameFlag, BlockSize> * GetUboData(DynamicUBOManager & self) {

            if(!GetContainer<FrameFlag>(self).contains(BlockSize)) {
                GetContainer<FrameFlag>(self)[BlockSize] = UboData<FrameFlag, BlockSize>{
                    self.vkn_.device,
                    self.vkn_.physical_device,
                    self.ubo_elements
                };

                return static_cast<UboData<FrameFlag, BlockSize>*>(
                    GetContainer<FrameFlag>(self)[BlockSize].get()
                    );
            }
        }

        template<std::uint8_t FrameFlag, std::uint8_t BlockSize>
        struct TypeErasure {

            static inline void Add(
                DynamicUBOManager & self,
                std::vector<wcr::wid::WEngId> ids,
                void * data) {
                std::span<BlockSizeType<BlockSize>> blck_dt{
                    reinterpret_cast<BlockSizeType<BlockSize>*>(data),
                    ids.size()};
                GetUboData<FrameFlag, BlockSize>(self)->Add(std::move(ids), blck_dt);
            }

            static inline void Update(
                DynamicUBOManager & self,
                std::uint8_t frame_index, 
                std::vector<wcr::wid::WEngId> ids,
                void * data) {
                std::span<BlockSizeType<BlockSize>> blck_dt {
                    reinterpret_cast<BlockSizeType<BlockSize>*>(data),
                    ids.size()};
                
                GetUboData<FrameFlag, BlockSize>(self)->Update(
                    std::min(static_cast<std::uint8_t>(FrameFlag - 1), frame_index),
                    std::move(ids), 
                    blck_dt
                    );
            }

            static inline void Remove(
                DynamicUBOManager & self,
                std::vector<wcr::wid::WEngId> ids) {
                GetUboData<FrameFlag, BlockSize>(self)->Remove(std::move(ids));
            }

            static inline bool Contains(
                DynamicUBOManager & self,
                wcr::wid::WEngId id) {
                return GetUboData<FrameFlag, BlockSize>(self)->Contains(id);
            }

            static inline std::size_t GetOffset(
                DynamicUBOManager & self,
                wcr::wid::WEngId id) {
                return GetUboData<FrameFlag, BlockSize>(self)->GetOffset();
            }

            static inline WVkUBO GetUBO(
                DynamicUBOManager & self,
                std::uint8_t frame_index) {
                return GetUboData<FrameFlag, BlockSize>(self)->GetUBO(frame_index);
            }
            
        };

        using TEAddFnT = decltype(TypeErasure<1,16>::Add)*;
        using TEUpdateFnT = decltype(TypeErasure<1,16>::Update)*;
        using TERemoveFnT = decltype(TypeErasure<1,16>::Remove)*;
        using TEContainsFnT = decltype(TypeErasure<1,16>::Contains)*;
        using TEGetOffsetFnT = decltype(TypeErasure<1,16>::GetOffset)*;
        using TEGetUBOFbT = decltype(TypeErasure<1,16>::GetUBO)*;

        struct TERegister {
            TEAddFnT add;
            TEUpdateFnT update;
            TERemoveFnT remove;
            TEContainsFnT contains;
            TEGetOffsetFnT offset;
            TEGetUBOFbT get_ubo;
        };

        static inline std::unordered_map<std::uint8_t, TERegister>
        te_static_reg_fn{};

        static inline std::unordered_map<std::uint8_t, TERegister>
        te_dynamic_reg_fn{};

        template<std::uint8_t FramesFlag>
        constexpr static auto & GetRegFnContainer() {
            if constexpr(FramesFlag == STATIC_FRAME_FLAG) {
                return te_static_reg_fn;
            }
            else {
                return te_dynamic_reg_fn;
            }
        }


        template<std::size_t... Is>
        static constexpr auto make_sequence(std::index_sequence<Is...>)
            {
                return std::integer_sequence<std::uint8_t,
                                             static_cast<std::uint8_t>((Is + 1) * 16)...>{};
            }


        using Seq = decltype(make_sequence(std::make_index_sequence<15>{}));


        static std::unordered_map<std::uint8_t, TEAddFnT> TEInitReg() {

            static bool called=false;

            if (called) return;

            auto reg_erasure_fn = [] <std::uint8_t FramesFlag, std::uint8_t ... Ints>
                (std::integer_sequence<std::uint8_t, Ints...>) {
                ((GetRegFnContainer<FramesFlag>()[Ints].add=
                  TypeErasure<FramesFlag, Ints>::Add), ...);
                ((GetRegFncontainer<FramesFlag>()[Ints].update=
                  TypeErasure<FramesFlag, Ints>::Update), ...);
                ((GetRegFncontainer<FramesFlag>()[Ints].remove=
                  TypeErasure<FramesFlag, Ints>::Remove), ...);
                ((GetRegFncontainer<FramesFlag>()[Ints].contains=
                  TypeErasure<FramesFlag, Ints>::Contains), ...);
                ((GetRegFncontainer<FramesFlag>()[Ints].offset=
                  TypeErasure<FramesFlag, Ints>::Offset), ...);
                ((GetRegFncontainer<FramesFlag>()[Ints].get_ubo=
                  TypeErasure<FramesFlag, Ints>::GetUBO), ...);
                
            };

            reg_erasure_fn. template operator()<STATIC_FRAME_FLAG>
                (make_sequence(std::make_index_sequence<15>()));


            reg_erasure_fn. template operator()<DYNAMIC_FRAME_FLAG>
                (make_sequence(std::make_index_sequence<15>()));


            called = true;

        }

    };
    
}
