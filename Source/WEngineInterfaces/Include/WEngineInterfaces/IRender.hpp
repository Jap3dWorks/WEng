#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include <vector>
#include <span>

class WRenderPipelineAsset;
// class WTextureAsset;
// class WStaticMeshAsset;
namespace wct::texture { struct WTexture; }

struct WMeshStruct;
struct WTransformStruct;

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
        const WAssetIndexId & in_mesh_id,
        const wct::render::WRenderPipelineParameters & in_parameters  // TODO Check
        )=0;

    /**
     * @brief Delete the created render pipeline and its binidngs with WId in_id.
     */
    virtual void DeleteRenderPipeline(const WAssetId & in_id)=0;

    /**
     * @brief Delete the create render pipeline binding.
     */
    virtual void DeletePipelineBinding(const WEntityComponentId & in_id)=0;

    /** @brief Refresh things like postprocess order */
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
                             const wct::texture::WTexture & in_texture)=0;

    virtual void UnloadTexture(const WAssetId & in_id)=0;

    virtual void LoadStaticMesh(const WAssetIndexId & in_id,
                                const WMeshStruct & in_mesh)=0;

    virtual void UnloadStaticMesh(const WAssetIndexId & in_id)=0;

    virtual void UpdateUboCamera(
        const wct::render::WCameraUBO & in_ubo
        )=0;

    /**
     * @brief Updates only for current frame in flight.
     *        Storage inside ubo_write will be consumed in the current call.
     */
    virtual void UpdateParameterDynamic(const WEntityComponentId & in_id,
                                        const wct::render::WRPParamUbo & ubo_write)=0;

    /**
     * @brief Updates for all frames in flight.
     *        Storage inside ubo_write will be in the current call.
     */
    virtual void UpdateParameterStatic(const WEntityComponentId & in_id,
                                       const wct::render::WRPParamUbo & ubo_write)=0;

    /**
     * @brief Unload all render resources.
     */
    virtual void UnloadAllResources()=0;

    // Window
    // ------

    /**
     * @brief Returns current render window.
     */
    virtual void Window(GLFWwindow * in_window)=0;

    // Render Data
    // -----------

    virtual wct::render::WRenderSize RenderSize() const =0;
    virtual void RenderSize(const wct::render::WRenderSize & in_render_size)=0;
    virtual void Rescale(const std::uint32_t & in_width, const std::uint32_t & in_height)=0;

    // Lighting
    // --------

    /**
     * @brief Updates each point light inside in_ids list.
     *        This method can update selective lights in the render data.
     *        Each point light can be located by its WEntityComponentId.
     */
    virtual void UpdatePointLights(
        std::span<WEntityComponentId> in_ids,
        std::span<wct::render::WPointLight> in_point_lights_structs
        )=0;

    /**
     * @brief Initialize lighting data.
     *        Previous lighting data is removed.
     */
    virtual void InitializeLights(
        std::span<WEntityComponentId> in_pl_ids,
        std::span<wct::render::WPointLight> in_point_lights,
        std::span<WEntityComponentId> in_dl_ids,
        std::span<wct::render::WDirectionalLight> in_directional_lights,
        const wct::render::WAmbientLight & in_ambient_light
        )=0;

    /**
     * @brief Removes all render lighting data.
     */
    virtual void ClearLights()=0;

    /**
     * @brief Updates each directional light inside in_ids list.
     *        Same functionality than UpdatePointLights
     */
    virtual void UpdateDirectionalLights(
        std::span<WEntityComponentId> in_ids,
        std::span<wct::render::WDirectionalLight> in_directional_light_structs
        )=0;

    /**
     * @brief Updates ambient light.
     *        Only one ambient light is suported.
     */
    virtual void UpdateAmbientLight(
        const wct::render::WAmbientLight & in_ambient_light
        )=0;

};
