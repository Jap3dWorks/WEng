#include "WSpacers.hpp"

#include "WAssets/WInputMappingAsset.hpp"
#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WComponents/WCameraInputComponent.hpp"
#include "WComponents/WMovementComponent.hpp"
#include "WEngine/WEngine.hpp"
#include "WLog.hpp"
#include "WCore/WCore.hpp"
#include "WImporterRegister.hpp"
#include "WImporters.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WStructs/WComponentStructs.hpp"
#include "WStructs/WEngineStructs.hpp"
#include "WStructs/WGeometryStructs.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WComponents/WStaticMeshComponent.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WCameraInputComponent.hpp"
#include "WComponents/WMovementComponent.hpp"
#include "WStructs/WMathStructs.hpp"
#include "WStructs/WRenderStructs.hpp"

#include <exception>
#include <glm/ext/matrix_transform.hpp>
#include <vector>

bool Run(WEngine & engine)
{
    engine.Run();
    return true;
}

struct ModelAssets {
    WAssetId static_mesh;
    WAssetId pipeline_asset;
    WAssetId param_asset;
    WEntityId entity;
};

bool LoadVikingRoom(WEngine & engine, ModelAssets & out_model)
{
    // Import Viking Room
    WImportObj obj_importer =
        engine.ImportersRegister().GetImporter<WImportObj>();

    std::vector<WAssetId> geo_ids =
        obj_importer.Import(
            engine.AssetManager(),
            "Content/Assets/Models/viking_room.obj", 
            "/Content/Assets/viking_room.viking_room"
            );

    out_model.static_mesh = geo_ids[0];

    WImportTexture texture_importer =
        engine.ImportersRegister().GetImporter<WImportTexture>();

    std::vector<WAssetId> tex_ids = texture_importer.Import(
        engine.AssetManager(),
        "Content/Assets/Textures/viking_room.png", 
        "/Content/Assets/viking_texture.viking_texture"
        );

    // Create Render Pipeline Asset

    WAssetId pipelineid = engine.AssetManager()
        .Create<WRenderPipelineAsset>("/Content/Assets/PipelineA.PipelineA");

    out_model.pipeline_asset = pipelineid;

    WRenderPipelineAsset * pipeline_asset = engine.AssetManager()
        .Get<WRenderPipelineAsset>(pipelineid);

    pipeline_asset->RenderPipeline().type = EPipelineType::Graphics;

    // TEMP use gbufer shader for debug

    pipeline_asset->RenderPipeline().shaders[0].type=EShaderStageFlag::Vertex;

    std::strcpy(
        pipeline_asset->RenderPipeline().shaders[0].file,
        "/Content/Shaders/WRender_GBuffer.graphic.spv"
        );

    std::strcpy(
        pipeline_asset->RenderPipeline().shaders[0].entry,
        "vsMain"
        );

    pipeline_asset->RenderPipeline().shaders[1].type=EShaderStageFlag::Fragment;
    
    std::strcpy(
        pipeline_asset->RenderPipeline().shaders[1].file,
        "/Content/Shaders/WRender_GBuffer.graphic.spv"
        );

    std::strcpy(
        pipeline_asset->RenderPipeline().shaders[1].entry,
        "fsMain"
        );

    auto & params = pipeline_asset->RenderPipeline().params_descriptor;

    params[0].binding=0;
    params[0].type=EPipeParamType::Ubo;
    params[0].stage_flags=EShaderStageFlag::Vertex;
    params[0].range=sizeof(WUBOGraphicsStruct);
    params[1].binding=1;
    params[1].type=EPipeParamType::Texture;
    params[1].stage_flags=EShaderStageFlag::Fragment;

    // Create Pipeline Parameter Asset

    WAssetId paramid = engine.AssetManager()
        .Create<WRenderPipelineParametersAsset>("/Content/Assets/ParamA.ParamA");

    out_model.param_asset = paramid;

    auto * param_asset = engine.AssetManager().Get<WRenderPipelineParametersAsset>(paramid);

    param_asset->RenderPipelineParameters().texture_params.push_back(
        {.binding=1, .value=tex_ids[0]}
        );

    return true;
}

bool PostprocessPipelines(WEngine & engine, std::vector<WRenderPipelineAssignmentStruct> & out_postprcss) {
    WAssetId pipid = engine.AssetManager().Create<WRenderPipelineAsset>("/Content/Assets/PPBlur.PPBlur");

    WRenderPipelineAsset * pipeline_asset =
        engine.AssetManager().Get<WRenderPipelineAsset>(pipid);

    pipeline_asset->RenderPipeline().type = EPipelineType::Postprocess;
    pipeline_asset->RenderPipeline().shaders[0].type=EShaderStageFlag::Vertex;

    std::strcpy(pipeline_asset->RenderPipeline().shaders[0].file,
                "/Content/Shaders/WRender_blur.pprcess.spv");
    std::strcpy(pipeline_asset->RenderPipeline().shaders[0].entry, "vsMain");

    pipeline_asset->RenderPipeline().shaders[1].type=EShaderStageFlag::Fragment;
    std::strcpy(pipeline_asset->RenderPipeline().shaders[1].file,
                "/Content/Shaders/WRender_blur.pprcess.spv");
    std::strcpy(pipeline_asset->RenderPipeline().shaders[1].entry, "fsMain");

    auto & params = pipeline_asset->RenderPipeline().params_descriptor;
    params[0].binding = 0;
    params[0].type = EPipeParamType::Ubo;
    params[0].stage_flags = EShaderStageFlag::Vertex;
    params[0].range = sizeof(WUBOPostprocessStruct);

    WAssetId paramid =
        engine.AssetManager().Create<WRenderPipelineParametersAsset>(
            "/Content/Assets/PPBlurParam.PPBlurParam"
            );

    WRenderPipelineParametersAsset * paramass =
        engine.AssetManager().Get<WRenderPipelineParametersAsset>(paramid);

    out_postprcss.clear();

    out_postprcss.push_back({pipid, paramid});

    return true;
}

bool LoadMonkey(WEngine & engine, ModelAssets & out_model, const WAssetId & in_render_pipeline) {
    WImportObj obj_importer =
        engine.ImportersRegister()
        .GetImporter<WImportObj>();

    std::vector<WAssetId> geo_ids =
        obj_importer.Import(
            engine.AssetManager(),
            "Content/Assets/Models/monkey.obj", 
            "/Content/Assets/monkey.monkey"
            );

    out_model.static_mesh = geo_ids[0];

    WImportTexture tex_importer = engine.ImportersRegister()
        .GetImporter<WImportTexture>();

    std::vector<WAssetId> tex_ids = tex_importer.Import(engine.AssetManager(),
                                                        "Content/Assets/Textures/orange.png",
                                                        "/Content/Assets/orange.orange");

    out_model.pipeline_asset = in_render_pipeline;

    WAssetId paramid = engine.AssetManager()
        .Create<WRenderPipelineParametersAsset>("/Content/Assets/ParamB.ParamB");

    out_model.param_asset = paramid;

    auto * param_asset = engine.AssetManager().Get<WRenderPipelineParametersAsset>(paramid);

    param_asset->RenderPipelineParameters().texture_params.push_back(
        {.binding=1, .value=tex_ids[0]}
        );

    return true;
}

bool InputAssets(WEngine & in_engine) {
    WAssetId cameramapping = in_engine.AssetManager().Create<WInputMappingAsset>(
        "/Content/Input/CameraMapping.CameraMapping"
        );

    WAssetId frontaction = in_engine.AssetManager().Create<WActionAsset>(
        "/Content/Input/FrontAction.FrontAction"
        );

    WAssetId backaction = in_engine.AssetManager().Create<WActionAsset>(
        "/Content/Input/BackAction.BackAction"
        );

    WAssetId leftaction = in_engine.AssetManager().Create<WActionAsset>(
        "/Content/Input/LeftAction.LeftAction"
        );

    WAssetId rightaction = in_engine.AssetManager().Create<WActionAsset>(
        "/Content/Input/RightAction.RightAction"
        );

    WAssetId mousemovement = in_engine.AssetManager().Create<WActionAsset>(
        "/Content/Input/MouseMovement.MouseMovement"
        );

    WInputMapStruct & input_map = in_engine.AssetManager()
        .Get<WInputMappingAsset>(cameramapping)->InputMap();

    input_map.map[{EInputKey::Key_W, EInputMode::Press}] = {frontaction};
    input_map.map[{EInputKey::Key_W, EInputMode::Release}] = {frontaction};
    input_map.map[{EInputKey::Key_S, EInputMode::Press}] = {backaction};
    input_map.map[{EInputKey::Key_S, EInputMode::Release}] = {backaction};
    input_map.map[{EInputKey::Key_A, EInputMode::Press}] = {leftaction};
    input_map.map[{EInputKey::Key_A, EInputMode::Release}] = {leftaction};
    input_map.map[{EInputKey::Key_D, EInputMode::Press}] = {rightaction};
    input_map.map[{EInputKey::Key_D, EInputMode::Release}] = {rightaction};
    input_map.map[{EInputKey::Mouse_Move, EInputMode::None}] = {mousemovement};

    return true;
}

bool SetupLevel(WEngine & in_engine,
                const ModelAssets & in_viking_room,
                const ModelAssets & in_monkey_dt,
                const std::vector<WRenderPipelineAssignmentStruct> & in_ppcss_assgnm
    ) {

    WLevelId levelid = in_engine.LevelRegister().Create();

    WLevel & level = in_engine.LevelRegister().Get(levelid);

    in_engine.AddInitSystem(levelid, "SystemInit_CameraInput");
    in_engine.AddPreSystem(levelid, "SystemPre_CameraInputMovement");
    in_engine.AddPreSystem(levelid, "SystemPre_UpdateMovement");

    in_engine.StartupLevel(levelid);

    // Camera

    WEntityId cid = level.CreateEntity<WEntity>();
    level.CreateComponent<WTransformComponent>(cid);
    level.CreateComponent<WCameraComponent>(cid);
    level.CreateComponent<WMovementComponent>(cid);
    level.CreateComponent<WCameraInputComponent>(cid);

    WCameraComponent & cameracomp = level.GetComponent<WCameraComponent>(cid);
    cameracomp.RenderId(1); // renderable camera
    WTransformStruct & cts = level.GetComponent<WTransformComponent>(cid).TransformStruct();
    cts.rotation = {0.0f, 0.0f, 0.0f};
    cts.position = {0.0, 0.0f, .5f};

    // postprocess
    
    for(std::uint8_t i=0; i < in_ppcss_assgnm.size(); i++) {
        cameracomp.SetRenderPipelineAssignment({i}, in_ppcss_assgnm[i]);
    }

    // Models
    // Viking Room
    WEntityId eid = level.CreateEntity<WEntity>();
    level.CreateComponent<WTransformComponent>(eid);
    WTransformStruct & ts = level.GetComponent<WTransformComponent>(eid).TransformStruct();
    ts.rotation_order = ERotationOrder::zxy;
    ts.position.z = -2.0f;
    ts.rotation.x = -3.1415 * 0.5;
    ts.rotation.y = -3.1415 * 0.5 ;

    level.CreateComponent<WStaticMeshComponent>(eid);    

    WStaticMeshComponent & smcomponent =
        level.GetComponent<WStaticMeshComponent>(eid);
    smcomponent.StaticMeshAsset(in_viking_room.static_mesh);
    smcomponent.SetRenderPipelineAssignment(
        0, in_viking_room.pipeline_asset, in_viking_room.param_asset
        );

    // Monkey 1
    WEntityId monkey_id = level.CreateEntity<WEntity>();
    level.CreateComponent<WTransformComponent>(monkey_id);
    WTransformStruct & monkey_ts = level.GetComponent<WTransformComponent>(monkey_id).TransformStruct();
    monkey_ts.rotation_order = ERotationOrder::zxy;
    monkey_ts.position.z = -2.0;
    monkey_ts.position.y = 0.5;
    monkey_ts.scale *= 0.15;

    level.CreateComponent<WStaticMeshComponent>(monkey_id);

    auto & monkeysm = level.GetComponent<WStaticMeshComponent>(monkey_id);

    monkeysm.StaticMeshAsset(in_monkey_dt.static_mesh);
    monkeysm.SetRenderPipelineAssignment(0,
                                         in_monkey_dt.pipeline_asset,
                                         in_monkey_dt.param_asset);

    // Monkey 2
    WEntityId monkey2_id = level.CreateEntity<WEntity>();
    level.CreateComponent<WTransformComponent>(monkey2_id);
    WTransformStruct & monkey2_ts = level.GetComponent<WTransformComponent>(monkey2_id).TransformStruct();
    monkey2_ts.rotation_order = ERotationOrder::zxy;
    monkey2_ts.position.z = -2.0;
    monkey2_ts.position.y = 0.65;
    monkey2_ts.position.x = 0.2;
    monkey2_ts.scale *= 0.25;

    level.CreateComponent<WStaticMeshComponent>(monkey2_id);
    auto & monkey2sm = level.GetComponent<WStaticMeshComponent>(monkey2_id);

    monkey2sm.StaticMeshAsset(in_monkey_dt.static_mesh);
    monkey2sm.SetRenderPipelineAssignment(0,
                                          in_monkey_dt.pipeline_asset,
                                          in_monkey_dt.param_asset);

    return true;

}

int main(int argc, char** argv)
{
    WFLOG("Initialize WSpacers App.");
    
    try
    {
        WFLOG("Create Engine Instance.");
        
        WEngine engine = WEngine::DefaultCreate();

        std::vector<WRenderPipelineAssignmentStruct> ppcsst_assignments;

        ModelAssets viking_room;
        ModelAssets monkey_1;
        ModelAssets monkey_2;

        WFLOG("[INFO] Create Assets.");

        LoadVikingRoom(engine, viking_room);
        LoadMonkey(engine, monkey_1, viking_room.pipeline_asset);

        PostprocessPipelines(engine, ppcsst_assignments);

        InputAssets(engine);

        WFLOG("[INFO] Setup Init Level.");
       
        SetupLevel(engine,
                   viking_room,
                   monkey_1,
                   ppcsst_assignments
            );

        WFLOG("[INFO] Initialize While Loop");

        Run(engine);

        WFLOG("Clossing app ...");

        engine.Destroy();

    }
    catch(const std::exception& e)
    {
        WFLOG("[ERROR] {}", e.what());
        throw;
        // return 1;
    }

    WFLOG("WSpacers App Ends");

    return 0;

}

