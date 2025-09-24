#include "WSystems/WSystems.hpp"
#include "WRenderUtils.hpp"
#include "WStructs/WEngineStructs.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WSystems/WSystemsRegister.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WComponents/WMovementComponent.hpp"
#include "WComponents/WCameraInputComponent.hpp"
#include "WLevel/WLevel.hpp"
#include "WUtils/WRenderLevelUtils.hpp"
#include "WUtils/WMathUtils.hpp"
#include "WEngine/WEngine.hpp"
#include <glm/geometric.hpp>


START_DEFINE_WSYSTEM(SystemInit_InitializeTransformsMatrix)
    parameters.engine->LevelInfo().level.ForEachComponent<WTransformComponent>(
        [&parameters](WTransformComponent * _transform) {
            WTransformStruct & ts = _transform->TransformStruct();
            ts.transform_matrix = WMathUtils::ToMat4(
                ts.position, ts.rotation, ts.rotation_order, ts.scale
                );
        }
        );
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemInit_RenderLevelResources)
    WRenderLevelUtils::InitializeResources(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemInit_CameraInput)
    WAssetId mapping, frontaction, backaction, leftaction, rightaction, mousemovement;

    WFLOG("[DEBUG] Find Camera Component.");
    WEntityId cid;
    WCameraComponent * camcomponent = parameters.level->GetFirstComponent<WCameraComponent>(cid);

    WFLOG("[DEBUG] Find Input Mapping.");
    parameters.engine->AssetManager().ForEach<WInputMappingAsset>([&mapping](WInputMappingAsset * a){
        mapping = a->WID();
    });

    WFLOG("[DEBUG] Find Actions.");
    // TODO: Get assets by Name, store in a GeneralTree?
    parameters.engine->AssetManager().ForEach<WActionAsset>([&frontaction,
                                                             &backaction,
                                                             &leftaction,
                                                             &rightaction,
                                                             &mousemovement] (WActionAsset * a) {
        std::string name(a->Name());

        if(name.contains("Front")) {
            frontaction = a->WID();
        }
        else if(name.contains("Back")) {
            backaction = a->WID();
        }
        else if (name.contains("Left")) {
            leftaction = a->WID();
        }
        else if(name.contains("Right")) {
            rightaction = a->WID();
        }
        else if(name.contains("MouseMovement")) {
            mousemovement = a->WID();
        }
    });

    WFLOG("[DEBUG] Put Input Mapping.");
    parameters.engine->InputMappingRegister().PutInputMapping(mapping);

    parameters.engine->InputMappingRegister().BindAction(
        frontaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            auto * ic = _e->LevelInfo().level.GetComponent<WCameraInputComponent>(cid);

            switch(_v.input.mode) {
            case EInputMode::Press:
                ic->Front(true);
                break;
            case EInputMode::Release:
                ic->Front(false);
                break;
            default:
                
            }
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        backaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            auto * ic = _e->LevelInfo().level.GetComponent<WCameraInputComponent>(cid);

            switch(_v.input.mode) {
            case EInputMode::Press:
                ic->Back(true);
                break;
            case EInputMode::Release:
                ic->Back(false);
                break;
            default:
                
            }
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        leftaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            auto * ic = _e->LevelInfo().level.GetComponent<WCameraInputComponent>(cid);

            switch(_v.input.mode) {
            case EInputMode::Press:
                ic->Left(true);
                break;
            case EInputMode::Release:
                ic->Left(false);
                break;
            default:
                
            }
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        rightaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            auto * ic = _e->LevelInfo().level.GetComponent<WCameraInputComponent>(cid);

            switch(_v.input.mode) {
            case EInputMode::Press:
                ic->Right(true);
                break;
            case EInputMode::Release:
                ic->Right(false);
                break;
            default:
                
            }
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        mousemovement,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {
            
            WTransformStruct & t = _e->LevelInfo()
                .level.GetComponent<WTransformComponent>(cid)
                ->TransformStruct();

            t.rotation.x = _v.direction.y * -0.001;
            t.rotation.y = _v.direction.x * -0.001;

            t.transform_matrix = WMathUtils::ToMat4(t.position, t.rotation, t.rotation_order, t.scale);
        }
        );
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemPre_UpdateMovement)
    parameters.level->ForEachComponent<WMovementComponent>(
        [&parameters](WMovementComponent * mc){
            WTransformComponent * tc = parameters.level->
                GetComponent<WTransformComponent>(mc->EntityId());

            float amag = std::min(glm::length(mc->Acceleration()), mc->MaxAcceleration());

            if (amag > 0.0000001) {
                mc->Acceleration(glm::normalize(mc->Acceleration()) * amag);
            }
            else {
                mc->Acceleration(glm::vec3{0});
            }

            mc->Velocity(
                mc->Velocity() + mc->Acceleration() * (float)parameters.engine->EngineCycle().DeltaTime
                );

            float vlength = glm::length(mc->Velocity());
            float vmag = std::min(vlength, mc->MaxVelocity());

            glm::vec3 current_direction{0.00001, 0.00001, 0.00001};
            if(vlength > 0.0000001) {
                current_direction = glm::normalize(mc->Velocity());
            }

            float drag = mc->Drag() * (float)parameters.engine->EngineCycle().DeltaTime;

            mc->Velocity(
                (current_direction * vmag) - (current_direction * vmag * drag)
                );

            WTransformStruct & ts = tc->TransformStruct();

            ts.position +=
                mc->Velocity() * (float)parameters.engine->EngineCycle().DeltaTime;

            ts.transform_matrix = WMathUtils::ToMat4(
                ts.position,
                ts.rotation,
                ts.rotation_order,
                ts.scale);
        }
        );
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemPre_CameraInputMovement)
    WEntityId id;
    auto * ic = parameters.level->GetFirstComponent<WCameraInputComponent>(id);
    auto * tc = parameters.level->GetComponent<WTransformComponent>(id);
    auto * mc = parameters.level->GetComponent<WMovementComponent>(id);

    glm::vec3 acc{0};

    if(ic->Front()) {
        acc -= glm::vec3(tc->TransformStruct().transform_matrix[2]);
    }
    if(ic->Back()) {
        acc += glm::vec3(tc->TransformStruct().transform_matrix[2]);
    }
    if(ic->Left()) {
        acc -= glm::vec3(tc->TransformStruct().transform_matrix[0]);
    }
    if(ic->Right()) {
        acc += glm::vec3(tc->TransformStruct().transform_matrix[0]);
    }

    mc->Acceleration(acc * 3.f);
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemPost_UpdateRenderCamera)
    parameters.level->ForEachComponent<WCameraComponent> (
        [&parameters] (WCameraComponent * cam) {

            WTransformComponent * ts =
                parameters.level->GetComponent<WTransformComponent>(
                    cam->EntityId()
                    );

            WRenderSize rsize = parameters.engine->Render()->RenderSize();

            parameters.engine->Render()->UpdateUboCamera(
                WRenderUtils::ToUBOCameraStruct(
                    cam->GetCameraStruct(),
                    ts->TransformStruct(),
                    (float) rsize.width / (float) rsize.height
                    )
                );
        });
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(SystemEnd_RenderLevelResources)
    WRenderLevelUtils::ReleaseRenderResources(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );
END_DEFINE_WSYSTEM()

