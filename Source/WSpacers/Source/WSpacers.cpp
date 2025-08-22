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

#include <exception>
#include <glm/ext/matrix_transform.hpp>
#include <vector>

#include <iostream>

bool run(WEngine & engine)
{
    engine.run();
    return true;
}

WModelStruct MeshPlane()
{
    WModelStruct model;
    WMeshStruct mesh;

    mesh.vertices.push_back({
            glm::vec3(-1.f, -1.f, 0.f),
            glm::vec2(0.f, 0.f),
            glm::vec3(0.f, 0.f, 1.f)
        });

    mesh.vertices.push_back({
            glm::vec3(-1.f, 1.f, 0.f),
            glm::vec2(0.f, 1.f),
            glm::vec3(0.f, 0.f, 1.f)
        });

    mesh.vertices.push_back({
            glm::vec3(1.f, 1.f, 0.f),
            glm::vec2(1.f, 1.f),
            glm::vec3(0.f, 0.f, 1.f)
        });

    mesh.vertices.push_back({
            glm::vec3(1.f, -1.f, 0.f),
            glm::vec2(1.f, 0.f),
            glm::vec3(0.f, 0.f, 1.f)
        });

    mesh.indices.push_back(0);
    mesh.indices.push_back(3);
    mesh.indices.push_back(1);
    mesh.indices.push_back(2);
    mesh.indices.push_back(1);
    mesh.indices.push_back(3);

    model.meshes.push_back(mesh);

    return model;
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

    pipeline_asset->RenderPipeline().shaders[0].type=EShaderType::Vertex;
    std::strcpy(pipeline_asset->RenderPipeline().shaders[0].file,
                "/Content/Shaders/Spacers_ShaderBase.vert");

    pipeline_asset->RenderPipeline().shaders[1].type=EShaderType::Fragment;
    std::strcpy(pipeline_asset->RenderPipeline().shaders[1].file,
                "/Content/Shaders/Spacers_ShaderBase.frag");

    pipeline_asset->RenderPipeline().shaders_count = 2;

    // Create Pipeline Parameter Asset

    WAssetId paramid = engine.AssetManager()
        .Create<WRenderPipelineParametersAsset>("/Content/Assets/ParamA.ParamA");

    out_model.param_asset = paramid;

    auto * param_asset = engine.AssetManager().Get<WRenderPipelineParametersAsset>(paramid);

    param_asset->RenderPipelineParameters().texture_assets[0].value = tex_ids[0];
    param_asset->RenderPipelineParameters().texture_assets[0].binding = 1;
    param_asset->RenderPipelineParameters().texture_assets_count = 1;

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
            "/Content/Assets/monkey.monkey"   // TODO only directory
            );

    out_model.static_mesh = geo_ids[0];

    WImportTexture tex_importer = engine.ImportersRegister()
        .GetImporter<WImportTexture>();

    std::vector<WAssetId> tex_ids = tex_importer.Import(engine.AssetManager(),
                        "Content/Assets/Textures/orange.png",
                        "/Content/Assets/orange.orange"
        );

    out_model.pipeline_asset = in_render_pipeline;

    WAssetId paramid = engine.AssetManager()
        .Create<WRenderPipelineParametersAsset>("/Content/Assets/ParamB.ParamB");

    out_model.param_asset = paramid;

    auto * param_asset = engine.AssetManager().Get<WRenderPipelineParametersAsset>(paramid);

    param_asset->RenderPipelineParameters().texture_assets[0].value = tex_ids[0];
    param_asset->RenderPipelineParameters().texture_assets[0].binding=1;
    param_asset->RenderPipelineParameters().texture_assets_count = 1;

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
                const ModelAssets & in_monkey
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

    WTransformStruct & cts = level.GetComponent<WTransformComponent>(cid)->TransformStruct();
    cts.rotation = {0.0f, 0.0f, 0.0f};
    cts.position = {0.0, 0.0f, .5f};

    // Models
    // Viking Room
    WEntityId eid = level.CreateEntity<WEntity>();
    level.CreateComponent<WTransformComponent>(eid);
    WTransformStruct & ts = level.GetComponent<WTransformComponent>(eid)->TransformStruct();
    ts.rotation_order = ERotationOrder::xzy;
    ts.position.z = -2.0f;
    ts.rotation.x = -3.1415 * 0.5;
    ts.rotation.y = -3.1415 * 0.5 ;

    level.CreateComponent<WStaticMeshComponent>(eid);

    WStaticMeshComponent * smcomponent =
        level.GetComponent<WStaticMeshComponent>(eid);
    smcomponent->StaticMeshAsset(in_viking_room.static_mesh);
    smcomponent->RenderPipelineAsset(in_viking_room.pipeline_asset);
    smcomponent->RenderPipelineParametersAsset(in_viking_room.param_asset);

    // Monkey
    WEntityId monkeyid = level.CreateEntity<WEntity>();
    level.CreateComponent<WTransformComponent>(monkeyid);
    WTransformStruct & monkeyts = level.GetComponent<WTransformComponent>(monkeyid)->TransformStruct();
    monkeyts.rotation_order = ERotationOrder::xzy;
    monkeyts.position.z = -2.0;
    monkeyts.position.y = 0.5;
    monkeyts.scale *= 0.25;

    level.CreateComponent<WStaticMeshComponent>(monkeyid);

    auto* monkeysm = level.GetComponent<WStaticMeshComponent>(monkeyid);

    monkeysm->StaticMeshAsset(in_monkey.static_mesh);
    monkeysm->RenderPipelineAsset(in_monkey.pipeline_asset);
    monkeysm->RenderPipelineParametersAsset(in_monkey.param_asset);

    return true;

}

int main(int argc, char** argv)
{
    WFLOG("Initialize WSpacers App.");
    
    try
    {
        WEngine engine = WEngine::DefaultCreate();

        ModelAssets viking_room;
        ModelAssets monkey_1;
        ModelAssets monkey_2;

        LoadVikingRoom(engine, viking_room);
        LoadMonkey(engine, monkey_1, viking_room.pipeline_asset);

        InputAssets(engine);
       
        SetupLevel(engine,
                   viking_room,
                   monkey_1);

        WFLOG("Initialize While Loop");

        run(engine);

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

