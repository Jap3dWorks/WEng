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
    EShaderType type{};
    const char file[64];
};

struct WRenderPipelineStruct {
    EPipelineType type {};
    std::array<WShaderStruct, 4> shaders;
};

template<typename T>
struct TRPParameter {
    size_t binding;
    T value;
};

struct WRenderPipelineParametersStruct {
    std::array<TRPParameter<float>, 8> float_parameters;
    std::array<TRPParameter<WId>, 8> texture_assets;
};

