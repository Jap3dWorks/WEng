#include "WSystems/WSystems.hpp"
#include "WSystems/WSystemsRegister.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WLevel/WLevel.hpp"
#include "WEngine.hpp"


START_DEFINE_WSYSTEM(System_UpdateRenderTransform)
    parameters;
    return true;
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(System_UpdateRenderCamera)
    parameters.level->ForEachComponent<WCameraComponent> (
        [&parameters] (WCameraComponent * cam) {

            WTransformComponent * ts =
                parameters.level->GetComponent<WTransformComponent>(
                    cam->EntityId()
                    );

            // ts->TransformStruct().position.x =
            //     ts->TransformStruct().position.x + .0001f;

            parameters.engine->Render()->UpdateCamera(
                cam->CameraStruct(),
                ts->TransformStruct()
                );
        });

    return true;
END_DEFINE_WSYSTEM()
