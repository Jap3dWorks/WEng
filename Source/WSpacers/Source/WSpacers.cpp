#include "WSpacers.hpp"

#include "WAssets/WInputMappingAsset.hpp"
#include "WAssets/WRenderPipelineParametersAsset.hpp"
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

bool LoadAssets(WEngine & engine,
                WStaticMeshAsset *& out_static_model,
                WTextureAsset *& out_texture_asset,
                WRenderPipelineAsset *& out_pipeline_asset,
                WRenderPipelineParametersAsset *& out_param_asset
    )
{
    WImportObj obj_importer =
        engine.ImportersRegister().GetImporter<WImportObj>();

    std::vector<WAssetId> geo_ids =
        obj_importer.Import(
            engine.AssetManager(),
            "Content/Assets/Models/viking_room.obj", 
            "/Content/Assets/viking_room.viking_room"
            );

    if (geo_ids.size() < 1)
    {
        std::cout << "Failed to import geo_asset!" << std::endl;
        return false;
    }

    out_static_model = engine.AssetManager().Get<WStaticMeshAsset>(geo_ids[0]);

    WImportTexture texture_importer =
        engine.ImportersRegister().GetImporter<WImportTexture>();

    std::vector<WAssetId> tex_ids = texture_importer.Import(
        engine.AssetManager(),
        "Content/Assets/Textures/viking_room.png", 
        "/Content/Assets/viking_texture.viking_texture"
        );

    if (tex_ids.size() < 1)
    {
        std::cout << "Failed to import tex_asset!" << std::endl;
        return false;
    }

    out_texture_asset = engine.AssetManager().Get<WTextureAsset>(tex_ids[0]);

    // Create Render Pipeline Asset

    WAssetId pipelineid = engine.AssetManager()
        .Create<WRenderPipelineAsset>("/Content/Assets/PipelineA.PipelineA");

    out_pipeline_asset = engine.AssetManager()
        .Get<WRenderPipelineAsset>(pipelineid);

    out_pipeline_asset->RenderPipeline().type = EPipelineType::Graphics;

    out_pipeline_asset->RenderPipeline().shaders[0].type=EShaderType::Vertex;
    std::strcpy(out_pipeline_asset->RenderPipeline().shaders[0].file,
                "/Content/Shaders/Spacers_ShaderBase.vert");

    out_pipeline_asset->RenderPipeline().shaders[1].type=EShaderType::Fragment;
    std::strcpy(out_pipeline_asset->RenderPipeline().shaders[1].file,
                "/Content/Shaders/Spacers_ShaderBase.frag");

    out_pipeline_asset->RenderPipeline().shaders_count = 2;

    // Create Pipeline Parameter Asset

    WAssetId paramid = engine.AssetManager()
        .Create<WRenderPipelineParametersAsset>("/Content/Assets/ParamA.ParamA");

    out_param_asset = static_cast<WRenderPipelineParametersAsset*>(
        engine.AssetManager().Get(paramid)
        );

    out_param_asset->RenderPipelineParameters().texture_assets[0].value = out_texture_asset->WID();
    out_param_asset->RenderPipelineParameters().texture_assets[0].binding = 1;
    out_param_asset->RenderPipelineParameters().texture_assets_count = 1;

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

    WInputMapStruct & input_map = in_engine.AssetManager()
        .Get<WInputMappingAsset>(cameramapping)->InputMap();

    input_map.map[{EInputKey::Key_W, EInputMode::Press}] = {frontaction};
    input_map.map[{EInputKey::Key_W, EInputMode::Repeat}] = {frontaction};
    input_map.map[{EInputKey::Key_S, EInputMode::Press}] = {backaction};
    input_map.map[{EInputKey::Key_S, EInputMode::Repeat}] = {backaction};
    input_map.map[{EInputKey::Key_A, EInputMode::Press}] = {leftaction};
    input_map.map[{EInputKey::Key_A, EInputMode::Repeat}] = {leftaction};
    input_map.map[{EInputKey::Key_D, EInputMode::Press}] = {rightaction};
    input_map.map[{EInputKey::Key_D, EInputMode::Repeat}] = {rightaction};

    return true;
}

bool SetupLevel(WEngine & in_engine,
                const WAssetId & in_smid,
                const WAssetId & in_pipelineid,
                const WAssetId & in_paramsid
    ) {

    WLevelId levelid = in_engine.LevelRegister().Create();

    WLevel & level = in_engine.LevelRegister().Get(levelid);

    // Camera

    WEntityId cid = level.CreateEntity<WEntity>();
    level.CreateComponent<WTransformComponent>(cid);
    level.CreateComponent<WCameraComponent>(cid);

    WTransformStruct & cts = level.GetComponent<WTransformComponent>(cid)->TransformStruct();
    cts.position = {0, 2.f, 2.f};

    // Model

    WEntityId eid = level.CreateEntity<WEntity>();
    level.CreateComponent<WTransformComponent>(eid);

    WEntityComponentId smid = level.CreateComponent<WStaticMeshComponent>(eid);

    WStaticMeshComponent * smcomponent =
        level.GetComponent<WStaticMeshComponent>(eid);
    
    smcomponent->StaticMeshAsset(in_smid);
    smcomponent->RenderPipelineAsset(in_pipelineid);
    smcomponent->RenderPipelineParametersAsset(in_paramsid);

    in_engine.StartupLevel(levelid);

    // Initialize Lvl Function

    level.SetInitFn([cid](WLevel * _level, WEngine * _engine){
        
        WAssetId mapping, frontaction, backaction, leftaction, rightaction;
        
        _engine->AssetManager().ForEach<WInputMappingAsset>([&mapping](WInputMappingAsset * a){
            mapping = a->WID();
        });

        // TODO Get assets by Name
        _engine->AssetManager().ForEach<WActionAsset>([&frontaction,
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

        _engine->InputMappingRegister().PutInputMapping(mapping);

        _engine->InputMappingRegister().BindAction(
            frontaction,
            [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {
                
                WTransformStruct & transform =
                    _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

                // TODO: Get Direction.
                transform.position[0] = transform.position[0] + 0.1;

                WLOG("[InputMapping] InputMapping Trigger!");
            }
            );

        _engine->InputMappingRegister().BindAction(
            backaction,
            [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {
                
                WTransformStruct & transform =
                    _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

                // TODO: Get Direction.
                transform.position[0] = transform.position[0] - 0.1;
                WLOG("[InputMapping] InputMapping Trigger!");
            }
            );

        _engine->InputMappingRegister().BindAction(
            leftaction,
            [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {
                
                WTransformStruct & transform =
                    _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

                // TODO: Get Direction.
                transform.position[0] = transform.position[1] + 0.1;
                WLOG("[InputMapping] InputMapping Trigger!");
            }
            );

        _engine->InputMappingRegister().BindAction(
            frontaction,
            [cid](const WInputValuesStruct & _v, const WActionStruct & _a, WEngine * _e) {
                
                WTransformStruct & transform =
                    _e->LevelInfo().level.GetComponent<WTransformComponent>(cid)->TransformStruct();

                // TODO: Get Direction.
                transform.position[0] = transform.position[1] - 0.1;
                WLOG("[InputMapping] InputMapping Trigger!");
            }
            );
    });

    return true;

}

int main(int argc, char** argv)
{
    WFLOG("Initialize WSpacers App.");
    
    try
    {
        WEngine engine = WEngine::DefaultCreate();

        WStaticMeshAsset * static_mesh;
        WTextureAsset * texture_asset;
        WRenderPipelineAsset * pipeline_asset;
        WRenderPipelineParametersAsset * param_asset;

        if (!LoadAssets(engine,
                        static_mesh,
                        texture_asset,
                        pipeline_asset,
                        param_asset))
        {
            return 1;
        }

        InputAssets(engine);
       
        SetupLevel(engine,
                   static_mesh->WID(),
                   pipeline_asset->WID(),
                   param_asset->WID());

        WFLOG("Initialize While Loop");

        run(engine);

        WFLOG("Clossing app ...");

        engine.Destroy();

    }
    catch(const std::exception& e)
    {
        WFLOG("[ERROR] {}", e.what());

        return 1;
    }

    WFLOG("WSpacers App Ends");

    return 0;

}

