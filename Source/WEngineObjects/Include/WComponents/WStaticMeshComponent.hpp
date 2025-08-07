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
        static_mesh_struct_.static_mesh_asset = in_id;
    }

    WAssetId StaticMeshAsset() const {
        return static_mesh_struct_.static_mesh_asset;
    }

    void RenderPipelineAsset(const WAssetId & in_id) {
        static_mesh_struct_.render_pipeline_asset = in_id;
    }

    WAssetId RenderPipelineAsset() const {
        return static_mesh_struct_.render_pipeline_asset;
    }

    void RenderPipelineParametersAsset(const WAssetId & in_id) {
        static_mesh_struct_.render_pipeline_parameters_asset = in_id;
    }

    WAssetId RenderPipelineParametersAsset() const {
        return static_mesh_struct_.render_pipeline_parameters_asset;
    }

    WStaticMeshStruct & StaticMeshStruct() {
        return static_mesh_struct_;
    }

    const WStaticMeshStruct & StaticMeshStruct() const {
        return static_mesh_struct_;
    }

private:

    WStaticMeshStruct static_mesh_struct_{};
  
};


