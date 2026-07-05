#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/WCore.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include <glm/glm.hpp>

#include "WCameraComponent.WEngine.hpp"


class WENGINEOBJECTS_API WCameraComponent : public WComponent {

    WOBJECT_BODY;

public:

    using WPostprocessAssignments =
        wct::render::WPipelineAssignments<WENG_MAX_ASSET_IDS>;

public:

    WPROPERTY(glm::vec3, point, 0);
    WPROPERTY(float, angle_of_view, 45.f);
    WPROPERTY(float, near_clipping, 0.01f);
    WPROPERTY(float, far_clipping, 100.f);

    WPROPERTY(WRenderId, render_id, 0);

    WPROPERTY(WPostprocessAssignments, postprocess_pipelines, );

public:    

    void SetPostprocessAssignment(const WSubIdxId & in_id,
                                  const WAssetId & in_pipeline_id,
                                  const WAssetId & in_param_id) {
        postprocess_pipelines[in_id.GetId()].pipeline=in_pipeline_id;
        postprocess_pipelines[in_id.GetId()].params=in_param_id;
    }

    wct::render::WPipelineAssignment GetPostprocessAssignment(const WSubIdxId & in_id=0) const {
        return postprocess_pipelines[in_id.GetId()];
    }

    template<CCallable<void,
                       const WCameraComponent *,
                       const WSubIdxId &,
                       const wct::render::WPipelineAssignment &> TFn>
    void ForEachPostprocessAssignment(TFn && in_fn) const {
        std::uint8_t idx = 0;
        for(const auto & assignment : postprocess_pipelines) {
            if(!assignment.pipeline.IsValid()) break;

            std::forward<TFn>(in_fn)(this, WSubIdxId(idx), assignment);
            idx++;
        }
    }


};
