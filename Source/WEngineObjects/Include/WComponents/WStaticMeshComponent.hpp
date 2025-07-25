#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WComponent.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WAssets/WRenderPipelineParametersAsset.hpp"

#include "WStaticMeshComponent.WEngine.hpp"

class WENGINEOBJECTS_API WStaticMeshComponent : public WComponent {

    WOBJECT_BODY(WStaticMeshComponent)

public:

    WId StaticMeshAsset() noexcept {
        return static_mesh_asset_id_;
    }

    void StaticMeshAsset(const WStaticMeshAsset & static_mesh_asset) noexcept {
        static_mesh_asset_id_ = static_mesh_asset.WID();
    }

    WId RenderPipelineAsset() noexcept {
        return render_pipeline_asset_id_;
    }
    
    void RenderPipelineAsset(const WRenderPipelineAsset & render_pipeline_asset) noexcept {
        render_pipeline_asset_id_ = render_pipeline_asset.WID();
    }

    WId RenderPipelineParametersAsset() noexcept {
        return render_pipeline_parameters_asset_id_;
    }

    void RenderPipelineParametersAsset(
        const WRenderPipelineParametersAsset & render_pipeline_parameters_asset
        ) noexcept {
        render_pipeline_parameters_asset_id_ = render_pipeline_parameters_asset.WID();
    }

private:

    WId static_mesh_asset_id_;

    WId render_pipeline_asset_id_{};

    WId render_pipeline_parameters_asset_id_{};
  
};


