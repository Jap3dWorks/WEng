#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WStructs/WRenderStructs.hpp"

#include <vector>

class WRenderPipelineAsset;
// class WTextureAsset;
// class WStaticMeshAsset;
struct WTextureStruct;
struct WMeshStruct;
struct WTransformStruct;
struct WCameraPropertiesStruct;


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
        WRenderPipelineAsset * in_pipeline_asset //TODO: avoid asset
        )=0;

    /**
     * @brief Create a pipeline binding to component.
     * Pipeline binding shares the same WId than the component.
     */
    virtual void CreatePipelineBinding(
        const WEntityComponentId & component_id,
        const WAssetId & pipeline_id,
        const WAssetIndexId & in_mesh_id,
        const WRenderPipelineParametersStruct & in_parameters  // TODO Check
        )=0;

    /**
     * @brief Delete the created render pipeline and its binidngs with WId in_id.
     */
    virtual void DeleteRenderPipeline(const WAssetId & in_id)=0;

    /**
     * @brief Delete the create render pipeline binding.
     */
    virtual void DeletePipelineBinding(const WEntityComponentId & in_id)=0;

    /** Refresh things like postprocess order */
    virtual void RefreshPipelines()=0;

    /**
     * @brief Remove all Render Pipelines.
     */
    virtual void ClearPipelines() =0;

    // Resources Section
    // -----------------

    /**
     * @brief Load the registered texture asset with id in_id.
     */
    virtual void LoadTexture(const WAssetId & in_id,
                             const WTextureStruct & in_texture)=0;

    virtual void UnloadTexture(const WAssetId & in_id)=0;

    virtual void LoadStaticMesh(const WAssetIndexId & in_id,
                                const WMeshStruct & in_mesh)=0;

    virtual void UnloadStaticMesh(const WAssetIndexId & in_id)=0;

    virtual void UpdateUboCamera(
        const WUBOCameraStruct & in_ubo
        )=0;

    /** @brief Current frame, but other frames in flight will not be updated. */
    virtual void UpdateUboBindingDynamic(const WEntityComponentId & in_id,
                                         const void * in_data,
                                         const std::size_t & in_size)=0;

    /** @brief Updates current frame and other frames in flight. */
    virtual void UpdateUboBindingStatic(const WEntityComponentId & in_id,
                                        const void * in_data,
                                        const std::size_t & in_size) =0;

    /**
     * @brief Unload all render resources.
     */
    virtual void UnloadAllResources() = 0;

    // Window
    // ------

    /**
     * Returns current render window.
     */
    virtual void Window(GLFWwindow * in_window)=0;

    virtual void Rescale(const std::uint32_t & in_width, const std::uint32_t & in_height)=0;

    // Render Data
    // -----------

    virtual WRenderSize RenderSize() const =0;

};
