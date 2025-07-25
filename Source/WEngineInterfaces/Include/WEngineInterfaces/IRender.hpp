#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WEngineInterfaces/IRenderResources.hpp"

#include <vector>

class WRenderPipelineAsset;

struct GLFWwindow;

class IRender {
public:

    virtual ~IRender()=default;

    virtual void Draw()=0;

    /**
     * @brief Create a render pipeline in graphics card for in_pipeline_asset parameter.
     * The render pipeline create should share the same WId
     * than the WRenderPipelineAsset * parameter.
     */
    virtual void CreateRenderPipeline(
        WRenderPipelineAsset * in_pipeline_asset
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
