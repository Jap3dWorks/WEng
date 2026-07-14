#pragma once

#include "WAssets/WInputMappingAsset.hpp"
#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WComponents/Light/WPointLightComponent.hpp"
#include "WComponents/WCameraInputComponent.hpp"
#include "WComponents/WMovementComponent.hpp"
#include "WEngine/WEngineDefaults.hpp"
#include "WLog.hpp"
#include "WCore/WCore.hpp"
#include "WImporterRegister/WImporterRegister.hpp"
#include "WImporter/WImporterTexture.hpp"
#include "WImporter/WImporterObj.hpp"

#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WAssets/Level.hpp"
#include "WCoreTypes/WEngineStructs.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WComponents/WStaticMeshComponent.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WCameraInputComponent.hpp"
#include "WComponents/WMovementComponent.hpp"
#include "WComponents/Light/WAmbientLightComponent.hpp"
#include "WCoreTypes/WMathStructs.hpp"
#include "WCoreTypes/WRenderTypes.hpp"


namespace spacers::monkey {
    
    struct ModelAssets {
        wid::WAssetId static_mesh;
        wid::WAssetId pipeline_asset;
        wid::WAssetId param_asset;
        wid::WEntityId entity;
    };

    inline bool LoadVikingRoom(WEngine & engine, ModelAssets & out_model)
    {
        // Import Viking Room
        wim::importer::WImporterObj obj_importer =
            engine.ImportersRegister().GetImporter<wim::importer::WImporterObj>();

        std::vector<wid::WAssetId> geo_ids =
            obj_importer.Import(
                engine.AssetManager(),
                "Content/Assets/Models/viking_room.obj", 
                "/Content/Assets/viking_room:viking_room"
                );

        out_model.static_mesh = geo_ids[0];

        wim::importer::WImportTexture texture_importer =
            engine.ImportersRegister().GetImporter<wim::importer::WImportTexture>();

        std::vector<wid::WAssetId> tex_ids = texture_importer.Import(
            engine.AssetManager(),
            "Content/Assets/Textures/viking_room.png", 
            "/Content/Assets/viking_texture:viking_texture"
            );

        WAsset * null_texture = engine.AssetManager()
            .Get(weng::defaults::NULL_RGBA_TEXTURE_ASSET_PATH);

        WAsset * null_normal = engine.AssetManager()
            .Get(weng::defaults::NULL_NORMAL_TEXTURE_ASSET_PATH);

        WRenderPipelineAsset * pipeline_asset = engine.AssetManager()
            .Get<WRenderPipelineAsset>(weng::defaults::PBR_PIPELINE_ASSET_PATH);

        out_model.pipeline_asset = pipeline_asset->Get_asset_id();

        // Create Pipeline Parameter Asset

        wid::WAssetId paramid = engine.AssetManager()
            .Create<WRenderPipelineParametersAsset>("/Content/Assets/Params/ParamA:ParamA");

        out_model.param_asset = paramid;

        auto & param_asset = engine.AssetManager().Get<WRenderPipelineParametersAsset>(paramid);
        param_asset.Set_texture_list(
            {
                {.binding=1, .value=tex_ids[0]},
                {.binding=2, .value=null_texture->Get_asset_id()},
                {.binding=3, .value=null_normal->Get_asset_id()},
                {.binding=4, .value=null_texture->Get_asset_id()}
            }
            );

        return true;
    }

    inline bool SetPostprocessPipelines(WEngine & engine, WCameraComponent & camera) {
        wid::WAssetId pipid =
            engine.AssetManager().Create<WRenderPipelineAsset>("/Content/Assets/PPDebug:PPDebug");

        WRenderPipelineAsset & pipeline_asset =
            engine.AssetManager().Get<WRenderPipelineAsset>(pipid);

        pipeline_asset.Set_pipeline_type(wct::render::EPipelineType::Postprocess);

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
        descriptors[0].type = wct::render::EPipeParamType::Ubo;
        descriptors[0].stage_flags = wct::render::EShaderStageFlag::Vertex;
        descriptors[0].size = sizeof(wct::render::WPostprocessUBO);

        pipeline_asset.Set_descriptor_list(descriptors);

        wid::WAssetId paramid =
            engine.AssetManager().Create<WRenderPipelineParametersAsset>(
                "/Content/Assets/PPDebugParam:PPDebugParam"
                );

        camera.SetPostprocessAssignment(0, pipid, paramid);

        return true;
    }

    inline bool LoadMonkey(WEngine & engine, ModelAssets & out_model, const wid::WAssetId & in_render_pipeline) {
        wim::importer::WImporterObj obj_importer =
            engine.ImportersRegister()
            .GetImporter<wim::importer::WImporterObj>();

        std::vector<wid::WAssetId> geo_ids =
            obj_importer.Import(
                engine.AssetManager(),
                "Content/Assets/Models/monkey.obj", 
                "/Content/Assets/monkey:monkey"
                );

        out_model.static_mesh = geo_ids[0];

        wim::importer::WImportTexture tex_importer = engine.ImportersRegister()
            .GetImporter<wim::importer::WImportTexture>();

        std::vector<wid::WAssetId> tex_ids = tex_importer.Import(engine.AssetManager(),
                                                            "Content/Assets/Textures/orange.png",
                                                            "/Content/Assets/orange:orange");

        out_model.pipeline_asset = in_render_pipeline;

        wid::WAssetId paramid = engine.AssetManager()
            .Create<WRenderPipelineParametersAsset>("/Content/Assets/ParamB:ParamB");

        out_model.param_asset = paramid;

        WAsset * null_texture = engine.AssetManager()
            .Get(weng::defaults::NULL_RGBA_TEXTURE_ASSET_PATH);

        WAsset * null_normal = engine.AssetManager()
            .Get(weng::defaults::NULL_NORMAL_TEXTURE_ASSET_PATH);

        auto & param_asset = engine.AssetManager().Get<WRenderPipelineParametersAsset>(paramid);

        param_asset.Set_texture_list(
            {
                {.binding=1, .value=tex_ids[0]},
                {.binding=2, .value=null_texture->Get_asset_id()},
                {.binding=3, .value=null_normal->Get_asset_id()},
                {.binding=4, .value=null_texture->Get_asset_id()}
            }
            );

        return true;
    }

    inline wid::WAssetId ConfigLevel(WEngine & in_engine,
                         const ModelAssets & in_viking_room,
                         const ModelAssets & in_monkey_dt
        ) {

        wid::WAssetId levelid = in_engine.AssetManager().Create<was::Level>(
            "/Content/Level/Level01:Level01"
            );

        was::Level & level = in_engine.AssetManager().Get<was::Level>(levelid);

        in_engine.AddInitSystem(levelid, "SystemInit_CameraInput");
        in_engine.AddPreSystem(levelid, "SystemPre_CameraInputMovement");
        in_engine.AddPreSystem(levelid, "SystemPre_UpdateMovement");

        // Camera

        wid::WEntityId cid = level.CreateEntity<WEntity>();
        level.CreateComponent<WTransformComponent>(cid);
        level.CreateComponent<WCameraComponent>(cid);
        level.CreateComponent<WMovementComponent>(cid);     // parametrized movement
        level.CreateComponent<WCameraInputComponent>(cid);  // User input

        WCameraComponent & cameracomp = level.GetComponent<WCameraComponent>(cid);
        cameracomp.Set_render_id(1); // The renderable camera
        WTransformComponent * tcmp = &level.GetComponent<WTransformComponent>(cid);

        tcmp->Set_rotation({0.0f, 0.0f, 0.0f});
        tcmp->Set_position({0.0, 0.0f, .5f});

        // postprocess
        SetPostprocessPipelines(in_engine, cameracomp);

        // Models
        // Viking Room
        wid::WEntityId eid = level.CreateEntity<WEntity>();
        level.CreateComponent<WTransformComponent>(eid);
        WTransformComponent & ctcmp = level.GetComponent<WTransformComponent>(eid);
        ctcmp.Set_rotation_order(ERotationOrder::zxy);
        ctcmp.Set_position({0.0, 0.0, -2.f});
        ctcmp.Set_rotation({-3.1415 * 0.5, -3.1415 * 0.5, 0.f});

        level.CreateComponent<WStaticMeshComponent>(eid);    

        WStaticMeshComponent & smcomponent =
            level.GetComponent<WStaticMeshComponent>(eid);
        smcomponent.Set_static_mesh_asset(in_viking_room.static_mesh);
        smcomponent.SetPipelineAssignment(
            0, in_viking_room.pipeline_asset, in_viking_room.param_asset
            );

        // Monkey 1
        wid::WEntityId monkey_id = level.CreateEntity<WEntity>();
        level.CreateComponent<WTransformComponent>(monkey_id);
        auto * monkey_tc = &level.GetComponent<WTransformComponent>(monkey_id);
        monkey_tc->Set_rotation_order(ERotationOrder::zxy);
        monkey_tc->Set_position({0.0, 0.5, -2.0});
        monkey_tc->Set_scale(monkey_tc->Get_scale() * 0.15f);

        level.CreateComponent<WStaticMeshComponent>(monkey_id);

        auto & monkeysm = level.GetComponent<WStaticMeshComponent>(monkey_id);

        monkeysm.Set_static_mesh_asset(in_monkey_dt.static_mesh);
        monkeysm.SetPipelineAssignment(0,
                                       in_monkey_dt.pipeline_asset,
                                       in_monkey_dt.param_asset);

        // Monkey 2
        wid::WEntityId monkey2_id = level.CreateEntity<WEntity>();
        level.CreateComponent<WTransformComponent>(monkey2_id);
        monkey_tc = &level.GetComponent<WTransformComponent>(monkey2_id);
        monkey_tc->Set_rotation_order(ERotationOrder::zxy);
        monkey_tc->Set_position({0.2, 0.65, -2.0});
        monkey_tc->Set_scale(monkey_tc->Get_scale() * 0.25f);

        level.CreateComponent<WStaticMeshComponent>(monkey2_id);
        auto & monkey2sm = level.GetComponent<WStaticMeshComponent>(monkey2_id);

        monkey2sm.Set_static_mesh_asset(in_monkey_dt.static_mesh);
        monkey2sm.SetPipelineAssignment(0,
                                        in_monkey_dt.pipeline_asset,
                                        in_monkey_dt.param_asset);


        // Lights
        // ------
    
        wid::WEntityId point_light_1 = level.CreateEntity<WEntity>();
        level.CreateComponent<WTransformComponent>(point_light_1);
        level.CreateComponent<wcm::light::WPointLightComponent>(point_light_1);
        auto * transform_ptr = &level.GetComponent<WTransformComponent>(point_light_1);

        transform_ptr->Set_position({0.0, 1.1, -2.0});
        transform_ptr->Set_scale(transform_ptr->Get_scale() * 0.1f);

        wcm::light::WPointLightComponent * light_ptr =
            &level.GetComponent<wcm::light::WPointLightComponent>(point_light_1);

        light_ptr->Set_intensity(5.0);
        light_ptr->Set_radius(1.5);
        light_ptr->Set_active(true);

        wid::WEntityId ambient_light = level.CreateEntity<WEntity>();
        level.CreateComponent<wcm::light::WAmbientLightComponent>(ambient_light);

        auto * ambient_ptr = &level.GetComponent<wcm::light::WAmbientLightComponent>(ambient_light);

        ambient_ptr->Set_color({0.5, 0.5, 0.5});
        ambient_ptr->Set_intensity(0.25);
        ambient_ptr->Set_active(true);

        return levelid;

    }

    inline wid::WAssetId CreateMonkeyLevel(WEngine & in_engine) {
                std::vector<wct::render::WPipelineAssignment> ppcsst_assignments;

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


