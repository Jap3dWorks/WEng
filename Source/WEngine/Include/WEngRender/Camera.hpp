#pragma once

#include "WAssets/RenderPipelineParams.hpp"
#include "WAssets/RenderPipeline.hpp"
#include "WComponents/Transform.hpp"
#include "WComponents/Camera.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WCore/WId.hpp"
#include "WInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WAssets/Level.hpp"
#include "WRender/WRender.hpp"

namespace wng::render::camera {

    inline wcr::wid::WEntityId GetRenderCamera(was::Level * in_level) {
        // Temporal solution, only one camera.
        // other cameras with RenderId() > 1 could render into textures.
        wcr::wid::WEntityId camera_entt{};
        in_level->ForEachComponent<wcm::Camera>(
            [&camera_entt](wcm::Camera * _cam){
                if (!camera_entt && _cam->Get_render_id().IsValid()) {
                    camera_entt = _cam->Get_entity_id();
                }
            }
            );

        return camera_entt;
    }

    inline void InitializePostprocess(
        IRender * in_render,
        was::Level * in_level,
        const WAssetDb & in_asset_db
        ) {
        
        wcr::wid::WEntityId camera_entt = GetRenderCamera(in_level);

        if(camera_entt.IsValid()) {
            TSparseSet<wcr::wid::WAssetId> cam_render_pipelines;
            cam_render_pipelines.Reserve(WENG_MAX_ASSET_IDS);

            in_level->GetComponent<wcm::Camera>(camera_entt).ForEachPostprocessAssignment(
                [&cam_render_pipelines](
                     const wcm::Camera * _cmp,
                     const wcr::wid::WSubIdxId & _idx,
                     const auto & _assgn) {
                    cam_render_pipelines.Insert(_assgn.pipeline.GetId(), _assgn.pipeline);
                }
                );

            for (const wcr::wid::WAssetId & id : cam_render_pipelines) {
                auto & render_pipeline = in_asset_db.Get<was::RenderPipeline>(id);
                in_render->CreateRenderPipeline(render_pipeline); // TODO Use the data struct
            }

            wcm::Camera & comp = in_level
                ->GetComponent<wcm::Camera>(camera_entt);
            
            comp.ForEachPostprocessAssignment(
                [&in_level,
                 &in_render,
                 &in_asset_db](
                    const wcm::Camera * _cmp,
                    const wcr::wid::WSubIdxId & _idx,
                    const auto & _assgn
                    ) {

                    wcr::wid::WEntityComponentId ecid = {
                        in_level->Get_asset_id(),
                        _cmp->Get_entity_id(),
                        in_level->GetComponentTypeId<wcm::Camera>(),
                        _idx
                    };

                    in_render->CreatePipelineBindingSet(
                        in_level->Get_asset_id().GetId(),
                        wcr::wid::WEngId::FromEntityComponent(ecid),
                        wcr::wid::nullid,
                        in_asset_db.Get<was::RenderPipeline>(_assgn.pipeline),
                        in_asset_db.Get<was::RenderPipelineParams>(_assgn.params)
                        );
                }
                );
        }

        in_render->RefreshPipelines();
    }

    inline void UpdateRenderCamera(
        IRender * render,
        was::Level * level
        ) {

        wcr::wid::WEntityId entity = GetRenderCamera(level);

        wct::render::RenderSize rsize = render->RenderSize();

        render->UpdateUboCamera(
            wrd::render::ToUBOCameraStruct(
                level->GetComponent<wcm::Camera>(entity),
                level->GetComponent<wcm::Transform>(entity),
                (float) rsize.width / (float) rsize.height
                )
            );
    }

    

}
