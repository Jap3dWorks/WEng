#include "WImporters.h"
#include "WAssets/WStaticModel.h"
#include "WObjectManager/WObjectManager.h"
#include "WAssets/WStaticModel.h"
#include "WAssets/WTextureAsset.h"

#include <vector>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


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

    for (const auto& shape : shapes)
    {
        WMeshStruct& mesh = model.meshes[index_offset++];
        std::unordered_map<WVertexStruct, uint32_t> unique_vertices = {};

        for (const auto& index : shape.mesh.indices)
        {
            WVertexStruct vertex = {};

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
    WObjectManager & object_manager = WObjectManager::GetInstance();
    WStaticModel* static_model = object_manager.CreateObject<WStaticModel>(
        "StaticModel"
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
std::vector<WAsset*> WImportTexture::Import(const char* file_path, const char* asset_path)
{
    int width, height, channels;
    stbi_uc* Pixels = stbi_load(
        file_path,
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
    );

    if (!Pixels)
    {
        throw std::runtime_error("Failed to load texture image!");
    }

    WTextureStruct texture_struct = {};
    texture_struct.width = width;
    texture_struct.height = height;
    texture_struct.channels = ETextureChannels::kRGBA;
    texture_struct.data.resize(width * height * 4);
    std::memcpy(
        texture_struct.data.data(), 
        Pixels, 
        texture_struct.data.size()
    );

    stbi_image_free(Pixels);
    WObjectManager& object_manager = WObjectManager::GetInstance();
    WTextureAsset* texture_asset = object_manager.CreateObject<WTextureAsset>(
        "TextureAsset"
    );

    texture_asset->SetTexture(
        std::move(texture_struct)
    );
    texture_asset->SetPath(asset_path);

    return {std::move(texture_asset)};
}

std::vector<std::string> WImportTexture::Extensions()
{
    return {".png", ".jpg", ".jpeg", ".tga", ".bmp"};
}

std::vector<std::string> WImportTexture::Formats()
{
    return {"png", "jpg", "jpeg", "tga", "bmp"};
}
