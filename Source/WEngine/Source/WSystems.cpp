#include "WSystems/WSystems.hpp"
#include "WSystems/WSystemsRegister.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WLevel/WLevel.hpp"
#include "WUtils/WRenderLevelUtils.hpp"
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

            parameters.engine->Render()->UpdateCamera(
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
    WAssetId mapping, frontaction, backaction, leftaction, rightaction;

    WEntityId cid;
    WCameraComponent * camcomponent = parameters.level->GetFirstComponent<WCameraComponent>(cid);

    parameters.engine->AssetManager().ForEach<WInputMappingAsset>([&mapping](WInputMappingAsset * a){
        mapping = a->WID();
    });

    // TODO Get assets by Name
    parameters.engine->AssetManager().ForEach<WActionAsset>([&frontaction,
                                                             &backaction,
                                                             &leftaction,
                                                             &rightaction] (WActionAsset * a) {
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
    });

    parameters.engine->InputMappingRegister().PutInputMapping(mapping);

    parameters.engine->InputMappingRegister().BindAction(
        frontaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            WTransformStruct & transform =
                _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

            // TODO: Get Direction.
            transform.position[0] = transform.position[0] + 0.1;

            WLOG("[InputMapping] InputMapping Trigger!");
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        backaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            WTransformStruct & transform =
                _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

            // TODO: Get Direction.
            transform.position[0] = transform.position[0] - 0.1;
            WLOG("[InputMapping] InputMapping Trigger!");
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        leftaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            WTransformStruct & transform =
                _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

            // TODO: Get Direction.
            transform.position[0] = transform.position[1] + 0.1;
            WLOG("[InputMapping] InputMapping Trigger!");
        }
        );

    parameters.engine->InputMappingRegister().BindAction(
        frontaction,
        [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {

            WTransformStruct & transform =
                _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

            // TODO: Get Direction.
            transform.position[0] = transform.position[1] - 0.1;
            WLOG("[InputMapping] InputMapping Trigger!");
        }
        );

END_DEFINE_WSYSTEM()
