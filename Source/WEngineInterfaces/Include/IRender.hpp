#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "IRenderResources.hpp"

#include <vector>

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

struct GLFWwindow;

class IRender {
public:

    virtual void Draw()=0;

    virtual WId CreateRenderPipeline(
        EPipelineType in_pipeline_type,
        const std::vector<std::string> & in_shader_files,
        const std::vector<EShaderType> & in_shader_types
        )=0;

    virtual void AddPipelineBinding(
        WId pipeline_id,
        WId in_mesh_id,
        const std::vector<WId> & in_textures,
        const std::vector<uint32_t> & in_textures_bindings
        )=0;

    virtual void Clear()=0;

    virtual TRef<IRenderResources> RenderResources()=0;

    virtual void WaitIdle() const=0;

    virtual GLFWwindow * Window() const=0;

};
