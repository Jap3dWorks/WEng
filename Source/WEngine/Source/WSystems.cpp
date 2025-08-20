#include "WSystems/WSystems.hpp"
#include "WSystems/WSystemsRegister.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WLevel/WLevel.hpp"
#include "WUtils/WRenderLevelUtils.hpp"
#include "WUtils/WMathUtils.hpp"
#include "WEngine/WEngine.hpp"


START_DEFINE_WSYSTEM(System_PostUpdateRenderTransform)
    parameters;
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(System_PostUpdateRenderCamera)
    parameters.engine->LevelInfo().level.ForEachComponent<WCameraComponent> (
        [&parameters] (WCameraComponent * cam) {

            WTransformComponent * ts =
                parameters.level->GetComponent<WTransformComponent>(
                    cam->EntityId()
                    );

            parameters.engine->Render()->UpdateUboCamera(
                cam->CameraStruct(),
                ts->TransformStruct()
                );
        });
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(System_InitRenderLevelResources)
    WRenderLevelUtils::InitializeResources(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(System_EndRenderLevelResources)
    WRenderLevelUtils::ReleaseRenderResources(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );

END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(System_InitCameraInput)
    WAssetId mapping, frontaction, backaction, leftaction, rightaction, mousemovement;

    WEntityId cid;
    WCameraComponent * camcomponent = parameters.level->GetFirstComponent<WCameraComponent>(cid);

    parameters.engine->AssetManager().ForEach<WInputMappingAsset>([&mapping](WInputMappingAsset * a){
        mapping = a->WID();
    });

    // TODO Get assets by Name, store in a GeneralTree?
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

    parameters.engine->InputMappingRegister().PutInputMapping(mapping);

    parameters.engine->InputMappingRegister().BindAction(
        frontaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            WTransformStruct & transform =
                _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

            // glm::vec3 d = transform.transform[2];
            // d *= 0.01;
            transform.position += glm::vec3(0.0, 0.001, 0.0);
            transform.transform[3] = {transform.position, 1};

            WLOG("[InputMapping] InputMapping Trigger!");
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        backaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            WTransformStruct & transform =
                _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

            glm::vec3 d = transform.transform[2];
            d *= 0.01;
            transform.position -= d;
            transform.transform[3] = {transform.position, 1};

            WLOG("[InputMapping] InputMapping Trigger!");
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        leftaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            WTransformStruct & transform =
                _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

            glm::vec3 d = transform.transform[0];
            d *= 0.01;
            transform.position += d;
            transform.transform[3] = {transform.position, 1};

            WLOG("[InputMapping] InputMapping Trigger!");
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        rightaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            WTransformStruct & transform =
                _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

            glm::vec3 d = transform.transform[0];
            d *= 0.01;
            transform.position -= d;
            transform.transform[3] = {transform.position, 1};

            WLOG("[InputMapping] InputMapping Trigger!");
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        mousemovement,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {
            WLOG("[InputMapping] Mouse Movement Trugger! {}, {}", _v.direction.x, _v.direction.y);
            
            WTransformStruct & t = _e->LevelInfo()
                .level.GetComponent<WTransformComponent>(cid)
                ->TransformStruct();

            t.rotation.x = _v.direction.y * 0.001;
            t.rotation.y = _v.direction.x * 0.001;

            t.transform = WMathUtils::ToMat4(t.position, t.rotation, t.rotation_order, t.scale);
        }
        );

END_DEFINE_WSYSTEM()
