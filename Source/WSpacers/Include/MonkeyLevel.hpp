#pragma once

#include "WAssets/InputMapping.hpp"
#include "WAssets/RenderPipelineParams.hpp"
#include "WComponents/Light/Point.hpp"
#include "WComponents/CameraInput.hpp"
#include "WComponents/Movement.hpp"
#include "WEngine/WEngineDefaults.hpp"
#include "WLog.hpp"
#include "WCore/WCore.hpp"
#include "WImporterRegister/WImporterRegister.hpp"
#include "WImporter/WImporterTexture.hpp"
#include "WImporter/WImporterObj.hpp"

#include "WAssets/StaticMesh.hpp"
#include "WAssets/Texture.hpp"
#include "WAssets/RenderPipeline.hpp"
#include "WAssets/Level.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WAssets/RenderPipeline.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WComponents/Transform.hpp"
#include "WComponents/StaticMesh.hpp"
#include "WComponents/Camera.hpp"
#include "WComponents/Light/Ambient.hpp"
#include "WCoreTypes/WMathStructs.hpp"
#include "WCoreTypes/WRenderTypes.hpp"


namespace spacers::monkey {
    
    struct ModelAssets {
        wcr::wid::WAssetId static_mesh;
        wcr::wid::WAssetId pipeline_asset;
        wcr::wid::WAssetId param_asset;
        wcr::wid::WEntityId entity;
    };

    inline bool LoadVikingRoom(WEngine & engine, ModelAssets & out_model)
    {
        // Import Viking Room
        wim::importer::WImporterObj obj_importer =
            engine.ImportersRegister().GetImporter<wim::importer::WImporterObj>();

        std::vector<wcr::wid::WAssetId> geo_ids =
            obj_importer.Import(
                engine.AssetManager(),
                "Content/Assets/Models/viking_room.obj", 
                "/Content/Assets/viking_room:viking_room"
                );

        out_model.static_mesh = geo_ids[0];

        wim::importer::WImportTexture texture_importer =
            engine.ImportersRegister().GetImporter<wim::importer::WImportTexture>();

        std::vector<wcr::wid::WAssetId> tex_ids = texture_importer.Import(
            engine.AssetManager(),
            "Content/Assets/Textures/viking_room.png", 
            "/Content/Assets/viking_texture:viking_texture"
            );

        WAsset * null_texture = engine.AssetManager()
            .Get(weng::defaults::NULL_TEXTURE_ASSET_PATH);

        WAsset * null_normal = engine.AssetManager()
            .Get(weng::defaults::NULL_NORMAL_TEXTURE_ASSET_PATH);

        was::RenderPipeline * pipeline_asset = engine.AssetManager()
            .Get<was::RenderPipeline>(weng::defaults::PBR_PIPELINE_ASSET_PATH);

        out_model.pipeline_asset = pipeline_asset->Get_asset_id();

        // Create Pipeline Parameter Asset

        wcr::wid::WAssetId paramid = engine.AssetManager()
            .Create<was::RenderPipelineParams>("/Content/Assets/Params/ParamA:ParamA");

        out_model.param_asset = paramid;

        auto & param_asset = engine.AssetManager().Get<was::RenderPipelineParams>(paramid);
        param_asset.Set_texture_list(
            {
                {
                    .binding=wct::render::PBRBindings::ALBEDO_TEXTURE,
                    .value=tex_ids[0]
                },
                {
                    .binding=wct::render::PBRBindings::EMISSION_TEXTURE,
                    .value=null_texture->Get_asset_id()
                },
                {
                    .binding=wct::render::PBRBindings::NORMAL_TEXTURE,
                    .value=null_normal->Get_asset_id()
                },
                {
                    .binding=wct::render::PBRBindings::ORM_TEXTURE,
                    .value=null_texture->Get_asset_id()
                }
            }
            );

        return true;
    }

    inline bool SetPostprocessPipelines(WEngine & engine, wcm::Camera & camera) {
        wcr::wid::WAssetId pipid =
            engine.AssetManager().Create<was::RenderPipeline>("/Content/Assets/PPDebug:PPDebug");

        was::RenderPipeline & pipeline_asset =
            engine.AssetManager().Get<was::RenderPipeline>(pipid);

        pipeline_asset.Set_pipeline_type(wct::render::ERPipeType::Postprocess);

        auto shader_list = pipeline_asset.Get_shader_list();
        shader_list[0].type=wct::render::EShaderStageFlag::Vertex;
        shader_list[0].file = "/Content/Shaders/WRender_rpassdebug.pprcess.spv";
        shader_list[0].entry = "vsMain";

        shader_list[1].type=wct::render::EShaderStageFlag::Fragment;
        shader_list[1].file = "/Content/Shaders/WRender_rpassdebug.pprcess.spv";
        shader_list[1].entry = "fsMain";

        pipeline_asset.Set_shader_list(shader_list);

        auto descriptors = pipeline_asset.Get_descriptor_list();

        descriptors[0].binding = 0;
        descriptors[0].type = wct::render::ERPipeParamType::UBO_Entity_Dynamic;
        descriptors[0].stage_flags = wct::render::EShaderStageFlag::Vertex;
        descriptors[0].size = sizeof(wct::render::PostprocessUBO);

        pipeline_asset.Set_descriptor_list(descriptors);

        wcr::wid::WAssetId paramid =
            engine.AssetManager().Create<was::RenderPipelineParams>(
                "/Content/Assets/PPDebugParam:PPDebugParam"
                );

        // camera.SetPostprocessAssignment(0, pipid, paramid);

        return true;
    }

    inline bool LoadMonkey(WEngine & engine, ModelAssets & out_model, const wcr::wid::WAssetId & in_render_pipeline) {
        wim::importer::WImporterObj obj_importer =
            engine.ImportersRegister()
            .GetImporter<wim::importer::WImporterObj>();

        std::vector<wcr::wid::WAssetId> geo_ids =
            obj_importer.Import(
                engine.AssetManager(),
                "Content/Assets/Models/monkey.obj", 
                "/Content/Assets/monkey:monkey"
                );

        out_model.static_mesh = geo_ids[0];

        wim::importer::WImportTexture tex_importer = engine.ImportersRegister()
            .GetImporter<wim::importer::WImportTexture>();

        std::vector<wcr::wid::WAssetId> tex_ids = tex_importer.Import(engine.AssetManager(),
                                                            "Content/Assets/Textures/orange.png",
                                                            "/Content/Assets/orange:orange");

        out_model.pipeline_asset = in_render_pipeline;

        wcr::wid::WAssetId paramid = engine.AssetManager()
            .Create<was::RenderPipelineParams>("/Content/Assets/ParamB:ParamB");

        out_model.param_asset = paramid;

        WAsset * null_texture = engine.AssetManager()
            .Get(weng::defaults::NULL_TEXTURE_ASSET_PATH);

        WAsset * null_normal = engine.AssetManager()
            .Get(weng::defaults::NULL_NORMAL_TEXTURE_ASSET_PATH);

        auto & param_asset = engine.AssetManager().Get<was::RenderPipelineParams>(paramid);

        param_asset.Set_texture_list(
            {
                {
                    .binding=wct::render::PBRBindings::ALBEDO_TEXTURE,
                    .value=tex_ids[0]
                },
                {
                    .binding=wct::render::PBRBindings::EMISSION_TEXTURE,
                    .value=null_texture->Get_asset_id()
                },
                {
                    .binding=wct::render::PBRBindings::NORMAL_TEXTURE,
                    .value=null_normal->Get_asset_id()
                },
                {
                    .binding=wct::render::PBRBindings::ORM_TEXTURE,
                    .value=null_texture->Get_asset_id()
                }
            }
            );

        return true;
    }

    inline wcr::wid::WAssetId ConfigLevel(WEngine & in_engine,
                         const ModelAssets & in_viking_room,
                         const ModelAssets & in_monkey_dt
        ) {

        wcr::wid::WAssetId levelid = in_engine.AssetManager().Create<was::Level>(
            "/Content/Level/Level01:Level01"
            );

        was::Level & level = in_engine.AssetManager().Get<was::Level>(levelid);

        in_engine.AddInitSystem(levelid, "SystemInit_CameraInput");
        in_engine.AddPreSystem(levelid, "SystemPre_CameraInputMovement");
        in_engine.AddPreSystem(levelid, "SystemPre_UpdateMovement");

        // Camera

        wcr::wid::WEntityId cid = level.CreateEntity<WEntity>();
        level.CreateComponent<wcm::Transform>(cid);
        level.CreateComponent<wcm::Camera>(cid);
        level.CreateComponent<wcm::Movement>(cid);     // parametrized movement
        level.CreateComponent<wcm::CameraInput>(cid);  // User input

        wcm::Camera & cameracomp = level.GetComponent<wcm::Camera>(cid);
        cameracomp.Set_render_id(1); // The renderable camera
        wcm::Transform * tcmp = &level.GetComponent<wcm::Transform>(cid);

        tcmp->Set_rotation({0.0f, 0.0f, 0.0f});
        tcmp->Set_position({0.0, 0.0f, .5f});

        // postprocess
        SetPostprocessPipelines(in_engine, cameracomp);

        // Models
        // Viking Room
        wcr::wid::WEntityId eid = level.CreateEntity<WEntity>();
        level.CreateComponent<wcm::Transform>(eid);
        wcm::Transform & ctcmp = level.GetComponent<wcm::Transform>(eid);
        ctcmp.Set_rotation_order(ERotationOrder::zxy);
        ctcmp.Set_position({0.0, 0.0, -2.f});
        ctcmp.Set_rotation({-3.1415 * 0.5, -3.1415 * 0.5, 0.f});

        level.CreateComponent<wcm::StaticMesh>(eid);    

        wcm::StaticMesh & smcomponent =
            level.GetComponent<wcm::StaticMesh>(eid);
        smcomponent.Set_static_mesh_asset(in_viking_room.static_mesh);
        smcomponent.SetPipelineAssignment(
            0, in_viking_room.pipeline_asset, in_viking_room.param_asset
            );

        // Monkey 1
        wcr::wid::WEntityId monkey_id = level.CreateEntity<WEntity>();
        level.CreateComponent<wcm::Transform>(monkey_id);
        auto * monkey_tc = &level.GetComponent<wcm::Transform>(monkey_id);
        monkey_tc->Set_rotation_order(ERotationOrder::zxy);
        monkey_tc->Set_position({0.0, 0.5, -2.0});
        monkey_tc->Set_scale(monkey_tc->Get_scale() * 0.15f);

        level.CreateComponent<wcm::StaticMesh>(monkey_id);

        auto & monkeysm = level.GetComponent<wcm::StaticMesh>(monkey_id);

        monkeysm.Set_static_mesh_asset(in_monkey_dt.static_mesh);
        monkeysm.SetPipelineAssignment(0,
                                       in_monkey_dt.pipeline_asset,
                                       in_monkey_dt.param_asset);

        // Monkey 2
        wcr::wid::WEntityId monkey2_id = level.CreateEntity<WEntity>();
        level.CreateComponent<wcm::Transform>(monkey2_id);
        monkey_tc = &level.GetComponent<wcm::Transform>(monkey2_id);
        monkey_tc->Set_rotation_order(ERotationOrder::zxy);
        monkey_tc->Set_position({0.2, 0.65, -2.0});
        monkey_tc->Set_scale(monkey_tc->Get_scale() * 0.25f);

        level.CreateComponent<wcm::StaticMesh>(monkey2_id);
        auto & monkey2sm = level.GetComponent<wcm::StaticMesh>(monkey2_id);

        monkey2sm.Set_static_mesh_asset(in_monkey_dt.static_mesh);
        monkey2sm.SetPipelineAssignment(0,
                                        in_monkey_dt.pipeline_asset,
                                        in_monkey_dt.param_asset);


        // Lights
        // ------
    
        wcr::wid::WEntityId point_light_1 = level.CreateEntity<WEntity>();
        level.CreateComponent<wcm::Transform>(point_light_1);
        level.CreateComponent<wcm::light::Point>(point_light_1);
        auto * transform_ptr = &level.GetComponent<wcm::Transform>(point_light_1);

        transform_ptr->Set_position({0.0, 1.1, -2.0});
        transform_ptr->Set_scale(transform_ptr->Get_scale() * 0.1f);

        wcm::light::Point * light_ptr =
            &level.GetComponent<wcm::light::Point>(point_light_1);

        light_ptr->Set_intensity(5.0);
        light_ptr->Set_radius(1.5);
        light_ptr->Set_active(true);

        wcr::wid::WEntityId ambient_light = level.CreateEntity<WEntity>();
        level.CreateComponent<wcm::light::Ambient>(ambient_light);

        auto * ambient_ptr = &level.GetComponent<wcm::light::Ambient>(ambient_light);

        ambient_ptr->Set_color({0.5, 0.5, 0.5});
        ambient_ptr->Set_intensity(0.25);
        ambient_ptr->Set_active(true);

        return levelid;

    }

    inline wcr::wid::WAssetId CreateMonkeyLevel(WEngine & in_engine) {
                std::vector<wct::render::RPipeAssignment> ppcsst_assignments;

        ModelAssets viking_room;
        ModelAssets monkey_1;
        ModelAssets monkey_2;

        WFLOG("[INFO] Create Assets.");

        LoadVikingRoom(in_engine, viking_room);
        LoadMonkey(in_engine, monkey_1, viking_room.pipeline_asset);

        WFLOG("[INFO] Setup Init Level.");
       
        return ConfigLevel(in_engine,
                           viking_room,
                           monkey_1);

    }

}


