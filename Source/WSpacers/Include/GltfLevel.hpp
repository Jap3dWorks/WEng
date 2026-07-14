#pragma once

#include "WEngine/WEngine.hpp"
#include "WEngine/WEngineDefaults.hpp"
#include "WString/WString.hpp"

#include "WAssets/Level.hpp"
#include "WImporter/WImporterGltf.hpp"
#include "WObjectDb/WAssetDb.hpp"

#include "WComponents/WTransformComponent.hpp"
#include "WComponents/WStaticMeshComponent.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WCameraInputComponent.hpp"
#include "WComponents/WMovementComponent.hpp"
#include "WComponents/Light/WAmbientLightComponent.hpp"


#include <algorithm>
#include <filesystem>

namespace spacers::gltflevel {

    inline void ConfigLevel(WEngine & engine, was::Level * level) {
        auto levelid = level->Get_asset_id();
        
        engine.AddInitSystem(levelid, "SystemInit_CameraInput");
        engine.AddPreSystem(levelid, "SystemPre_CameraInputMovement");
        engine.AddPreSystem(levelid, "SystemPre_UpdateMovement");

        // Camera

        wid::WEntityId cid = level->CreateEntity<WEntity>();
        level->CreateComponent<WTransformComponent>(cid);
        level->CreateComponent<WCameraComponent>(cid);
        level->CreateComponent<WMovementComponent>(cid);     // parametrized movement
        level->CreateComponent<WCameraInputComponent>(cid);  // User input

        WCameraComponent & cameracomp = level->GetComponent<WCameraComponent>(cid);
        cameracomp.Set_render_id(1); // The renderable camera
        WTransformComponent * tcmp = &level->GetComponent<WTransformComponent>(cid);

        tcmp->Set_rotation({0.0f, 0.0f, 0.0f});
        tcmp->Set_position({0.0, 0.0f, .5f});

        // postprocess

        WAsset * debugshader = engine
            .AssetManager()
            .Get("/Content/Assets/PPDebug:PPDebug");

        WAsset * debugparams = engine
            .AssetManager()
            .Get("/Content/Assets/PPDebugParam:PPDebugParam");

        cameracomp.SetPostprocessAssignment(0,
                                            debugshader->Get_asset_id(),
                                            debugparams->Get_asset_id());
    }

    inline void SetupLighting(was::Level * level) {
        wid::WEntityId ambient_light = level->CreateEntity<WEntity>();
        level->CreateComponent<wcm::light::WAmbientLightComponent>(ambient_light);

        auto * ambient_ptr = &level
            ->GetComponent<wcm::light::WAmbientLightComponent>(ambient_light);

        ambient_ptr->Set_color({0.5, 0.5, 0.5});
        ambient_ptr->Set_intensity(1);
        ambient_ptr->Set_active(true);
    }

    inline wid::WAssetId CreateLevel(WEngine & in_engine) {
        std::vector<wid::WAssetId> imported_assets = in_engine
            .ImportersRegister()
            .GetImporter<wim::importer::WImporterGltf>()
            .Import(
                in_engine.AssetManager(),
                std::filesystem::absolute("Content/Assets/Levels/ABeautifulGame.glb").string(),
                "/Content/Assets/BeautifulGame/"
                );

        wid::WAssetId level_id{wid::NULL_V};
        was::Level * level_asset{nullptr};

        for (auto & item : imported_assets) {
            if (in_engine.AssetManager().Get(item)->Class()->IsEqual(was::Level::StaticClass())) {
                level_id = item;
                level_asset = &in_engine
                    .AssetManager()
                    .Get<was::Level>(item);

                break;
            }
        }

        ConfigLevel(in_engine, level_asset);
        SetupLighting(level_asset);

        return level_id;

    }
}
