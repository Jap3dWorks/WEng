#include "WImporters.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WObjectManager/WObjectManager.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WStructs/WTextureStructs.hpp"

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

constexpr WImporter::WImporter(WObjectManager & in_object_manager) noexcept :
    object_manager_(in_object_manager) {}

constexpr WImporter::WImporter(const WImporter & other) noexcept :
    object_manager_(other.object_manager_) {}

constexpr WImporter::WImporter(WImporter && out_other) noexcept :
    object_manager_(std::move(out_other.object_manager_)) {}

constexpr WImporter & WImporter::operator=(const WImporter & other)  noexcept
{
    if (this != &other) {
        object_manager_ = other.object_manager_;        
    }

    return *this;
}

constexpr WImporter & WImporter::operator=(WImporter && other) noexcept
{
    if (this != &other) {
        object_manager_ = std::move(other.object_manager_);
    }
    
    return *this;
}

WObjectManager & WImporter::ObjectManager() 
{
    return object_manager_.Get();
}


// WImportObj
// -----------

WImportObj::WImportObj(WObjectManager & in_object_manager) :
    WImporter(in_object_manager){}

std::unique_ptr<WImporter> WImportObj::clone()
{
    return std::make_unique<WImportObj>(*this);
}

std::vector<TWRef<WAsset>> WImportObj::Import(const char* file_path, const char* asset_directory)
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

    // WModelStruct model = {};
    // model.meshes.resize(shapes.size());
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

    std::vector<TWRef<WAsset>> imported_assets(shapes.size());

    for (uint32_t i=0; i < meshes.size(); i++) {
        TWRef<WStaticMeshAsset> static_mesh =
            ObjectManager().Get<WStaticMeshAsset>(
                ObjectManager().Create<WStaticMeshAsset>(
                    "StaticMesh"
                    ));

        static_mesh->SetMesh(std::move(meshes[i]));
        // static_mesh->SetPath(WStringUtils::AssetPath(asset_directory, file_path, names[i]));

        // imported_assets[i] = std::move(static_mesh);
        imported_assets[i] = static_mesh.Ptr();
    }

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

// WImportTexture
// --------------

WImportTexture::WImportTexture(WObjectManager & in_object_manager) :
    WImporter(in_object_manager) {}

std::unique_ptr<WImporter> WImportTexture::clone()
{
    return std::make_unique<WImportTexture>(*this);
}

std::vector<TWRef<WAsset>> WImportTexture::Import(const char* file_path, const char* asset_directory)
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
    TWRef<WTextureAsset> texture_asset =
        ObjectManager().Get<WTextureAsset>(
            ObjectManager().Create<WTextureAsset>(
                "TextureAsset"
                ));

    texture_asset->SetTexture(std::move(texture_struct));
    // texture_asset->SetPath(WStringUtils::AssetPath(asset_directory, file_path, "texture"));

    return { texture_asset.Ptr() };
}

std::vector<std::string> WImportTexture::Extensions()
{
    return {".png", ".jpg", ".jpeg", ".tga", ".bmp"};
}

std::vector<std::string> WImportTexture::Formats()
{
    return {"png", "jpg", "jpeg", "tga", "bmp"};
}
