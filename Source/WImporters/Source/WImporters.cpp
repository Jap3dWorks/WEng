#include "WImporters.h"
#include "WAssets/WStaticModel.h"
#include "WObjectManager/WObjectManager.h"
#include "WAssets/WStaticModel.h"

#include <vector>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

std::vector<WAsset*> WImportObj::Import(const char* file_path, const char* dst_path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning, error;

    if(
        !tinyobj::LoadObj(
            &attrib, 
            &shapes, 
            &materials, 
            &warning, 
            &error, 
            file_path
        )
    )
    {
        throw std::runtime_error(warning + error);
    }

    WModel model = {};
    model.meshes.resize(shapes.size());
    uint32_t index_offset = 0;

    for (const auto& shape : shapes)
    {
        WMesh& mesh = model.meshes[index_offset++];
        std::unordered_map<WVertex, uint32_t> unique_vertices = {};

        for (const auto& index : shape.mesh.indices)
        {
            WVertex vertex = {};

            vertex.Position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.Color = {1.0f, 1.0f, 1.0f, 1.0f};

            if (!unique_vertices.count(vertex) == 0)
            {
                unique_vertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
                mesh.vertices.push_back(vertex);
            }

            mesh.indices.push_back(unique_vertices[vertex]);
        }
    }

    // create an static asset
    WObjectManager& object_manager = WObjectManager::GetInstance();
    WStaticModel* static_model = object_manager.CreateObject<WStaticModel>(
        "StaticModel"
    );
    static_model->SetModel(std::move(model));
    static_model->SetFilePath(file_path);

    std::vector<WAsset*> imported_assets = {static_model};
    return imported_assets;
};

std::vector<std::string> WImportObj::Extensions()
{
    return {".obj"};
}

std::vector<std::string> WImportObj::Formats()
{
    return {"obj"};
}

