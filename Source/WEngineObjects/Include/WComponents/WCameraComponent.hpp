#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/WCore.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WStructs/WComponentStructs.hpp"

#include <glm/glm.hpp>

#include "WCameraComponent.WEngine.hpp"


class WENGINEOBJECTS_API WCameraComponent : public WComponent {
    WOBJECT_BODY(WCameraComponent)
        
public:

    WCameraPropertiesStruct & GetCameraStruct() { return camera_struct_; };

    const WCameraPropertiesStruct & GetCameraStruct() const { return camera_struct_; }

    void SetRenderPipelineAssignment(const WSubIdxId & in_id,
                                const WAssetId & in_pipeline_id,
                                const WAssetId & in_param_id) {
        render_pipelines_[in_id.GetId()].pipeline=in_pipeline_id;
        render_pipelines_[in_id.GetId()].params=in_param_id;
    }

    void SetRenderPipelineAssignment(const WSubIdxId & in_id,
                                     const WRenderPipelineAssignmentStruct & in_assignment) {
        render_pipelines_[in_id.GetId()] = in_assignment;
    }

    WRenderPipelineAssignmentStruct GetRenderPipelineAssignment(const WSubIdxId & in_id=0) const {
        return render_pipelines_[in_id.GetId()];
    }

    template<CCallable<void,
                       const WCameraComponent *,
                       const WSubIdxId &,
                       const WRenderPipelineAssignmentStruct &> TFn>
    void ForEachAssignment(TFn && in_fn) const {
        std::uint8_t idx = 0;
        for(const auto & assignment : render_pipelines_) {
            if(!assignment.pipeline.IsValid()) break;

            std::forward<TFn>(in_fn)(this, WSubIdxId(idx), assignment);
            idx++;
        }
    }

    void RenderId(const WRenderId & in_id) {
        render_id_ = in_id;
    }

    WRenderId RenderId() const {
        return render_id_;
    }

private:

    WCameraPropertiesStruct camera_struct_{};

    WRenderId render_id_{0};

    WRenderPipelineAssignments render_pipelines_{};
    
};
