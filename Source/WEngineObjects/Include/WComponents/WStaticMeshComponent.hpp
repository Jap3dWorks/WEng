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

public:

    WPROPERTY(WAssetId, static_mesh_asset,);
    WPROPERTY(WStaticMeshAsset::PipelineAssignments, pipeline_assignments,);

public:

    /**
     * @brief assigns a static mesh asset and its render pipeline assignments.
     */
    void SetStaticMeshAsset(WStaticMeshAsset const & in_static_mesh) {
        Set_static_mesh_asset(in_static_mesh.Get_asset_id());
        Set_pipeline_assignments(in_static_mesh.Get_pipeline_assignments());
    }

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


