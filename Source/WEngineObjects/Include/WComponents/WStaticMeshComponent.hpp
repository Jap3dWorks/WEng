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

    void SetRenderPipelineAssignment(const WSubIdxId & in_id,
                                const WAssetId & in_pipeline_id,
                                const WAssetId & in_param_id) {
        render_pipelines_[in_id.GetId()].pipeline=in_pipeline_id;
        render_pipelines_[in_id.GetId()].params=in_param_id;
    }

    WRenderPipelineAssignmentStruct GetRenderPipelineAssignment(const WSubIdxId & in_id=0) const {
        return render_pipelines_[in_id.GetId()];
    }

private:

    WAssetId static_mesh_asset_{};

    WRenderPipelineAssignments render_pipelines_{};
  
};


