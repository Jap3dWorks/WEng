#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WComponent.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WStructs/WComponentStructs.hpp"

#include "WStaticMeshComponent.WEngine.hpp"

class WENGINEOBJECTS_API WStaticMeshComponent : public WComponent {

    WOBJECT_BODY(WStaticMeshComponent)

public:

    void StaticMeshAsset(const WAssetId & in_id) {
        static_mesh_asset_ = in_id;
    }

    WAssetId GetStaticMeshAsset() const {
        return static_mesh_asset_;
    }

    void SetRenderPipelineAsset(const WAssetId & in_asset_id, const WAssIdxId & in_id=0) {
        render_pipeline_assets_[in_id.GetId()] = in_asset_id;
    }

    WAssetId GetRenderPipelineAsset(const WAssIdxId & in_id=0) const {
        return render_pipeline_assets_[in_id.GetId()];
    }

    void SetRenderPipelineParametersAsset(const WAssetId & in_asset_id, const WAssIdxId & in_id=0) {
        render_pipeline_params_assets_[in_id.GetId()] = in_asset_id;
    }

    WAssetId GetRenderPipelineParametersAsset(const WAssIdxId & in_id=0) const {
        return render_pipeline_params_assets_[in_id.GetId()];
    }

private:

    WAssetId static_mesh_asset_{};
    std::array<WAssetId, WENG_MAX_ASSET_IDS> render_pipeline_assets_{};
    std::array<WAssetId, WENG_MAX_ASSET_IDS> render_pipeline_params_assets_{};
  
};


