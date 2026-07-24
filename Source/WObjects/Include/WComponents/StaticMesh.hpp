#pragma once

#include "WCore/WCore.hpp"

#include "WObjects/WComponent.hpp"
#include "WAssets/StaticMesh.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "wcm::StaticMesh.WEng.hpp"

namespace wcm {

    class WOBJECTS_API StaticMesh : public WComponent {

        WOBJECT_BODY;

    public:

        WPROPERTY(wcr::wid::WAssetId, static_mesh_asset,);
        WPROPERTY(was::StaticMesh::PipelineAssignments, pipeline_assignments,);

    public:

        /**
         * @brief assigns a static mesh asset and its render pipeline assignments.
         */
        void SetStaticMeshAsset(was::StaticMesh const & in_static_mesh) {
            Set_static_mesh_asset(in_static_mesh.Get_asset_id());
            Set_pipeline_assignments(in_static_mesh.Get_pipeline_assignments());
        }

        void SetPipelineAssignment(const wcr::wid::WSubIdxId & in_id,
                                   const wcr::wid::WAssetId & in_pipeline_id,
                                   const wcr::wid::WAssetId & in_param_id) {
            pipeline_assignments[in_id.GetId()].pipeline=in_pipeline_id;
            pipeline_assignments[in_id.GetId()].params=in_param_id;
        }

        wct::render::RPipeAssignment GetPipelineAssignment(const wcr::wid::WSubIdxId & in_id=0) const {
            return pipeline_assignments[in_id.GetId()];
        }
  
    };

}
