#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/WCore.hpp"
#include "WEngineObjects/WComponent.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "WCameraComponent.WEngine.hpp"


class WENGINEOBJECTS_API WCameraComponent : public WComponent {

    WOBJECT_BODY;

public:

    using WPostprocessAssignments =
        wct::render::RPipeAssignments<WENG_MAX_ASSET_IDS>;

public:

    /**
     * field of view in radiants
     */
    WPROPERTY(float, field_of_view, glm::pi<float>() * 0.25);
    WPROPERTY(float, near_clipping, 0.01f);
    WPROPERTY(float, far_clipping, 100.f);
    // Focus Distance
    WPROPERTY(float, focus_distance, 0)

    WPROPERTY(wcr::wid::WRenderId, render_id, 0);

    WPROPERTY(WPostprocessAssignments, postprocess_pipelines, );

public:    

    void SetPostprocessAssignment(const wcr::wid::WSubIdxId & in_id,
                                  const wcr::wid::WAssetId & in_pipeline_id,
                                  const wcr::wid::WAssetId & in_param_id) {
        postprocess_pipelines[in_id.GetId()].pipeline=in_pipeline_id;
        postprocess_pipelines[in_id.GetId()].params=in_param_id;
    }

    wct::render::RPipeAssignment GetPostprocessAssignment(const wcr::wid::WSubIdxId & in_id=0) const {
        return postprocess_pipelines[in_id.GetId()];
    }

    template<CCallable<void,
                       const WCameraComponent *,
                       const wcr::wid::WSubIdxId &,
                       const wct::render::RPipeAssignment &> TFn>
    void ForEachPostprocessAssignment(TFn && in_fn) const {
        std::uint8_t idx = 0;
        for(const auto & assignment : postprocess_pipelines) {
            if(!assignment.pipeline.IsValid()) break;

            std::forward<TFn>(in_fn)(this, wcr::wid::WSubIdxId(idx), assignment);
            idx++;
        }
    }


};
