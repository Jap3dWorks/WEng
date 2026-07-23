#pragma once

#include "WAssets/RenderPipelineParams.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WEngine/WEngine.hpp"
#include "WEngine/WEngineDefaults.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WImporter/WImporterTexture.hpp"

#include "WAssets/Level.hpp"
#include "WObjectDb/WAssetDb.hpp"

#include "WComponents/Transform.hpp"
#include "WComponents/StaticMesh.hpp"
#include "WComponents/Camera.hpp"
#include "WComponents/CameraInput.hpp"
#include "WComponents/Movement.hpp"
#include "WComponents/Light/Ambient.hpp"

namespace spacers::plane {
    
    struct LevelData {
        wcr::wid::WAssetId texture_id;
    };

    inline wct::geometry::WMesh Plane() {
        wct::geometry::WMesh result;
        result.vertices = {
            {
                {5.f, 0.f, 5.f},
                {0, 0},
                {},
                {0,1,0}
            },
            {
                {-5.f, 0.f, 5.f},
                {1,0},
                {},
                {0,1,0}
            },
            {
                {-5.f, 0.f, -5.f},
                {1,1},
                {},
                {0,1,0}
            },
            {
                {5.f, 0.f, -5.f},
                {0,1},
                {},
                {0,1,0}
            }
        };

        result.indices = {
            2, 1, 3,
            0, 3, 1
        };

        return result;
    }


    inline std::vector<wcr::wid::WAssetId> ImportAssets(WEngine & engine) {
        
        wim::importer::WImportTexture texture_importer =
            engine.ImportersRegister().GetImporter<wim::importer::WImportTexture>();

        return texture_importer.Import(
            engine.AssetManager(),
            "Content/Assets/Textures/viking_room.png", 
            "/Content/planelevel/"
            );
        
    }

    inline void PopulateMesh(WEngine & engine, was::Level * level, LevelData level_data) {
        was::StaticMesh smasset {};

        smasset.SetMesh(Plane(), 0);

        wcr::wid::WAssetId smid = engine.AssetManager().CreateFrom<was::StaticMesh>(
            "/Content/planelevel/smplane00:smplane00",
            smasset);

        wcr::wid::WEntityId entt = level->CreateEntity<WEntity>();
        level->CreateComponent<wcm::Transform>(entt);
        level->CreateComponent<wcm::StaticMesh>(entt);

        auto & smcmp = level->GetComponent<wcm::StaticMesh>(entt);

        smcmp.SetStaticMeshAsset(
            engine.AssetManager().Get<was::StaticMesh>(smid)
            );

        was::RenderPipelineParams param{};

        wct::render::UBOData ubo_data{};
        param.Set_ubo_list(
            {
                {
                    wct::render::PBRBindings::PBR_SCALAR_UBO,
                    wct::render::ToUBOData(&ubo_data)
                }  
            }
            );

        param.Set_texture_list(
            {
                {
                    wct::render::PBRBindings::ALBEDO_TEXTURE,
                    level_data.texture_id
                },
                {
                    wct::render::PBRBindings::EMISSION_TEXTURE,
                    engine.AssetManager().GetId(weng::defaults::NULL_RGBA_TEXTURE_ASSET_PATH)
                },
                {
                    wct::render::PBRBindings::NORMAL_TEXTURE,
                    engine.AssetManager().GetId(weng::defaults::NULL_NORMAL_TEXTURE_ASSET_PATH)
                },
                {
                    wct::render::PBRBindings::ORM_TEXTURE,
                    engine.AssetManager().GetId(weng::defaults::NULL_TEXTURE_ASSET_PATH)
                }
            }
            );

        smcmp.SetPipelineAssignment(
            0,
            engine.AssetManager().GetId(weng::defaults::PBR_PIPELINE_ASSET_PATH),
            engine.AssetManager().CreateFrom(
                "/Content/planelevel/param00:param00",
                param
                )
            );
    }


    inline void ConfigLevel(WEngine & engine, was::Level * level) {
        auto levelid = level->Get_asset_id();
        
        engine.AddInitSystem(levelid, "SystemInit_CameraInput");
        engine.AddPreSystem(levelid, "SystemPre_CameraInputMovement");
        engine.AddPreSystem(levelid, "SystemPre_UpdateMovement");

        // Camera

        wcr::wid::WEntityId cid = level->CreateEntity<WEntity>();
        level->CreateComponent<wcm::Transform>(cid);
        level->CreateComponent<wcm::Camera>(cid);
        level->CreateComponent<wcm::Movement>(cid);     // parametrized movement
        level->CreateComponent<wcm::CameraInput>(cid);  // User input

        wcm::Camera & cameracomp = level->GetComponent<wcm::Camera>(cid);
        cameracomp.Set_render_id(1); // The renderable camera
        wcm::Transform * tcmp = &level->GetComponent<wcm::Transform>(cid);

        tcmp->Set_rotation({0.0f, 0.0f, 0.0f});
        tcmp->Set_position({0.0, 0.0f, .5f});

        // postprocess

        // WAsset * debugshader = engine
        //     .AssetManager()
        //     .Get("/Content/Assets/PPDebug:PPDebug");

        // WAsset * debugparams = engine
        //     .AssetManager()
        //     .Get("/Content/Assets/PPDebugParam:PPDebugParam");

        // cameracomp.SetPostprocessAssignment(0,
        //                                     debugshader->Get_asset_id(),
        //                                     debugparams->Get_asset_id());
    }

    inline void SetupLighting(was::Level * level) {
        wcr::wid::WEntityId ambient_light = level->CreateEntity<WEntity>();
        level->CreateComponent<wcm::light::Ambient>(ambient_light);

        auto * ambient_ptr = &level
            ->GetComponent<wcm::light::Ambient>(ambient_light);

        ambient_ptr->Set_color({0.5, 0.5, 0.5});
        ambient_ptr->Set_intensity(1);
        ambient_ptr->Set_active(true);
    }

    inline wcr::wid::WAssetId CreateLevel(WEngine & in_engine) {
        wcr::wid::WAssetId level_id = in_engine.AssetManager().Create<was::Level>(
            "/Content/planelevel/level01:level01"
            );

        was::Level * level_asset = &in_engine.AssetManager()
            .Get<was::Level>(level_id);

        LevelData level_data;

        level_data.texture_id = ImportAssets(in_engine)[0];
        PopulateMesh(in_engine, level_asset, level_data);
        ConfigLevel(in_engine, level_asset);
        SetupLighting(level_asset);

        return level_id;

    }
}
