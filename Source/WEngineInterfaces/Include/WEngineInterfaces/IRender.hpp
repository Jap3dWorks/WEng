#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WStructs/WRenderStructs.hpp"

#include <vector>

class WRenderPipelineAsset;
class WTextureAsset;
class WStaticMeshAsset;
struct WTransformStruct;
struct WCameraStruct;

struct GLFWwindow;

class IRender {
public:

    virtual ~IRender()=default;

    virtual void Initialize()=0;

    virtual void Draw()=0;

    virtual void Destroy()=0;

    virtual void WaitIdle() const=0;

    // Pipeline Section
    // ----------------

    /**
     * @brief Create a render pipeline in graphics card for in_pipeline_asset parameter.
     * The render pipeline create should share the same WId
     * than the WRenderPipelineAsset * parameter.
     */
    virtual void CreateRenderPipeline(
        WRenderPipelineAsset * in_pipeline_asset
        )=0;

    /**
     * @brief Create a pipeline binding to component.
     * Pipeline binding shares the same WId than the component.
     */
    virtual void CreatePipelineBinding(
        const WEntityComponentId & component_id,
        const WAssetId & pipeline_id,
        const WAssetId & in_mesh_id,
        const WRenderPipelineParametersStruct & in_parameters
        )=0;

    /**
     * @brief Remove all Render Pipelines.
     */
    virtual void ClearPipelines() =0;

    /**
     * @brief Delete the created render pipeline and its binidngs with WId in_id.
     */
    virtual void DeleteRenderPipeline(const WAssetId & in_id)=0;

    /**
     * @brief Delete the create render pipeline binding.
     */
    virtual void DeletePipelineBinding(const WEntityComponentId & in_id)=0;

    // Resources Section
    // -----------------

    /**
     * @brief Register a texture, later you can load it using the asset id.
     */
    virtual void RegisterTexture(WTextureAsset & in_texture_asset)=0;

    virtual void UnregisterTexture(const WAssetId & in_id)=0;

    /**
     * @brief Load the registered texture asset with id in_id.
     */
    virtual void LoadTexture(const WAssetId & in_id)=0;

    virtual void UnloadTexture(const WAssetId & in_id)=0;

    virtual void RegisterStaticMesh(WStaticMeshAsset & in_static_mesh_asset)=0;

    virtual void UnregisterStaticMesh(const WAssetId & in_id)=0;

    virtual void LoadStaticMesh(const WAssetId & in_id)=0;

    virtual void UnloadStaticMesh(const WAssetId & in_id)=0;

    virtual void UpdateUboCamera(
        const WCameraStruct & camera_struct,
        const WTransformStruct & transform_struct
        )=0;

    virtual void UpdateUboModelDynamic(
        const WEntityComponentId & in_component_id,
        const WTransformStruct & in_transform_struct
        )=0;

    virtual void UpdateUboModelStatic(
        const WEntityComponentId & in_component_id,
        const WTransformStruct & in_transform_struct
        )=0;

    /**
     * @brief Unload all render resources.
     */
    virtual void UnloadAllResources() = 0;

    /**
     * Returns current render window.
     */
    virtual void Window(GLFWwindow * in_window)=0;

    virtual void Rescale(const std::uint32_t & in_width, const std::uint32_t & in_height)=0;

};
