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

    virtual void Destroy()=0;

    /**
     * @brief Create a render pipeline in graphics card for in_pipeline_asset parameter.
     * The render pipeline create should share the same WId
     * than the WRenderPipelineAsset * parameter.
     */
    virtual void CreateRenderPipeline(
        WRenderPipelineAsset * in_pipeline_asset
        )=0;

    virtual void CreatePipelineBinding(
        const WId & component_id,
        const WId & pipeline_id,
        const WId & in_mesh_id,
        const WRenderPipelineParametersStruct & in_parameters
        )=0;

    /**
     * @brief Remove current Render Pipelines.
     */
    virtual void ClearPipelines() =0;

    virtual void RegisterTexture(WTextureAsset & in_texture_asset)=0;

    virtual void UnregisterTexture(const WId & in_id)=0;

    virtual void LoadTexture(const WId & in_id)=0;

    virtual void UnloadTexture(const WId & in_id)=0;

    virtual void RegisterStaticMesh(WStaticMeshAsset & in_static_mesh_asset)=0;

    virtual void UnregisterStaticMesh(const WId & in_id)=0;

    virtual void LoadStaticMesh(const WId & in_id)=0;

    virtual void UnloadStaticMesh(const WId & in_id)=0;

    /**
     * @brief Unload all render resources.
     */
    virtual void UnloadCurrentResources() = 0;

    virtual void WaitIdle() const=0;

    virtual GLFWwindow * Window() const=0;

};
