#pragma once

#include "WCore/WCore.hpp"

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
    std::uint32_t  height{0};
};

enum class EShaderType : uint8_t
{
    None,
    Vertex,
    Fragment,
    // Geometry,
    Compute,
    // TessellationControl,
    // TessellationEvaluation
};

enum class EPipelineType : uint8_t
{
    Graphics,       // Default Opaque
    Transparency,   // Alpha Blending
    Postprocess,     // Camera shader pipelines
    Compute,        // GPU
    RayTracing     // Ray Tracing
};

struct WShaderStruct {
    EShaderType type{EShaderType::None};
    char file[128]{""};
    char entry[16]{"main"};
};

using WShaderList = std::array<WShaderStruct, WENG_MAX_PIPELINE_SHADERS>;

struct WRenderPipelineStruct {
    EPipelineType type {EPipelineType::Graphics};
    WShaderList shaders{};

    uint8_t shaders_count{0};
};

template<typename T>
struct TRPParameter {
    uint16_t binding;
    T value;
};

using WRPParameterList_Float = std::array<TRPParameter<float>, 8>;
using WRPParameterList_WAssetId = std::array<TRPParameter<WAssetId>, 8>;

struct WRenderPipelineParametersStruct {
    WRPParameterList_Float float_parameters{};
    uint8_t float_parameters_count{0};
    WRPParameterList_WAssetId texture_assets{};
    uint8_t texture_assets_count{0};
};

