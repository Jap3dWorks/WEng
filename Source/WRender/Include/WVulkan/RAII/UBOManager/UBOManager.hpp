#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WVulkan/RAII/UBOManager/UBOData.hpp"

#include <vulkan/vulkan_core.h>
#include <unordered_map>
#include <variant>

namespace wvk::raii::ubo_manager {

    template<std::uint8_t FramesInFlight>
    class UBOManager{

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


        
        void Clear(VkDevice device);

        void Reg(wcr::wid::WEngId set_id, std::size_t ubo_id);

        std::size_t CreateUBO(VkDevice device,
                              VkPhysicalDevice pdevice,
                              wcr::wid::WEngId id,
                              std::size_t ubo_size,
                              void const * initial_data);

        void DestroyUBOs(wcr::wid::WEngId wid, VkDevice device);


    private:

        struct Vkn{
            VkDevice device;
            VkPhysicalDevice physical_device;
            VkCommandPool command_pool;
        } vkn_;

        // Data block size . SpareSet
        std::unordered_map<std::uint8_t, UboData<1>> static_ubo;

        // Reserves one block for each frame in flight continously.
        std::unordered_map<std::uint8_t, UboData<FramesInFlight>> dynamic_ubo;



    };
    
}
