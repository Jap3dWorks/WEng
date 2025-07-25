#pragma once

#include "WCore/WCore.hpp"

#include <cstdint>
#include <array>

#include <glm/glm.hpp>

/**
 * Uniform buffer data structure
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
    size_t binding;
    T value;
};

using WRPFloatParameterList = std::array<TRPParameter<float>, 8>;
using WRPWIdParameterList = std::array<TRPParameter<WId>, 8>;

struct WRenderPipelineParametersStruct {
    WRPFloatParameterList float_parameters;
    WRPWIdParameterList texture_assets;
};

