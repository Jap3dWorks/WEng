#include "WImporter/WImporterObj.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WObjectDb/WAssetDb.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// WImportObj
// -----------

wim::importer::WImporterObj::WImporterObj() noexcept {}

std::vector<WAssetId> wim::importer::WImporterObj::Import(
    WAssetDb & in_asset_manager,
    std::string_view file_path,
    std::string_view asset_directory
    )
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
            std::string(file_path).data()
        )
    )
    {
        throw std::runtime_error(warning + error);
    }

    uint32_t index_offset = 0;
    
    std::vector<wct::geometry::WMesh> meshes(shapes.size());
    std::vector<std::string> names(shapes.size());

    for (const auto & shape : shapes)
    {
        wct::geometry::WMesh & mesh = meshes[index_offset];
        names[index_offset] = shape.name;

        ++index_offset;

        std::unordered_map<wct::geometry::WVertex, uint32_t> unique_vertices = {};

        for (const auto & index : shape.mesh.indices)
        {
            wct::geometry::WVertex vertex = {};

            vertex.position = {
                attrib.vertices[(3 * index.vertex_index) + 0],
                attrib.vertices[(3 * index.vertex_index) + 1],
                attrib.vertices[(3 * index.vertex_index) + 2]
            };

            vertex.tex_coords = {
                attrib.texcoords[(2 * index.texcoord_index) + 0],
                1.f - attrib.texcoords[(2 * index.texcoord_index) + 1]
            };

            vertex.color = {1.0f, 1.0f, 1.0f};

            vertex.normal = {
                attrib.normals[(3 * index.normal_index) + 0],
                attrib.normals[(3 * index.normal_index) + 1],
                attrib.normals[(3 * index.normal_index) + 2]
            };

            if (!unique_vertices.contains(vertex))
            {
                unique_vertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
                mesh.vertices.push_back(vertex);
            }

            mesh.indices.push_back(unique_vertices[vertex]);
        }
    }

    std::vector<WAssetId> result {
        in_asset_manager.Create<WStaticMeshAsset>("StaticMesh")
    };
    
    WStaticMeshAsset * static_mesh =
        in_asset_manager.Get<WStaticMeshAsset>(result[0]);

    for (uint32_t i=0; i < meshes.size(); i++) {
        static_mesh->SetMesh(std::move(meshes[i]), i);
    }

    return result;
};
