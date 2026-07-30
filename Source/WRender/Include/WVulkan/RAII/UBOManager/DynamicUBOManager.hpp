#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/UBOManager/UBOData.hpp"

#include <vulkan/vulkan_core.h>
#include <unordered_map>
#include <variant>
#include <cstdint>
#include <algorithm>

namespace wvk::raii::ubo_manager {

    /** Dynamic UBOs Manager */
    template<std::uint8_t FramesInFlight>
    class DynamicUBOManager{

    public:

        static inline constexpr std::uint32_t INITIAL_BUFFER_SIZE{1024};
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

        template<std::uint8_t FrameFlag, typename T>
        void Add(std::vector<wcr::wid::WEngId> ids, std::span<T> & data) {
            GetUboData<FrameFlag, T>()->Add(std::move(ids), data);
        }

        template<std::uint8_t FrameFlag, typename T>
        void Update(std::uint8_t frame_index, 
                    std::vector<wcr::wid::WEngId> ids, 
                    std::span<T> & data) {
            GetUboData<FrameFlag, T>()->Update(
                std::min(static_cast<std::uint8_t>(FrameFlag - 1), frame_index),
                std::move(ids), 
                data
                );
        }

        template<std::uint8_t FrameFlag, typename T>
        void Remove(std::vector<wcr::wid::WEngId> ids) {
            GetUboData<FrameFlag, T>()->Remove(std::move(ids));
        }

        template<std::uint8_t FrameFlag, typename T>
        bool Contains(wcr::wid::WEngId id) const {
            return GetUboData<FrameFlag, T>()->Contains(id);
        }

        template<std::uint8_t FrameFlag, typename T>
        std::size_t GetOffset(wcr::wid::WEngId id) const {
            return GetUboData<FrameFlag, T>()->GetOffset();
        }

        template<std::uint8_t FrameFlag, typename T>
        WVkUBO GetUBO(std::uint8_t frame_index) const {
            return GetUboData<FrameFlag, T>()->GetUBO(frame_index);
        }

    private:

        struct Vkn{
            VkDevice device;
            VkPhysicalDevice physical_device;
        } vkn_;

        std::size_t ubo_elements;
        
        // Data block size . SpareSet
        std::unordered_map<std::size_t,
                           std::unique_ptr<UboDataBase<STATIC_FRAME_FLAG>>> static_frame_ubo{};

        // Reserves one block for each frame in flight continously.
        std::unordered_map<std::size_t,
                           std::unique_ptr<UboDataBase<DYNAMIC_FRAME_FLAG>>> dynamic_frame_ubo{};

        template<std::uint8_t FrameFlag>
        requires (FrameFlag == STATIC_FRAME_FLAG || FrameFlag == DYNAMIC_FRAME_FLAG)
        constexpr auto & GetContainer() {
            if constexpr(FrameFlag == STATIC_FRAME_FLAG) {
                return static_frame_ubo;
            }
            else {
                return dynamic_frame_ubo;
            }
        }

        template<std::uint8_t FrameFlag, typename T>
        requires (FrameFlag == STATIC_FRAME_FLAG || FrameFlag == DYNAMIC_FRAME_FLAG)
        UboData<FrameFlag, sizeof(T)> * GetUboData() {

            if(!GetContainer<FrameFlag>().contains(sizeof(T))) {
                GetContainer<FrameFlag>()[sizeof(T)] = UboData<FrameFlag, sizeof(T)>{
                    vkn_.device,
                    vkn_.physical_device,
                    ubo_elements
                };

                return static_cast<UboData<FrameFlag, sizeof(T)>*>(
                    GetContainer<FrameFlag>()[sizeof(T)].get()
                    );
            }
        }

    };
    
}
