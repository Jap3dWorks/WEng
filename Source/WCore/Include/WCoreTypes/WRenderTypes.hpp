#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WCore/WConcepts.hpp"

#include <cstdint>
#include <array>
#include <stdexcept>
#include <span>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <utility>

namespace wct::render {

    // Uniform Buffer Objects
    // ----------------------

    /**
     * @brief Uniform buffer data structure.
     */
    struct WModelUBO
    {
        glm::mat4 model;
        glm::mat4 normal_matrix;
    
        // params here
        // ...
    };

    /**
     * @brief parameters to use in a shader.
     */
    struct WPostprocessUBO
    {
        glm::vec4 param1;
        glm::vec4 param2;
        glm::vec4 param3;
        glm::vec4 param4;
    };

    /**
     * @brief Camera Data to be used in the shader.
     */
    struct WCameraUBO {
        glm::mat4 proj{};
        glm::mat4 view{1};
        glm::vec3 pos{1};
        glm::vec3 rot{0};  // pitch, yaw, roll
        glm::vec3 scale{1};
        float field_of_view{glm::pi<float>() * 0.25};
        float near_clipping{0.01f};
        float far_clipping{100.f};
    };

    inline constexpr std::uint8_t WENG_MAX_PIPELINE_SHADERS=8;

    struct WRenderSize {
        std::uint32_t width{0};
        std::uint32_t height{0};
    };

    enum class EShaderStageFlag : std::uint8_t {
        None=0,
        Vertex=1,
        Fragment=2,
        Compute=4,
        // Geometry,
        // TessellationControl,
        // TessellationEvaluation
    };

    constexpr const std::array<EShaderStageFlag, 4> SHADER_STAGE_FLAGS_LIST =
    {
        EShaderStageFlag::None,
        EShaderStageFlag::Vertex,
        EShaderStageFlag::Fragment,
        EShaderStageFlag::Compute
    };
    

    constexpr EShaderStageFlag operator|(
        const EShaderStageFlag & l,
        const EShaderStageFlag & r) noexcept {
        return static_cast<EShaderStageFlag>(static_cast<std::uint8_t>(l) |
                                             static_cast<std::uint8_t>(r));
    }

    constexpr EShaderStageFlag operator&(
        const EShaderStageFlag & l,
        const EShaderStageFlag & r) noexcept {
        return static_cast<EShaderStageFlag>(static_cast<std::uint8_t>(l) &
                                             static_cast<std::uint8_t>(r));
    }

    enum class EPipelineType : uint8_t
    {
        Graphics,       // DEPRECATED Default Opaque
        GBuffer,        // GBuffer generation shader
        Offscreen,      // Offscreen render using GBuffers
        Transparency,   // Alpha Blending
        Postprocess,    // Camera shader pipelines
        Compute,        // GPU
        RayTracing,     // Ray Tracing
        Tonemapping,    // Apply tonemapping
        Swap            // Used to write in the swap chain
    };

    namespace {
        template<typename ...Args>
        inline constexpr bool _dispatch_impl(EPipelineType pype_type) {
            return false;
        }

        template<EPipelineType NextType, EPipelineType ... PTypes,
                 typename NextHandler, typename ...Handlers >
        inline constexpr bool _dispatch_impl(EPipelineType pipe_type,
                                             NextHandler&& next_handler,
                                             Handlers&&... handlers) {

            static_assert(sizeof...(PTypes) == sizeof...(handlers), 
                          "Number of pipeline types must match number of handlers");
            
            if (NextType == pipe_type) {
                next_handler();
                return true;
            }
            else {
                return _dispatch_impl<PTypes...>(pipe_type, std::forward<Handlers>(handlers)...);
            }
        }
    }
    
    template<EPipelineType... PipelineTypes, typename... Handlers>
    inline constexpr void pipeline_type_dispatcher(EPipelineType type, Handlers&&... handlers) {

        static_assert(sizeof...(PipelineTypes) == sizeof...(Handlers));
    
        if(!_dispatch_impl<PipelineTypes...>(type, std::forward<Handlers>(handlers)...)) {
            throw std::runtime_error("Unknown type");
        }
    }

    enum class EPipeParamType {
        None,
        Texture,
        Ubo
    };

    struct WPipeParamDescriptorInfo {
        std::uint8_t binding{0};
        EPipeParamType type{EPipeParamType::None};
        EShaderStageFlag stage_flags{EShaderStageFlag::None};
    
        /** @brief UBO range, total UBO size. */
        std::size_t size{0};
    };

    using WPipeParamDescriptorList = std::array<WPipeParamDescriptorInfo, 16>;

    struct WShaderInfo {
        EShaderStageFlag type{EShaderStageFlag::None};
        TName<128> file{};
        // char file[128]{""};   // TODO : use TName based types
        TName<16> entry{};
        // char entry[16]{"main"};
    };

    using WShaderList = std::array<WShaderInfo, WENG_MAX_PIPELINE_SHADERS>;

// Pipeline Parameters Structs
// ---------------------------

    /** Render Pipeline Param Ubo Struct */
    struct WRPParamUbo {
        std::uint16_t binding{0};
        std::span<std::uint8_t> data{};
        std::size_t offset{0};
    };

    template<typename T>
    struct TRPParam {
        std::uint16_t binding{0};
        T value{};
    };

    using WRPParamAsset = TRPParam<wid::WAssetId>;

    using WRPParameterList_WAssetId = std::vector<WRPParamAsset>;
    using WRPParameterList_Ubo = std::vector<WRPParamUbo>;

    template<CCallable<void, const WPipeParamDescriptorInfo &> TFn>
    inline void ForEach(const wct::render::WPipeParamDescriptorList & in_lst, TFn && in_fn) {
        for(const auto& param: in_lst) {
            if (param.type==EPipeParamType::None)
                break;

            std::forward<TFn>(in_fn)(param);
        }
    }

    template<CCallable<void, const WShaderInfo &> TFn>
    inline void ForEach(const WShaderList & in_lst, TFn && in_fn) {
        for(const auto& shd : in_lst) {
            if (shd.type == EShaderStageFlag::None)
                break;

            std::forward<TFn>(in_fn)(shd);
        }
    }

    // Pipeline Params assignment

    struct WPipelineAssignment {
        wid::WAssetId pipeline{};
        wid::WAssetId params{};
    };

    template<std::uint8_t Max=WENG_MAX_ASSET_IDS>
    using WPipelineAssignments = std::array<WPipelineAssignment, Max>;

   // Lighting
   // --------

    struct WPointLight {
        glm::vec3 color{0.5, 0.5, 0.5};
        float radius{10.f};
        glm::vec3 position{0.f, 0.f, 0.f};
        float _padding[1];
    };

    static_assert(sizeof(WPointLight) == 32, "Size must match Vulkan layout");
    static_assert(offsetof(WPointLight, color) == 0, "Color at offset 0");
    static_assert(offsetof(WPointLight, radius) == 12, "Radius at offset 12");
    static_assert(offsetof(WPointLight, position) == 16, "Intensity at offset 16");

    struct WDirectionalLight {
        glm::vec3 color{0.5, 0.5, 0.5};
        float _padding_1;
        glm::vec3 direction{0.f, 0.f, 0.f};
        float _padding_2;
    };

    static_assert(sizeof(WDirectionalLight)==32, "Size must match Vulkan layout");
    static_assert(offsetof(WDirectionalLight, color)==0, "Color at offset 0");
    static_assert(offsetof(WDirectionalLight, direction)==16, "Direction at offset 16");

    struct WAmbientLight {
        glm::vec3 color{0.5, 0.5, 0.5};
        float _padding;
    };

    static_assert(sizeof(WAmbientLight)==16, "Size must match Vulkan layout");
    static_assert(offsetof(WAmbientLight, color)==0, "Color at offset 0");

    struct WLightingUBO {
        static constexpr std::uint32_t MAX_POINT_LIGHTS{64};
        static constexpr std::uint32_t MAX_DIRECTIONAL_LIGHTS{16};

        std::array<WPointLight, MAX_POINT_LIGHTS> point_lights;
        std::array<WDirectionalLight, MAX_DIRECTIONAL_LIGHTS> directional_lights;
        WAmbientLight ambient_light{};

        std::uint32_t point_lights_count{0};
        std::uint32_t directional_lights_count{0};
        float _padding[2];
    };

    static_assert(sizeof(WPointLight) * WLightingUBO::MAX_POINT_LIGHTS +
                  sizeof(WDirectionalLight) * WLightingUBO::MAX_DIRECTIONAL_LIGHTS +
                  sizeof(WAmbientLight) +
                  16 == sizeof(WLightingUBO), "Size must match a Vulkan layout");

}
