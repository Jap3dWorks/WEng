#include "WSystems/WSystems.hpp"
#include "WSystems/WSystemsRegister.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WLevel/WLevel.hpp"
#include "WUtils/WRenderLevelUtils.hpp"
#include "WEngine/WEngine.hpp"


START_DEFINE_WSYSTEM(System_PostUpdateRenderTransform)
    parameters;
    return true;
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
    return true;
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(System_InitRenderLevelResources)
    WRenderLevelUtils::InitializeResources(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );
    return true;
END_DEFINE_WSYSTEM()


START_DEFINE_WSYSTEM(System_EndRenderLevelResources)
    WRenderLevelUtils::ReleaseRenderResources(
        parameters.engine->Render().Ptr(),
        parameters.level,
        parameters.engine->AssetManager()
        );
    return true;
END_DEFINE_WSYSTEM()
