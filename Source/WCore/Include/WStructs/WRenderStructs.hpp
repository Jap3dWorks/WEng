#pragma once

#include "WCore/WCore.hpp"

#include <cstdint>
#include <array>

#include <glm/glm.hpp>

/**
 * Uniform buffer data structure, TODO, only transform
*/
struct WVkUBOStruct
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
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
    Graphics,       // Default
    Transparency,   // Alpha Blending
    Compute,        // GPGPU
    RayTracing,     // Ray Tracing
    Postprocess     // Camera shader pipelines
};

struct WShaderStruct {
    EShaderType type{EShaderType::None};
    char file[64]{""};
};

using WShaderList = std::array<WShaderStruct, 4>;

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

/**
 * @brief Camera Data to be used in the shader.
 */
struct WRenderCameraStruct {
    glm::mat4 proj{};
    glm::mat4 view{1};
    glm::vec3 pos{1};
    glm::vec3 rot{0};  // pitch, yaw, roll
    glm::vec3 scale{1};
    float angle_of_view{54.f};
    float near_clipping{0.01f};
    float far_clipping{100.f};
};


