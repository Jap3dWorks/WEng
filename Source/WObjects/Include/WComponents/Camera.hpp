#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/WCore.hpp"
#include "WObjects/WComponent.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "wcm::Camera.WEng.hpp"

namespace wcm {

    class WOBJECTS_API Camera : public WComponent {

        WOBJECT_BODY;

    public:

        using PostprocessAssignments =
            wct::render::RPipeAssignments<WENG_MAX_ASSET_IDS>;

    public:

        /**
         * field of view in radiants
         */
        WPROPERTY(float, field_of_view, glm::pi<float>() * 0.25);
        WPROPERTY(float, near_clipping, 0.01f);
        WPROPERTY(float, far_clipping, 100.f);

        // Focus Distance
        WPROPERTY(float, focus_distance, 1.5f);

        WPROPERTY(wcr::wid::WRenderId, render_id, 0);

        WPROPERTY(PostprocessAssignments, postprocess_pipelines, );

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
                           const wcm::Camera *,
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

        WNODISCARD
        glm::vec3 GetPointOfInterest(glm::vec3 point, glm::vec3 direction) const {
            return point + glm::normalize(direction) * focus_distance;
        }

        WNODISCARD static inline constexpr
        glm::vec3 GetCameraFrontVector(glm::mat4 transform_matrix) {
            // camera fron vector is -z.
            return -transform_matrix[2];
        }

    };

}
