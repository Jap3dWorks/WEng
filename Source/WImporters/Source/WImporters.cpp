#include "WImporters.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WStructs/WTextureStructs.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WCore/WStringUtils.hpp"

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// WImporter
// ---------

WImporter::WImporter() noexcept {}

WImporter::WImporter(const WImporter & other) noexcept :
    asset_manager_(other.asset_manager_) {}

WImporter::WImporter(WImporter && out_other) noexcept :
    asset_manager_(std::move(out_other.asset_manager_)) {}

WImporter & WImporter::operator=(const WImporter & other)  noexcept
{
    if (this != &other) {
        asset_manager_ = other.asset_manager_;        
    }

    return *this;
}

WImporter & WImporter::operator=(WImporter && other) noexcept
{
    if (this != &other) {
        asset_manager_ = std::move(other.asset_manager_);
    }
    
    return *this;
}

// WImportObj
// -----------

WImportObj::WImportObj() noexcept {}

std::vector<WAssetId> WImportObj::Import(
    WAssetDb & in_asset_manager,
    const char* file_path,
    const char* asset_directory)
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

    uint32_t index_offset = 0;
    
    std::vector<WMeshStruct> meshes(shapes.size());
    std::vector<std::string> names(shapes.size());

    for (const auto & shape : shapes)
    {
        WMeshStruct & mesh = meshes[index_offset];
        names[index_offset] = shape.name;

        ++index_offset;

        std::unordered_map<WVertexStruct, uint32_t> unique_vertices = {};

        for (const auto & index : shape.mesh.indices)
        {
            WVertexStruct vertex = {};

            vertex.Position = {
                attrib.vertices[(3 * index.vertex_index) + 0],
                attrib.vertices[(3 * index.vertex_index) + 1],
                attrib.vertices[(3 * index.vertex_index) + 2]
            };

            vertex.TexCoords = {
                attrib.texcoords[(2 * index.texcoord_index) + 0],
                1.f - attrib.texcoords[(2 * index.texcoord_index) + 1]
            };

            vertex.Color = {1.0f, 1.0f, 1.0f};

            if (!unique_vertices.contains(vertex))
            {
                unique_vertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
                mesh.vertices.push_back(vertex);
            }

            mesh.indices.push_back(unique_vertices[vertex]);
        }
    }

    std::vector<WAssetId> result(shapes.size());

    for (uint32_t i=0; i < meshes.size(); i++) {
        WAssetId id = in_asset_manager.Create(
            WStaticMeshAsset::StaticClass(),
            "StaticMesh"
            );

        WStaticMeshAsset* static_mesh =
            static_cast<WStaticMeshAsset*>(in_asset_manager.Get(id));

        static_mesh->SetMesh(std::move(meshes[i]));

        result[i] = id;
    }

    return result;
};

// WImportTexture
// --------------

WImportTexture::WImportTexture() noexcept {}

std::vector<WAssetId> WImportTexture::Import(
    WAssetDb & in_asset_manager,
    const char* file_path,
    const char* asset_directory)
{
    int width, height, num_channels;
    stbi_uc * Pixels = stbi_load(
        file_path,
        &width,
        &height,
        &num_channels,
        STBI_rgb_alpha
    );

    if (!Pixels)
    {
        throw std::runtime_error("Failed to load texture image!");
    }

    WTextureStruct texture_struct = {};
    texture_struct.width = width;
    texture_struct.height = height;

    switch(num_channels) {
    case 1:
        texture_struct.channels = ETextureChannels::kR;
        break;
    case 2:
        texture_struct.channels = ETextureChannels::kRG;
        break;
    case 3:
        texture_struct.channels = ETextureChannels::kRGB;
        break;
    case 4:
        texture_struct.channels = ETextureChannels::kRGBA;
        break;
    default:
        texture_struct.channels = ETextureChannels::kRGBA;
    }

    size_t csize = width * height;

    texture_struct.data.resize(csize * 4, 0);

    std::memcpy(
        texture_struct.data.data(), 
        Pixels,
        csize * num_channels
        // texture_struct.data.size()
    );

    stbi_image_free(Pixels);

    WAssetId id = in_asset_manager.Create<WTextureAsset>(
        WStringUtils::AssetPath(asset_directory, file_path, "texture").c_str()
        );

    static_cast<WTextureAsset*>(in_asset_manager.Get(id))->
        SetTexture(std::move(texture_struct));

    return { id };
}

