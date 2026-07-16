#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WAssets/WRenderPipelineParametersAsset.hpp"

#include <vector>
#include <span>

class WRenderPipelineAsset;

// class WTextureAsset;
// class WStaticMeshAsset;
// namespace wct::texture { struct WTexture; }

namespace wct::geometry {struct WMesh;}

namespace wdw { class WWindow; }

struct WTransformStruct;

class WTextureAsset; 

struct GLFWwindow;

class IRender {
public:

    // Rule of Five (explicit defaults for clarity)
    IRender() = default;
    IRender(const IRender&) = default;
    IRender(IRender&&) noexcept = default;
    IRender& operator=(const IRender&) = default;
    IRender& operator=(IRender&&) noexcept = default;
    virtual ~IRender() = default;

    virtual void Draw()=0;

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
        const wid::WEntityComponentId & component_id,
        const wid::WAssetId & pipeline_id,
        const wid::WTypeAssetIndexId & in_mesh_id,
        const WRenderPipelineParametersAsset & in_parameters
        )=0;

    /**
     * @brief Delete the created render pipeline and its binidngs with WId in_id.
     */
    virtual void DeleteRenderPipeline(const wid::WAssetId & in_id)=0;

    /**
     * @brief Delete the create render pipeline binding.
     */
    virtual void DeletePipelineBinding(const wid::WEntityComponentId & in_id)=0;

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
    virtual void LoadTexture(const wid::WAssetId & in_id,
                             const WTextureAsset & in_texture)=0;

    virtual void UnloadTexture(const wid::WAssetId & in_id)=0;

    virtual void LoadStaticMesh(const wid::WTypeAssetIndexId & in_id,
                                const wct::geometry::WMesh & in_mesh)=0;

    virtual void UnloadStaticMesh(const wid::WTypeAssetIndexId & in_id)=0;

    virtual void UpdateUboCamera(
        const wct::render::CameraUBO & in_ubo
        )=0;

    /**
     * @brief Updates only for current frame in flight.
     *        Storage inside ubo_write will be consumed in the current call.
     */
    virtual void UpdateParameterDynamic(const wid::WEntityComponentId & in_id,
                                        const wct::render::RPipeParamUbo & ubo_write)=0;

    /**
     * @brief Updates for all frames in flight.
     *        Storage inside ubo_write will be in the current call.
     */
    virtual void UpdateParameterStatic(const wid::WEntityComponentId & in_id,
                                       const wct::render::RPipeParamUbo & ubo_write)=0;

    /**
     * @brief Unload all render resources.
     */
    virtual void UnloadAllResources()=0;

    // Window
    // ------

    /**
     * @brief Sets the render window.
     */
    virtual void SetWindow(wdw::WWindow * in_window)=0;

    // Render Data
    // -----------

    virtual wct::render::RenderSize RenderSize() const =0;
    virtual void Rescale(const std::uint32_t & in_width, const std::uint32_t & in_height)=0;

    // Lighting
    // --------

    /**
     * @brief Updates each point light inside in_ids list.
     *        This method can update selective lights in the render data.
     *        Each point light can be located by its wid::WEntityComponentId.
     */
    virtual void UpdatePointLights(
        std::span<wid::WEntityComponentId> in_ids,
        std::span<wct::render::PointLight> in_point_lights_structs
        )=0;

    /**
     * @brief Initialize lighting data.
     *        Previous lighting data is removed.
     */
    virtual void InitializeLights(
        std::span<wid::WEntityComponentId> in_pl_ids,
        std::span<wct::render::PointLight> in_point_lights,
        std::span<wid::WEntityComponentId> in_dl_ids,
        std::span<wct::render::DirectionalLight> in_directional_lights,
        const wct::render::AmbientLight & in_ambient_light
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
        std::span<wid::WEntityComponentId> in_ids,
        std::span<wct::render::DirectionalLight> in_directional_light_structs
        )=0;

    /**
     * @brief Updates ambient light.
     *        Only one ambient light is suported.
     */
    virtual void UpdateAmbientLight(
        const wct::render::AmbientLight & in_ambient_light
        )=0;

};
