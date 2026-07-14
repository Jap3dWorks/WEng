#include "WSystems/WSystems.hpp"
#include "WRender/WRender.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
// #include "WSystems/WSystemsRegister.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WComponents/WMovementComponent.hpp"
#include "WComponents/WCameraInputComponent.hpp"
#include "WEngRender/WEngRender.hpp"
#include "WUtils/WMath.hpp"
#include "WEngine/WEngine.hpp"
#include "WEngine/WEngineDefaults.hpp"

#include <glm/geometric.hpp>


START_DEFINE_WSYSTEM(SystemInit_InitializeTransformsMatrix)
    parameters.engine->LevelInfo().level.ForEachComponent<WTransformComponent>(
        [&parameters](WTransformComponent * _transform) {
            // WTransformStruct & ts = _transform->TransformStruct();
            
            _transform
                ->Set_transform_matrix(
                    WMath::ToMat4(
                        _transform->Get_position(),
                        _transform->Get_rotation(),
                        _transform->Get_rotation_order(),
                        _transform->Get_scale()
                        ));
            
        }
        );
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemInit_RenderLevelResources)
    wng::render::InitializeResources(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemInit_CameraInput)

    wid::WEntityId camid;
    parameters.level->GetFirstComponent<WCameraComponent>(camid);

    auto & asset_manager = parameters.engine->AssetManager();

    wid::WAssetId mapping = asset_manager
        .Get(weng::defaults::CAMERA_MAPPING_ASSET_PATH)
        ->Get_asset_id();

    wid::WAssetId frontaction = asset_manager
        .Get(weng::defaults::FRONT_ACTION_ASSET_PATH)
        ->Get_asset_id();

    wid::WAssetId backaction = asset_manager
        .Get(weng::defaults::BACK_ACTION_ASSET_PATH)
        ->Get_asset_id();

    wid::WAssetId leftaction = asset_manager
        .Get(weng::defaults::LEFT_ACTION_ASSET_PATH)
        ->Get_asset_id();

    wid::WAssetId rightaction = asset_manager
        .Get(weng::defaults::RIGHT_ACTION_ASSET_PATH)
        ->Get_asset_id();

    wid::WAssetId mousemovement = asset_manager
        .Get(weng::defaults::MOUSE_MOVEMENT_ACTION_ASSET_PATH)
        ->Get_asset_id();

    parameters.engine->InputMappingRegister().PutInputMapping(mapping);

    parameters.engine->InputMappingRegister().BindAction(
        frontaction,
        [camid](const WInputValuesStruct & _v, WActionAsset const * _a, WEngine * _e) {

            auto & ic = _e->LevelInfo().level.GetComponent<WCameraInputComponent>(camid);

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
        [camid](const WInputValuesStruct & _v, WActionAsset const * _a, WEngine * _e) {

            auto & ic = _e->LevelInfo().level.GetComponent<WCameraInputComponent>(camid);

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
        [camid](const WInputValuesStruct & _v, WActionAsset const * _a, WEngine * _e) {

            auto & ic = _e->LevelInfo().level.GetComponent<WCameraInputComponent>(camid);

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
        [camid](const WInputValuesStruct & _v, WActionAsset const * _a, WEngine * _e) {

            auto & ic = _e->LevelInfo().level.GetComponent<WCameraInputComponent>(camid);

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
        [camid](const WInputValuesStruct & _v, WActionAsset const * _a, WEngine * _e) {

            auto * transform_component = &_e->LevelInfo()
                .level.GetComponent<WTransformComponent>(camid);
            
            // WTransformStruct & t = _e->LevelInfo()
            //     .level.GetComponent<WTransformComponent>(camid)
            //     .TransformStruct();

            auto rot = transform_component->Get_rotation();
            rot.x = _v.direction.y * -0.001;
            rot.y = _v.direction.x * -0.001;

            transform_component->Set_rotation(rot);

            transform_component->Set_transform_matrix(
                WMath::ToMat4(
                    transform_component->Get_position(),
                    transform_component->Get_rotation(),
                    transform_component->Get_rotation_order(),
                    transform_component->Get_scale()
                    )
                );

            // t.transform_matrix = WMath::ToMat4(t.position, t.rotation, t.rotation_order, t.scale);
        }
        );
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemPre_UpdateMovement)
    parameters.level->ForEachComponent<WMovementComponent>(
        [&parameters](WMovementComponent * mc){
            WTransformComponent & tc = parameters.level->
                GetComponent<WTransformComponent>(mc->Get_entity_id());

            float amag = std::min(glm::length(mc->Get_acceleration()), mc->Get_max_acceleration());

            if (amag > 0.0000001) {
                mc->Set_acceleration(glm::normalize(mc->Get_acceleration()) * amag);
            }
            else {
                mc->Set_acceleration(glm::vec3{0});
            }

            mc->Set_velocity(
                mc->Get_velocity() + mc->Get_acceleration() * (float)parameters.engine->EngineCycle().DeltaTime
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
                WMath::ToMat4(
                    tc.Get_position(),
                    tc.Get_rotation(),
                    tc.Get_rotation_order(),
                    tc.Get_scale())
                );

            // ts.transform_matrix = WMath::ToMat4(
            //     ts.position,
            //     ts.rotation,
            //     ts.rotation_order,
            //     ts.scale);
        }
        );
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemPre_CameraInputMovement)
    wid::WEntityId id;
    auto & ic = parameters.level->GetFirstComponent<WCameraInputComponent>(id);
    auto & tc = parameters.level->GetComponent<WTransformComponent>(id);
    auto & mc = parameters.level->GetComponent<WMovementComponent>(id);

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
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemPost_UpdateRenderCamera)
    parameters.level->ForEachComponent<WCameraComponent> (
        [&parameters] (WCameraComponent * cam) {

            WTransformComponent & ts =
                parameters.level->GetComponent<WTransformComponent>(
                    cam->Get_entity_id()
                    );

            wct::render::WRenderSize rsize = parameters.engine->Render()->RenderSize();

            parameters.engine->Render()->UpdateUboCamera(
                wrd::render::ToUBOCameraStruct(
                    *cam,
                    ts,
                    (float) rsize.width / (float) rsize.height
                    )
                );
        });
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemEnd_RenderLevelResources)
    wng::render::ReleaseRenderResources(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );
END_DEFINE_WSYSTEM()

