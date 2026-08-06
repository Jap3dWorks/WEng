#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WCore/WConcepts.hpp"
#include "WCore/WId.hpp"

#include <cstdint>
#include <array>
#include <stdexcept>
#include <span>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <utility>
#include <variant>

namespace wct::render {

    // Uniform Buffer Objects
    // ----------------------

    /**
     * @brief Uniform buffer data structure.
     */
    struct ModelUBO
    {
        glm::mat4 model;
        glm::mat4 normal_matrix;
    
        // params here
        // ...
    };

    /**
     * @brief parameters to use in a shader.
     */
    struct PostprocessUBO
    {
        glm::vec4 param1;
        glm::vec4 param2;
        glm::vec4 param3;
        glm::vec4 param4;
    };

    /**
     * @brief Camera Data to be used in the shader.
     */
    struct CameraUBO {
        glm::mat4 proj{};
        glm::mat4 view{1};
        glm::mat4 inv_proj{};
        glm::mat4 inv_view{1};
        glm::vec3 pos{1};
        glm::vec3 rot{0};  // pitch, yaw, roll
        glm::vec3 scale{1};
        float field_of_view{glm::pi<float>() * 0.25};
        float near_clipping{0.01f};
        float far_clipping{100.f};
    };

    struct PBRScalarUBO {
        glm::vec4 albedo{0.0, 0.0, 0.0, 0.0};
        glm::vec4 emission{0.0, 0.0, 0.0, 0.0};
        glm::vec4 orm_nscale{1.0, .8, .2, 1.0};
    };

    struct CommonBindings {
        static constexpr std::uint8_t MODEL_SET{2};
        static constexpr std::uint8_t MODEL_UBO{0};
    };

    struct PBRBindings {
        static constexpr std::uint8_t SET{1};

        static constexpr std::uint8_t PARAM_UBO{0};
        static constexpr std::uint8_t ALBEDO_TEXTURE{1};
        static constexpr std::uint8_t EMISSION_TEXTURE{2};
        static constexpr std::uint8_t NORMAL_TEXTURE{3};
        static constexpr std::uint8_t ORM_TEXTURE{4};
    };

    inline constexpr std::uint8_t MAX_PIPELINE_SHADERS=8;

    struct RenderSize {
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

    enum class ERPipeType : uint8_t
    {
        GBuffer,        // GBuffer generation shader
        Lighting,      // Lighting render using GBuffers
        Transparency,   // Alpha Blending
        Postprocess,    // Camera shader pipelines
        Compute,        // GPU
        RayTracing,     // Ray Tracing
        Tonemapping,    // Apply tonemapping
        Swap            // Used to write in the swap chain
    };

    namespace {
        template<typename ...Args>
        inline constexpr bool _dispatch_impl(ERPipeType pype_type) {
            return false;
        }

        template<ERPipeType NextType, ERPipeType ... PTypes,
                 typename NextHandler, typename ...Handlers >
        inline constexpr bool _dispatch_impl(ERPipeType pipe_type,
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
    
    template<ERPipeType... PipelineTypes, typename... Handlers>
    inline constexpr void pipeline_type_dispatcher(ERPipeType type, Handlers && ... handlers) {

        static_assert(sizeof...(PipelineTypes) == sizeof...(Handlers));
    
        if(!_dispatch_impl<PipelineTypes...>(type, std::forward<Handlers>(handlers)...)) {
            throw std::runtime_error("Unknown type");
        }
    }

    enum class ERPipeParamType {
        None,
        Texture,
        UBO_Static,            // 1 buffer for parameter asset
        UBO_Dynamic,           // 1 buffer for frame in flight and parameter asset
        UBO_Entity_Static,     // 1 buffer for entity  (e.g ubo model)
        UBO_Entity_Dynamic,    // 1 buffer for frame in flight and entity
        UBO_Component_Static,  // 1 buffer for component subindex
        UBO_Component_Dynamic, // 1 buffer for frame and component subindex (e.g effects controlled by a component)
    };

    inline constexpr bool IsUBOParamType(ERPipeParamType param_type) {
        return param_type == ERPipeParamType::UBO_Static        ||
            param_type == ERPipeParamType::UBO_Dynamic          ||
            param_type == ERPipeParamType::UBO_Entity_Dynamic   ||
            param_type == ERPipeParamType::UBO_Entity_Static    ||
            param_type == ERPipeParamType::UBO_Component_Static ||
            param_type == ERPipeParamType::UBO_Component_Dynamic;
    }

    struct RPipeParamDescriptor {
        std::uint8_t set{1};
        std::uint8_t binding{0};
        
        ERPipeParamType type{ERPipeParamType::None};
        EShaderStageFlag stage_flags{EShaderStageFlag::None};
    
        /** @brief Only for UBOs, total UBO size. */
        std::uint8_t size{0};
    };

    using RPipeParamDescriptorsLayout = std::array<RPipeParamDescriptor, 16>;

    // constexpr const std::uint8_t MODEL_UBO_BINDING{0};
    // constexpr const std::uint8_t MODEL_UBO_DESC_SET{2};
    constexpr wct::render::RPipeParamDescriptor ModelParamDescriptor_Static() {
        return wct::render::RPipeParamDescriptor {
            .set=CommonBindings::MODEL_SET,
            .binding=CommonBindings::MODEL_UBO,
            .type=wct::render::ERPipeParamType::UBO_Entity_Static,
            .stage_flags=wct::render::EShaderStageFlag::Vertex,
            .size=sizeof(wct::render::ModelUBO)
        };
    }

    constexpr wct::render::RPipeParamDescriptor ModelParamDescriptor_Dynamic() {
        return wct::render::RPipeParamDescriptor {
            .set=CommonBindings::MODEL_SET,
            .binding=CommonBindings::MODEL_UBO,
            .type=wct::render::ERPipeParamType::UBO_Entity_Dynamic,
            .stage_flags=wct::render::EShaderStageFlag::Vertex,
            .size=sizeof(wct::render::ModelUBO)
        };
    }



    struct ShaderInfo {
        EShaderStageFlag type{EShaderStageFlag::None};
        TName<128> file{};
        TName<16> entry{};
    };

    using ShaderList = std::array<ShaderInfo, MAX_PIPELINE_SHADERS>;

   // Pipeline Parameters Structs
   // ---------------------------

    using UBORef = std::span<std::uint8_t>;
    using UBOData = std::vector<std::uint8_t>;

    template<typename T>
    inline UBOData ToUBOData(T const & value) {
        auto begin = reinterpret_cast<std::uint8_t const *>(&value);
        return UBOData(begin, begin + sizeof(T));
    }

    /** Render Pipeline Param Ubo Struct */
    struct RPipeParamUbo {

        std::uint8_t set{1};
        std::uint8_t binding{0};

        std::variant<UBORef, UBOData> data{};

        /**
         * Offset is applied to locate the memory address on VkBuffer when mapped.
         * It allows to update a segment of the entire buffer.
         * // TODO activate offset
         */
        // std::size_t offset{0};
    };

    template<typename T>
    struct TRPipeParam {
        std::uint8_t set{1};
        std::uint8_t binding{0};
        T value{};
    };

    using RPipeParamAsset = TRPipeParam<wcr::wid::WAssetId>;

    using RPipeParamList_WAssetId = std::vector<RPipeParamAsset>;
    using RPipeParamList_Ubo = std::vector<RPipeParamUbo>;

    template<CCallable<void, const RPipeParamDescriptor &> TFn>
    inline void ForEach(const wct::render::RPipeParamDescriptorsLayout & in_lst, TFn && in_fn) {
        for(const auto& param: in_lst) {
            if (param.type==ERPipeParamType::None)
                break;

            std::forward<TFn>(in_fn)(param);
        }
    }

    template<CCallable<void, const ShaderInfo &> TFn>
    inline void ForEach(const ShaderList & in_lst, TFn && in_fn) {
        for(const auto& shd : in_lst) {
            if (shd.type == EShaderStageFlag::None)
                break;

            std::forward<TFn>(in_fn)(shd);
        }
    }

    // Pipeline Params assignment

    struct RPipeAssignment {
        wcr::wid::WAssetId pipeline{};
        wcr::wid::WAssetId params{};
    };

    inline constexpr std::uint8_t MAX_PIPELINE_ASSINGMENTS{16};

    template<std::uint8_t Max=MAX_PIPELINE_ASSINGMENTS>
    using RPipeAssignments = std::array<RPipeAssignment, Max>;

   // Lighting
   // --------

    struct PointLight {
        glm::vec3 color{0.5, 0.5, 0.5};
        float radius{10.f};
        glm::vec3 position{0.f, 0.f, 0.f};
        float _padding[1];
    };

    static_assert(sizeof(PointLight) == 32, "Size must match Vulkan layout");
    static_assert(offsetof(PointLight, color) == 0, "Color at offset 0");
    static_assert(offsetof(PointLight, radius) == 12, "Radius at offset 12");
    static_assert(offsetof(PointLight, position) == 16, "Intensity at offset 16");

    struct DirectionalLight {
        glm::vec3 color{0.5, 0.5, 0.5};
        float _padding_1;
        glm::vec3 direction{0.f, 0.f, 0.f};
        float _padding_2;
    };

    static_assert(sizeof(DirectionalLight)==32, "Size must match Vulkan layout");
    static_assert(offsetof(DirectionalLight, color)==0, "Color at offset 0");
    static_assert(offsetof(DirectionalLight, direction)==16, "Direction at offset 16");

    struct AmbientLight {
        glm::vec3 color{0.5, 0.5, 0.5};
        float _padding;
    };

    static_assert(sizeof(AmbientLight)==16, "Size must match Vulkan layout");
    static_assert(offsetof(AmbientLight, color)==0, "Color at offset 0");

    struct LightingUBO {
        static constexpr std::uint32_t MAX_POINT_LIGHTS{64};
        static constexpr std::uint32_t MAX_DIRECTIONAL_LIGHTS{16};

        std::array<PointLight, MAX_POINT_LIGHTS> point_lights;
        std::array<DirectionalLight, MAX_DIRECTIONAL_LIGHTS> directional_lights;
        AmbientLight ambient_light{};

        std::uint32_t point_lights_count{0};
        std::uint32_t directional_lights_count{0};
        float _padding[2];

        // shadow map data
        glm::mat4 shadow_map_projection{};
        glm::mat4 shadow_map_view_matrix{};
    };

    static_assert(sizeof(PointLight) * LightingUBO::MAX_POINT_LIGHTS +
                  sizeof(DirectionalLight) * LightingUBO::MAX_DIRECTIONAL_LIGHTS +
                  sizeof(AmbientLight) +
                  16  +
                  sizeof(glm::mat4) +
                  sizeof(glm::mat4) == sizeof(LightingUBO), "Size must match a Vulkan layout");

    

}
