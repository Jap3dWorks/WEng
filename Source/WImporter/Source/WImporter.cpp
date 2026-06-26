#include "WImporter/WImporter.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WCoreTypes/WTexture.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WUtils/WStringUtils.hpp"

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

wim::importer::WImporter::WImporter() noexcept {}

wim::importer::WImporter::WImporter(const WImporter & other) noexcept :
    asset_manager_(other.asset_manager_) {}

wim::importer::WImporter::WImporter(WImporter && out_other) noexcept :
    asset_manager_(std::move(out_other.asset_manager_)) {}

wim::importer::WImporter & wim::importer::WImporter::operator=(const WImporter & other)  noexcept
{
    if (this != &other) {
        asset_manager_ = other.asset_manager_;        
    }

    return *this;
}

wim::importer::WImporter & wim::importer::WImporter::operator=(WImporter && other) noexcept
{
    if (this != &other) {
        asset_manager_ = std::move(other.asset_manager_);
    }
    
    return *this;
}

// WImportObj
// -----------

wim::importer::WImportObj::WImportObj() noexcept {}

std::vector<WAssetId> wim::importer::WImportObj::Import(
    WAssetDb & in_asset_manager,
    const char * file_path,
    const char * asset_directory)
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

// WImportTexture
// --------------

wim::importer::WImportTexture::WImportTexture() noexcept {}

std::vector<WAssetId> wim::importer::WImportTexture::Import(
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

    // wct::texture::WTexture texture_struct = {};
    // texture_struct.width = width;
    // texture_struct.height = height;

    wct::texture::ETextureFormat format;

    // Default formats
    switch(num_channels) {
    case 1:
        format = wct::texture::ETextureFormat::R8_UNORM;
        break;
    case 2:
        format = wct::texture::ETextureFormat::RG8_UNORM;
        break;
    case 3:
        format = wct::texture::ETextureFormat::RGB8_SRGB;
        break;
    case 4:
        format = wct::texture::ETextureFormat::RGBA8_SRGB;
        break;
    default:
        format = wct::texture::ETextureFormat::RGBA8_SRGB;
    }

    // size_t csize = width * height;

    // texture_struct.data.resize(csize * 4, 0);

    // std::memcpy(
    //     texture_struct.data.data(), 
    //     Pixels,
    //     csize * num_channels
    //     // texture_struct.data.size()
    // );

    // TODO is this filename okey?

    WAssetId id = in_asset_manager.Create<WTextureAsset>(
        WStringUtils::AssetPath(asset_directory, file_path, "texture").c_str()
        );

    auto * asset = in_asset_manager.Get<WTextureAsset>(id);

    asset->SetTextureData(
        Pixels,
        width, height,
        format
        );

    stbi_image_free(Pixels);

    return { id };
}

