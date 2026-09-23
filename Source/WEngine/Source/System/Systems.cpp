#include "WEngine/System/Systems.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include "WComponents/Camera.hpp"
#include "WComponents/Transform.hpp"
#include "WComponents/Movement.hpp"
#include "WComponents/CameraInput.hpp"
#include "WEngRender/Assets.hpp"
#include "WEngRender/Camera.hpp"
#include "WEngRender/Lights.hpp"
#include "WMath/LinAlgebra.hpp"
#include "WEngine/WEngine.hpp"
#include "WEngine/WEngineDefaults.hpp"

#include <glm/geometric.hpp>

bool weng::system::common::Init_InitializeTransformsMatrix(wsm::SystemParameters const & parameters) {
    
    parameters.engine->LevelInfo().level.ForEachComponent<wcm::Transform>(
        [&parameters](wcm::Transform * _transform) {
            _transform
                ->Set_transform_matrix(
                    wmath::lin_algbr::ToMat4(
                        _transform->Get_position(),
                        _transform->Get_rotation(),
                        _transform->Get_rotation_order(),
                        _transform->Get_scale()
                        ));
            
        }
        );

    return true;

}


bool weng::system::common::Init_RenderLevelResources(wsm::SystemParameters const & parameters) {
    weng::render::assets::InitializeRenderAssets(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );

    weng::render::camera::InitializePostprocess(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );

    weng::render::lights::InitializeLights(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );

	return true;
}


bool weng::system::common::Init_CameraInput(wsm::SystemParameters const & parameters) {
    wcr::wid::WEntityId camid;
    parameters.level->GetFirstComponent<wcm::Camera>(camid);

    auto & asset_manager = parameters.engine->AssetManager();

    wcr::wid::WAssetId mapping = asset_manager
        .Get(weng::defaults::asset::CAMERA_MAPPING_ASSET_PATH)
        ->Get_asset_id();

    wcr::wid::WAssetId frontaction = asset_manager
        .Get(weng::defaults::asset::FRONT_ACTION_ASSET_PATH)
        ->Get_asset_id();

    wcr::wid::WAssetId backaction = asset_manager
        .Get(weng::defaults::asset::BACK_ACTION_ASSET_PATH)
        ->Get_asset_id();

    wcr::wid::WAssetId leftaction = asset_manager
        .Get(weng::defaults::asset::LEFT_ACTION_ASSET_PATH)
        ->Get_asset_id();

    wcr::wid::WAssetId rightaction = asset_manager
        .Get(weng::defaults::asset::RIGHT_ACTION_ASSET_PATH)
        ->Get_asset_id();

    wcr::wid::WAssetId mousemovement = asset_manager
        .Get(weng::defaults::asset::MOUSE_MOVEMENT_ACTION_ASSET_PATH)
        ->Get_asset_id();

    parameters.engine->InputMappingRegister().PutInputMapping(mapping);

    parameters.engine->InputMappingRegister().BindAction(
        frontaction,
        [camid](const WInputValuesStruct & _v, was::Action const * _a, WEngine * _e) {

            auto & ic = _e->LevelInfo().level.GetComponent<wcm::CameraInput>(camid);

            switch(_v.input.mode) {
            case EInputMode::Press:
                ic.Set_front(true);
                break;
            case EInputMode::Release:
                ic.Set_front(false);
                break;
            default:
                
            }
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        backaction,
        [camid](const WInputValuesStruct & _v, was::Action const * _a, WEngine * _e) {

            auto & ic = _e->LevelInfo().level.GetComponent<wcm::CameraInput>(camid);

            switch(_v.input.mode) {
            case EInputMode::Press:
                ic.Set_back(true);
                break;
            case EInputMode::Release:
                ic.Set_back(false);
                break;
            default:
                
            }
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        leftaction,
        [camid](const WInputValuesStruct & _v, was::Action const * _a, WEngine * _e) {

            auto & ic = _e->LevelInfo().level.GetComponent<wcm::CameraInput>(camid);

            switch(_v.input.mode) {
            case EInputMode::Press:
                ic.Set_left(true);
                break;
            case EInputMode::Release:
                ic.Set_left(false);
                break;
            default:
                
            }
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        rightaction,
        [camid](const WInputValuesStruct & _v, was::Action const * _a, WEngine * _e) {

            auto & ic = _e->LevelInfo().level.GetComponent<wcm::CameraInput>(camid);

            switch(_v.input.mode) {
            case EInputMode::Press:
                ic.Set_right(true);
                break;
            case EInputMode::Release:
                ic.Set_right(false);
                break;
            default:
                
            }
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        mousemovement,
        [camid](const WInputValuesStruct & _v, was::Action const * _a, WEngine * _e) {

            auto * transform_component = &_e->LevelInfo()
                .level.GetComponent<wcm::Transform>(camid);
            
            // WTransformStruct & t = _e->LevelInfo()
            //     .level.GetComponent<wcm::Transform>(camid)
            //     .TransformStruct();

            auto rot = transform_component->Get_rotation();
            rot.x = _v.direction.y * -0.001;
            rot.y = _v.direction.x * -0.001;

            transform_component->Set_rotation(rot);

            transform_component->Set_transform_matrix(
                wmath::lin_algbr::ToMat4(
                    transform_component->Get_position(),
                    transform_component->Get_rotation(),
                    transform_component->Get_rotation_order(),
                    transform_component->Get_scale()
                    )
                );

            // t.transform_matrix = wmath::lin_algbr::ToMat4(t.position, t.rotation, t.rotation_order, t.scale);
        }
        );

	return true;
}


bool weng::system::common::Pre_UpdateMovement(wsm::SystemParameters const & parameters) {
    parameters.level->ForEachComponent<wcm::Movement>(
        [&parameters](wcm::Movement * mc){
            wcm::Transform & tc = parameters.level->
                GetComponent<wcm::Transform>(mc->Get_entity_id());

            float amag = std::min(glm::length(mc->Get_acceleration()), mc->Get_max_acceleration());

            if (amag > 0.0000001) {
                mc->Set_acceleration(glm::normalize(mc->Get_acceleration()) * amag);
            }
            else {
                mc->Set_acceleration(glm::vec3{0});
            }

            mc->Set_velocity(
                mc->Get_velocity() + mc->Get_acceleration() *
                (float)parameters.engine->EngineCycle().DeltaTime
                );

            float vlength = glm::length(mc->Get_velocity());
            float vmag = std::min(vlength, mc->Get_max_velocity());

            glm::vec3 current_direction{0.00001, 0.00001, 0.00001};
            if(vlength > 0.0000001) {
                current_direction = glm::normalize(mc->Get_velocity());
            }

            float drag = mc->Get_drag() * (float)parameters.engine->EngineCycle().DeltaTime;

            mc->Set_velocity(
                (current_direction * vmag) - (current_direction * vmag * drag)
                );

            // WTransformStruct & ts = tc.TransformStruct();

            tc.Set_position(
                tc.Get_position() +
                mc->Get_velocity() * (float)parameters.engine->EngineCycle().DeltaTime
                );

            // ts.position +=
            //     mc->Get_velocity() * (float)parameters.engine->EngineCycle().DeltaTime;

            tc.Set_transform_matrix(
                wmath::lin_algbr::ToMat4(
                    tc.Get_position(),
                    tc.Get_rotation(),
                    tc.Get_rotation_order(),
                    tc.Get_scale())
                );

            // ts.transform_matrix = wmath::lin_algbr::ToMat4(
            //     ts.position,
            //     ts.rotation,
            //     ts.rotation_order,
            //     ts.scale);
        }
        );

	return true;
}


bool weng::system::common::Pre_CameraInputMovement(wsm::SystemParameters const & parameters) {
    wcr::wid::WEntityId id;
    auto & ic = parameters.level->GetFirstComponent<wcm::CameraInput>(id);
    auto & tc = parameters.level->GetComponent<wcm::Transform>(id);
    auto & mc = parameters.level->GetComponent<wcm::Movement>(id);

    glm::vec3 acc{0};

    auto matrx = tc.Get_transform_matrix();

    if(ic.Get_front()) {
        acc -= glm::vec3(matrx[2]);
    }
    if(ic.Get_back()) {
        acc += glm::vec3(matrx[2]);
    }
    if(ic.Get_left()) {
        acc -= glm::vec3(matrx[0]);
    }
    if(ic.Get_right()) {
        acc += glm::vec3(matrx[0]);
    }

    mc.Set_acceleration(acc * 3.f);

	return true;
}


bool weng::system::common::Post_UpdateRenderCamera(wsm::SystemParameters const & parameters) {
    weng::render::camera::UpdateRenderCamera(
        &parameters.engine->Render().Get(),
        parameters.level
        );

	return true;
}


bool weng::system::common::Post_UpdateShadowMap(wsm::SystemParameters const & parameters) {
    weng::render::lights::UpdateShadowMap(
        &parameters.engine->Render().Get(),
        parameters.level
        );

	return true;
}


bool weng::system::common::End_RenderLevelResources(wsm::SystemParameters const & parameters) {

    weng::render::assets::ReleaseRenderResources(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );
    
    return true;
}

