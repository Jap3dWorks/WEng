#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/UBOManager/UBOData.hpp"
#include "WCore/Execution.hpp"

#include <limits>
#include <type_traits>
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
            {
                TypeErasureInitReg();
            }

    public:

        template<std::uint8_t FrameFlag, BlockSizeIntT BlockSize>
        constexpr auto * GetUBOData() {
            return TypeErasureFns<FrameFlag,BlockSize>::GetUboDataConst(*this);
        }

        template<std::uint8_t FrameFlag>
        void Add(BlockSizeIntT block_size,
                 std::vector<wcr::wid::WEngId> ids,
                 void * data) {
            GetTEFnContainer<FrameFlag>[block_size]
                .add(*this, std::move(ids), data);
        }

        template<std::uint8_t FrameFlag>
        void Update(BlockSizeIntT block_size,
                    std::uint8_t frame_index,
                    std::vector<wcr::wid::WEngId> ids,
                    void * data) {
            
            GetTEFnContainer<FrameFlag>[block_size]
                .update(*this,
                        frame_index,
                        std::move(ids),
                        data);
        }

        template<std::uint8_t FrameFlag>
        void Remove(BlockSizeIntT block_size, std::vector<wcr::wid::WEngId> ids) {
            GetTEFnContainer<FrameFlag>[block_size].remove(*this, std::move(ids));
        }

        template<std::uint8_t FrameFlag>
        bool Contains(BlockSizeIntT block_size, wcr::wid::WEngId id) const {
            return GetTEFnContainer<FrameFlag>()[block_size].contains(*this, id);
        }

        template<std::uint8_t FrameFlag>
        std::size_t GetOffset(BlockSizeIntT block_size, wcr::wid::WEngId id) const {
            return GetTEFnContainer<FrameFlag>()[block_size].get_offset(*this, id);
        }

        template<std::uint8_t FrameFlag>
        WVkUBO GetUBO(BlockSizeIntT block_size,
                      std::uint8_t frame_index) const {
            return GetTEFnContainer<FrameFlag>()[block_size].get_UBO(*this, frame_index);
        }

    private:

        struct Vkn{
            VkDevice device;
            VkPhysicalDevice physical_device;
        } vkn_;

        std::size_t ubo_elements;

        struct UniqueVoidPtr {
            static inline void NoDelete(void*) {}

            std::unique_ptr<void, void(*)(void*)> ptr{nullptr, &NoDelete};

            void * get() const {return ptr.get(); }
        };
        
        std::unordered_map<BlockSizeIntT,
                           UniqueVoidPtr> static_frame_ubo_{};

        std::unordered_map<BlockSizeIntT,
                           UniqueVoidPtr> dynamic_frame_ubo_{};

        template<std::uint8_t FrameFlag, typename T>
        requires requires{
            {FrameFlag == STATIC_FRAME_FLAG || FrameFlag == DYNAMIC_FRAME_FLAG};
            {std::is_same_v<std::decay_t<T>, DynamicUBOManager>};
        }
        static inline constexpr auto & GetContainer(T && self) {
            if constexpr(FrameFlag == STATIC_FRAME_FLAG) {
                return std::forward<T>(self).static_frame_ubo_;
            }
            else {
                return std::forward<T>(self).dynamic_frame_ubo_;
            }
        }
        
        template<std::uint8_t FrameFlag, BlockSizeIntT BlockSize>
        requires (FrameFlag == STATIC_FRAME_FLAG || FrameFlag == DYNAMIC_FRAME_FLAG)
        struct TypeErasureFns {

            using UBODataT = UBOData<FrameFlag, BlockSize>;

            static inline void Destructor(void * ptr) {
                UBODataT* ubo_ptr = reinterpret_cast<UBODataT *>(ptr);
                ubo_ptr->~UBOData();
                delete ubo_ptr;
            }

            static inline UniqueVoidPtr CreateUBOData(
                DynamicUBOManager const & self
                ) {
                return {
                    std::unique_ptr<UBODataT, void(*)(void*)>(
                        new UBODataT(self.vkn_.device,
                                     self.vkn_.physical_device,
                                     self.ubo_elements),
                        Destructor
                        )
                };
            }

            static inline void EnsureContainer(DynamicUBOManager & self) {
                if(!GetContainer<FrameFlag>(self).contains(BlockSize)) {
                    GetContainer<FrameFlag>(self).try_emplace(
                        BlockSize,
                        CreateUBOData(self)
                        );
                }
            }

            static inline UBODataT * GetUboData(DynamicUBOManager & self) {
                EnsureContainer(self);

                return reinterpret_cast<UBODataT*>(
                    GetContainer<FrameFlag>(self)[BlockSize].get()
                    );
            }

            static inline UBODataT const * GetUboDataConst(DynamicUBOManager const & self) {
                return reinterpret_cast<UBODataT const *>(
                    GetContainer<FrameFlag>(self).at(BlockSize).get()
                    );
            }

            static inline void Add(
                DynamicUBOManager & self,
                std::vector<wcr::wid::WEngId> ids,
                void * data) {
                std::span<BlockSizeT<BlockSize>> blck_dt{
                    reinterpret_cast<BlockSizeT<BlockSize>*>(data),
                    ids.size()};
                
                GetUboData(self)->Add(std::move(ids), blck_dt);
            }

            static inline void Update(
                DynamicUBOManager & self,
                std::uint8_t frame_index, 
                std::vector<wcr::wid::WEngId> ids,
                void * data) {
                std::span<BlockSizeT<BlockSize>> blck_dt {
                    reinterpret_cast<BlockSizeT<BlockSize>*>(data),
                    ids.size()};
                
                GetUboData(self)->Update(
                    std::min(static_cast<std::uint8_t>(FrameFlag - 1), frame_index),
                    std::move(ids), 
                    blck_dt
                    );
            }

            static inline void Remove(
                DynamicUBOManager & self,
                std::vector<wcr::wid::WEngId> ids
                ) {
                GetUboData(self)->Remove(std::move(ids));
            }

            static inline bool Contains(
                DynamicUBOManager const & self,
                wcr::wid::WEngId id
                ) {
                return GetUboDataConst(self)->Contains(id);
            }

            static inline std::size_t GetOffset(
                DynamicUBOManager const & self,
                wcr::wid::WEngId id) {
                return GetUboDataConst(self)->GetOffset(id);
            }

            static inline WVkUBO GetUBO(
                DynamicUBOManager const & self,
                std::uint8_t frame_index) {
                return GetUboDataConst(self)->GetUBO(frame_index);
            }
            
        };

        using AddFnT = decltype(TypeErasureFns<1,16>::Add)*;
        using UpdateFnT = decltype(TypeErasureFns<1,16>::Update)*;
        using RemoveFnT = decltype(TypeErasureFns<1,16>::Remove)*;
        using ContainsFnT = decltype(TypeErasureFns<1,16>::Contains)*;
        using GetOffsetFnT = decltype(TypeErasureFns<1,16>::GetOffset)*;
        using GetUBOFnT = decltype(TypeErasureFns<1,16>::GetUBO)*;

        struct TERegister {
            AddFnT add;
            UpdateFnT update;
            RemoveFnT remove;
            ContainsFnT contains;
            GetOffsetFnT get_offset;
            GetUBOFnT get_UBO;
        };

        template<std::uint8_t FramesFlag>
        constexpr static auto & GetTEFnContainer() {
            if constexpr(FramesFlag == STATIC_FRAME_FLAG) {
                return te_static_reg_fn;
            }
            else {
                return te_dynamic_reg_fn;
            }
        }

        static inline std::unordered_map<BlockSizeIntT, TERegister>
        te_static_reg_fn{};

        static inline std::unordered_map<BlockSizeIntT, TERegister>
        te_dynamic_reg_fn{};

        static constexpr auto BlockSizesSequence() {

            auto index_seq =
                std::make_integer_sequence<
                    BlockSizeIntT,
                    (std::numeric_limits<BlockSizeIntT>::max()/16) + 1>();

            auto make_seq = []
                <BlockSizeIntT... Ints>
                (std::integer_sequence<BlockSizeIntT, Ints...> idxseq) constexpr -> auto {
                return std::integer_sequence<BlockSizeIntT, (Ints * 16)...>{};
            };

            return make_seq(index_seq);
        }

        static void TypeErasureInitReg() {

            auto reg_erasure_fn = [] <std::uint8_t FramesFlag, BlockSizeIntT ... Ints>
                (std::integer_sequence<std::uint8_t, Ints...>) {
                ((GetTEFnContainer<FramesFlag>()[Ints].add=
                  TypeErasureFns<FramesFlag, Ints>::Add), ...);
                
                ((GetTEFnContainer<FramesFlag>()[Ints].update=
                  TypeErasureFns<FramesFlag, Ints>::Update), ...);
                
                ((GetTEFnContainer<FramesFlag>()[Ints].remove=
                  TypeErasureFns<FramesFlag, Ints>::Remove), ...);

                ((GetTEFnContainer<FramesFlag>()[Ints].contains=
                  TypeErasureFns<FramesFlag, Ints>::Contains), ...);

                ((GetTEFnContainer<FramesFlag>()[Ints].get_offset=
                  TypeErasureFns<FramesFlag, Ints>::GetOffset), ...);

                ((GetTEFnContainer<FramesFlag>()[Ints].get_UBO=
                  TypeErasureFns<FramesFlag, Ints>::GetUBO), ...);
            };

            WCORE_EXECUTE_ONCE(

                reg_erasure_fn.template operator()<STATIC_FRAME_FLAG> (BlockSizesSequence());

                reg_erasure_fn.template operator()<DYNAMIC_FRAME_FLAG> (BlockSizesSequence());
                
                );
        }
    };
    
}
