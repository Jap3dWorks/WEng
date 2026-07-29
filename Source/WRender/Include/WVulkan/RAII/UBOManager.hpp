#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WCore/TSparseSet.hpp"

#include <vulkan/vulkan_core.h>
#include <unordered_map>
#include <variant>

namespace wvk::raii {

    template<std::uint8_t FramesInFlight>
    class WRENDER_API UBOManager{

    // TODO extract ubo management from AssetRenderData.
    // WARNING UBOs can be frame dependent.
    // using WVkUBODb = TObjectDataBase<WVkUBO, void, wcr::wid::WEngId::IdType>;
    public:

        static inline constexpr std::uint32_t INITIAL_BUFFER_SIZE{1024};

    public:

        void CreateUBO(wcr::wid::WEngId id,
                       std::size_t ubo_size,
                       wct::render::ERPipeParamType,
                       void const * initial_data);

    private:

        template<std::uint8_t Frames>
        struct UboData {

            void Create(wcr::wid::WEngId);
            void Remove(wcr::wid::WEngId);

            bool Contains(wcr::wid::WEngId);

            void Update(std::uint8_t frame_index, wcr::wid::WEngId, const void* data);

            std::size_t GetPosition(std::uint8_t frame_index, wcr::wid::WEngId);

            std::size_t GetOffset(std::uint8_t frame_index, wcr::wid::WEngId);

            // Something like update block too

            TSparseSet<wcr::wid::WEngId> position_track;
            VkBuffer vk_buffer;
            std::size_t block_size;
        };
        
        // Data block size . SpareSet
        std::unordered_map<std::uint8_t, UboData<1>> static_ubo;

        // Reserves one block for each frame in flight continously.
        std::unordered_map<std::uint8_t, UboData<FramesInFlight>> dynamic_ubo;

        // wct::render::ERPipeParamType::UBO_Component_Dynamic;

        struct Vkn{
            VkDevice device;
            VkPhysicalDevice physical_device;
            VkCommandPool command_pool;
        } vkn_;

        std::unordered_map<
        wcr::wid::WEngId,
        std::variant<std::size_t, std::vector<std::size_t>>
        > ubo_sets{};

        void Clear(VkDevice device);

        void Reg(wcr::wid::WEngId set_id, std::size_t ubo_id);

        std::size_t CreateUBO(VkDevice device, VkPhysicalDevice pdevice,
                              wcr::wid::WEngId id,
                              std::size_t ubo_size, void const * initial_data);

        void DestroyUBOs(wcr::wid::WEngId wid, VkDevice device);
        
  
    };
    
}
