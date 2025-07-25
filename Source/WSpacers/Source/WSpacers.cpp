#include "WSpacers.hpp"

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

bool LoadAssets(WEngine & engine, WStaticMeshAsset *& out_static_model, WTextureAsset *& out_texture_asset)
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

    return true;

}

int main(int argc, char** argv)
{
    bool as_plane=false;
    
    try
    {
        WEngine engine = WEngine::DefaultCreate();

        // TODO use WRenderPipelineAsset
        WId pipeline_wid = engine.AssetManager().Create(
            WRenderPipelineAsset::StaticClass(),
            "/Content/RenderPipeline/RPip.RPip");

        TWRef<WRenderPipelineAsset> render_pipeline =
            static_cast<WRenderPipelineAsset*>(
                engine.AssetManager().Get(pipeline_wid)
                );

        render_pipeline->RenderPipeline().type = EPipelineType::Graphics;
        render_pipeline->RenderPipeline().shaders[0].type=EShaderType::Vertex;
        std::strcpy(render_pipeline->RenderPipeline().shaders[0].file,
                    "/Content/Shaders/Spacers_ShaderBase.vert");

        render_pipeline->RenderPipeline().shaders[1].type=EShaderType::Fragment;
        std::strcpy(render_pipeline->RenderPipeline().shaders[1].file,
                    "/Content/Shaders/Spacers_ShaderBase.frag");

        engine.Render()->CreateRenderPipeline(render_pipeline.Ptr());

        WStaticMeshAsset * static_mesh;
        WTextureAsset * texture_asset;

        if (!LoadAssets(engine, static_mesh, texture_asset))
        {
            return 1;
        }
	        
        const WTextureStruct & texture_data =
            texture_asset->GetTexture();

        engine.Render()->RenderResources()->RegisterStaticMesh(static_mesh);
        engine.Render()->RenderResources()->RegisterTexture(texture_asset);

        // Load Resources into Graphics Memory
        engine.Render()->RenderResources()->LoadStaticMesh(static_mesh->WID());
        engine.Render()->RenderResources()->LoadTexture(texture_asset->WID());

        engine.Render()->AddPipelineBinding(
            pipeline_wid,
            static_mesh->WID(),
            {texture_asset->WID()},
            {1}
            );

        WLOG("Bind Pipeline: {:d}", pipeline_wid.GetId());

        run(engine);

    }
    catch(const std::exception& e)
    {
        WLOG("[ERROR] {}", e.what());

        return 1;
    }

    return 0;

}

