#include "WImporters.h"

#include <utility>
#include <vector>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "WAssets/WStaticModel.h"
#include "WObjectManager/WObjectManager.h"
#include "WAssets/WStaticModel.h"
#include "WAssets/WTextureAsset.h"
#include "WStructs/WTextureStructs.h"

// WImporter
// ---------

WImporter::WImporter(WObjectManager & in_object_manager) :
    object_manager_(in_object_manager) {}

WImporter::WImporter(const WImporter & in_other) 
{
    object_manager_ = in_other.object_manager_;
}

WImporter::WImporter(WImporter && out_other) noexcept
{
    Move(std::move(out_other));
}

WImporter & WImporter::operator=(const WImporter & in_other) 
{
    object_manager_ = in_other.object_manager_;
    return *this;
}

WImporter & WImporter::operator=(WImporter && out_other) noexcept
{
    Move(std::move(out_other));
    return *this;
}


void WImporter::Move(WImporter && other) noexcept
{
    object_manager_ = std::move(other.object_manager_);
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

std::vector<WAsset*> WImportObj::Import(const char* file_path, const char* asset_path)
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

    WModelStruct model = {};
    model.meshes.resize(shapes.size());
    uint32_t index_offset = 0;

    for (const auto & shape : shapes)
    {
        WMeshStruct & mesh = model.meshes[index_offset++];
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

    // create an static asset
    WStaticModel * static_model = ObjectManager().CreateObject<WStaticModel>(
        "StaticModel"  // This is not used? CHECK
    );
    static_model->SetModel(std::move(model));
    static_model->SetPath(asset_path);

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

// WImportTexture
// --------------

WImportTexture::WImportTexture(WObjectManager & in_object_manager) :
    WImporter(in_object_manager) {}

std::unique_ptr<WImporter> WImportTexture::clone()
{
    return std::make_unique<WImportTexture>(*this);
}

std::vector<WAsset*> WImportTexture::Import(const char* file_path, const char* asset_path)
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

    texture_struct.data.resize(width * height * num_channels);

    std::memcpy(
        texture_struct.data.data(), 
        Pixels,
        width * height * num_channels
        // texture_struct.data.size()
    );

    stbi_image_free(Pixels);
    WTextureAsset* texture_asset = ObjectManager().CreateObject<WTextureAsset>(
        "TextureAsset"
    );

    texture_asset->SetTexture(
        std::move(texture_struct)
    );
    texture_asset->SetPath(asset_path);

    return { texture_asset };
}

std::vector<std::string> WImportTexture::Extensions()
{
    return {".png", ".jpg", ".jpeg", ".tga", ".bmp"};
}

std::vector<std::string> WImportTexture::Formats()
{
    return {"png", "jpg", "jpeg", "tga", "bmp"};
}
