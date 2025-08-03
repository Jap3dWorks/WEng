#include "WSpacers.hpp"

#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WEngine.hpp"
#include "WLog.hpp"
#include "WCore/WCore.hpp"
#include "WImporterRegister.hpp"
#include "WImporters.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WStructs/WGeometryStructs.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WComponents/WStaticMeshComponent.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

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

    std::vector<WId> geo_ids =
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

    TWRef<WAsset> geo_asset = engine.AssetManager().Get(geo_ids[0]);

    if (geo_asset->Class() != WStaticMeshAsset::StaticClass())
    {
        std::cout << "geo_asset is not a static model!" << std::endl;
        return false;
    }

    WImportTexture texture_importer =
        engine.ImportersRegister().GetImporter<WImportTexture>();

    std::vector<WId> tex_ids = texture_importer.Import(
        engine.AssetManager(),
        "Content/Assets/Textures/viking_room.png", 
        "/Content/Assets/viking_texture.viking_texture"
        );

    if (tex_ids.size() < 1)
    {
        std::cout << "Failed to import tex_asset!" << std::endl;
        return false;
    }

    TWRef<WAsset> tex_asset = engine.AssetManager().Get(tex_ids[0]);

    if (tex_asset->Class() != WTextureAsset::StaticClass())
    {
        std::cout << "tex_asset is not a texture!" << std::endl;
        return false;
    }

    out_static_model = static_cast<WStaticMeshAsset*>(geo_asset.Ptr());
    out_texture_asset = static_cast<WTextureAsset*>(tex_asset.Ptr());

    WAssetId pipelineid = engine.AssetManager()
        .Create<WRenderPipelineAsset>("/Content/Assets/PipelineA.PipelineA");

    out_pipeline_asset = static_cast<WRenderPipelineAsset*>(
        engine.AssetManager().Get(pipelineid)
        );

    out_pipeline_asset->RenderPipeline().type = EPipelineType::Graphics;

    out_pipeline_asset->RenderPipeline().shaders[0].type=EShaderType::Vertex;
    std::strcpy(out_pipeline_asset->RenderPipeline().shaders[0].file,
                "/Content/Shaders/Spacers_ShaderBase.vert");

    out_pipeline_asset->RenderPipeline().shaders[1].type=EShaderType::Fragment;
    std::strcpy(out_pipeline_asset->RenderPipeline().shaders[1].file,
                "/Content/Shaders/Spacers_ShaderBase.frag");

    WAssetId paramid = engine.AssetManager()
        .Create<WRenderPipelineParametersAsset>("/Content/Assets/ParamA.ParamA");

    out_param_asset = static_cast<WRenderPipelineParametersAsset*>(
        engine.AssetManager().Get(paramid)
        );

    out_param_asset->RenderPipelineParameters().texture_assets[0].value = tex_asset->WID();
    out_param_asset->RenderPipelineParameters().texture_assets[0].binding = 1;
    out_param_asset->RenderPipelineParameters().texture_assets_count = 1;

    return true;
}

bool SetupLevel(WEngine & in_engine,
                const WAssetId & in_smid,
                const WAssetId & in_pipelineid,
                const WAssetId & in_paramsid
) {

    WLevelId levelid = in_engine.LevelRegister().Create();

    TOptionalRef<WLevel> level = in_engine.LevelRegister().Get(levelid);

    WEntityId eid = level->CreateEntity<WEntity>();

    level->CreateComponent<WTransformComponent>(eid);

    WEntityComponentId smid = level->CreateComponent<WStaticMeshComponent>(eid);
    WStaticMeshComponent * smcomponent = level->GetComponent<WStaticMeshComponent>(eid);
    
    smcomponent->StaticMeshAsset(in_smid);
    smcomponent->RenderPipelineAsset(in_pipelineid);
    smcomponent->RenderPipelineParametersAsset(in_paramsid);

    in_engine.StartupLevel(levelid);

    return true;

}

int main(int argc, char** argv)
{
    bool as_plane=false;
    
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
	        
        const WTextureStruct & texture_data =
            texture_asset->GetTexture();

        SetupLevel(engine,
                   static_mesh->WID(),
                   pipeline_asset->WID(),
                   param_asset->WID()
            );

        run(engine);

    }
    catch(const std::exception& e)
    {
        WLOG("[ERROR] {}", e.what());

        return 1;
    }

    return 0;

}

