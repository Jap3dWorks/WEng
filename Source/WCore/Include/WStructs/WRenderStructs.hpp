#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WConcepts.hpp"

#include <cstdint>
#include <array>

#include <glm/glm.hpp>

#define WENG_MAX_PIPELINE_SHADERS 8

/**
 * @brief Uniform buffer data structure.
*/
struct WUBOGraphicsStruct
{
    glm::mat4 model;
    glm::mat3 normal_matrix;
    
    // TODO params here
};

/**
 * @brief parameters to use in a shader.
 */
struct WUBOPostprocessStruct
{
    glm::vec4 param1;
    glm::vec4 param2;
    glm::vec4 param3;
    glm::vec4 param4;
};

/**
 * @brief Camera Data to be used in the shader.
 */
struct WUBOCameraStruct {
    glm::mat4 proj{};
    glm::mat4 view{1};
    glm::vec3 pos{1};
    glm::vec3 rot{0};  // pitch, yaw, roll
    glm::vec3 scale{1};
    float angle_of_view{54.f};
    float near_clipping{0.01f};
    float far_clipping{100.f};
};

struct WRenderSize {
    std::uint32_t width{0};
    std::uint32_t height{0};
};

enum class EShaderStageFlag : std::uint8_t {
    None=0,
    Vertex=1,
    Fragment=2,
    // Geometry,
    Compute=4,
    // TessellationControl,
    // TessellationEvaluation
};

constexpr EShaderStageFlag operator|(const EShaderStageFlag & l, const EShaderStageFlag & r) noexcept {
    return static_cast<EShaderStageFlag>(static_cast<std::uint8_t>(l) | static_cast<std::uint8_t>(r));
}

constexpr EShaderStageFlag operator&(const EShaderStageFlag & l, const EShaderStageFlag & r) noexcept {
    return static_cast<EShaderStageFlag>(static_cast<std::uint8_t>(l) & static_cast<std::uint8_t>(r));
}

enum class EPipelineType : uint8_t
{
    Graphics,       // Default Opaque
    Transparency,   // Alpha Blending
    Postprocess,    // Camera shader pipelines
    Compute,        // GPU
    RayTracing,     // Ray Tracing
    Swap            // Used to write in the swap chain
};

enum class EPipeParamType {
    None,
    Texture,
    Ubo
};

struct WPipeParamDescriptorStruct {
    std::uint8_t binding{0};
    EPipeParamType type{EPipeParamType::None};
    EShaderStageFlag stage_flags{EShaderStageFlag::None};
    
    /** @brief UBO range, total UBO size. */
    std::size_t range{0};
};

using WPipeParamDescriptorList = std::array<WPipeParamDescriptorStruct, 16>;

struct WShaderStruct {
    EShaderStageFlag type{EShaderStageFlag::None};
    char file[128]{""};
    char entry[16]{"main"};
};

using WShaderList = std::array<WShaderStruct, WENG_MAX_PIPELINE_SHADERS>;

struct WRenderPipelineStruct {
    EPipelineType type {EPipelineType::Graphics};
    WShaderList shaders{};

    WPipeParamDescriptorList params_descriptor{};
};

// Pipeline Parameters Structs
// ---------------------------

struct WRPParamUboStruct {
    std::uint16_t binding{0};
    std::vector<char> databuffer{};
    std::size_t offset{0};
};

template<typename T>
struct TRPParamStruct {
    std::uint16_t binding{0};
    T value{};
};

using WRPParamAssetStruct = TRPParamStruct<WAssetId>;

using WRPParameterList_WAssetId = std::vector<WRPParamAssetStruct>;
using WRPParameterList_Ubo = std::vector<WRPParamUboStruct>;

struct WRenderPipelineParametersStruct {
    WRPParameterList_Ubo ubo_params{};
    WRPParameterList_WAssetId texture_params{};
};
 
namespace WRenderUtils {
    
    template<CCallable<void, const WPipeParamDescriptorStruct &> TFn>
    inline void ForEach(const WPipeParamDescriptorList & in_lst, TFn && in_fn) {
        for(const auto& param: in_lst) {
            if (param.type==EPipeParamType::None)
                break;

            std::forward<TFn>(in_fn)(param);
        }
    }

    template<CCallable<void, const WShaderStruct &> TFn>
    inline void ForEach(const WShaderList & in_lst, TFn && in_fn) {
        for(const auto& shd : in_lst) {
            if (shd.type == EShaderStageFlag::None)
                break;

            std::forward<TFn>(in_fn)(shd);
        }
    }

    constexpr const std::array<EShaderStageFlag, 4> SHADER_STAGE_FLAGS_LIST =
    {EShaderStageFlag::None,
     EShaderStageFlag::Vertex,
     EShaderStageFlag::Fragment,
     EShaderStageFlag::Compute};
    
}

