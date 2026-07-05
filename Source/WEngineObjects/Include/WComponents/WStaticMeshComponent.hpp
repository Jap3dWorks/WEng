#pragma once

#include "WCore/WCore.hpp"

#include "WCore/WCoreMacros.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "WStaticMeshComponent.WEngine.hpp"

class WENGINEOBJECTS_API WStaticMeshComponent : public WComponent {

    WOBJECT_BODY;

private:
    
    using SMeshAssignments = wct::render::WPipelineAssignments<WENG_MAX_ASSET_IDS>;


public:

    WPROPERTY(WAssetId, static_mesh_asset,);
    WPROPERTY(SMeshAssignments, pipeline_assignments,);

public:

    // void StaticMeshAsset(const WAssetId & in_id) {
    //     static_mesh_asset = in_id;
    // }

    // WAssetId GetStaticMeshAsset() const {
    //     return static_mesh_asset;
    // }

    void SetPipelineAssignment(const WSubIdxId & in_id,
                                     const WAssetId & in_pipeline_id,
                                     const WAssetId & in_param_id) {
        pipeline_assignments[in_id.GetId()].pipeline=in_pipeline_id;
        pipeline_assignments[in_id.GetId()].params=in_param_id;
    }

    wct::render::WPipelineAssignment GetPipelineAssignment(const WSubIdxId & in_id=0) const {
        return pipeline_assignments[in_id.GetId()];
    }
  
};


