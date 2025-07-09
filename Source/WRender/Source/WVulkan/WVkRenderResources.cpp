#include "WVulkan/WVkRenderResources.hpp"
#include "WVulkan/WVkRenderCore.hpp"
#include "WVulkan/WVulkan.hpp"

WVkRenderResources::WVkRenderResources() :
    texture_collection_(),
    static_mesh_collection_()
{}

WVkRenderResources::~WVkRenderResources() {
    texture_collection_.Clear();
    static_mesh_collection_.Clear();
}

WVkRenderResources::WVkRenderResources(
    const WVkDeviceInfo & in_device_info,
    const WVkCommandPoolInfo & in_command_pool_info
    )  :
    texture_collection_(
        [this,
         in_device_info,
         in_command_pool_info](const WId & in_id) -> WVkTextureInfo {
            WVkTextureInfo result;
            WVulkan::Create(
                result,
                texture_collection_.GetAsset(in_id).GetTexture(),
                in_device_info,
                in_command_pool_info
                );
            
            return result;
        },
        [this,
         in_device_info](WVkTextureInfo & in_info) -> void {
            WVulkan::Destroy(
                in_info,
                in_device_info
                );
        }
        ),
    static_mesh_collection_(
        [this,
         in_device_info,
         in_command_pool_info] (const WId & in_id) -> WVkMeshInfo {
            WVkMeshInfo result;
            WVulkan::Create(
                result,
                static_mesh_collection_.GetAsset(in_id).GetMesh(),
                in_device_info,
                in_command_pool_info
                );
            return result;
        },
        [this,
         in_device_info] (WVkMeshInfo & in_mesh_info) -> void {
            WVulkan::Destroy(
                in_mesh_info,
                in_device_info
                );
        }
        ) {}

WVkRenderResources::WVkRenderResources(WVkRenderResources && other) :
    texture_collection_(std::move(other.texture_collection_)),
    static_mesh_collection_(std::move(other.static_mesh_collection_))
{}

WVkRenderResources & WVkRenderResources::operator=(WVkRenderResources && other) {
    if (this != &other) {
        texture_collection_ = std::move(other.texture_collection_);
        static_mesh_collection_ = std::move(other.static_mesh_collection_);
    }

    return *this;
}

void WVkRenderResources::RegisterTexture(TRef<WTextureAsset> in_texture_asset) {
    return texture_collection_.RegisterAsset(in_texture_asset.Get());
}

void WVkRenderResources::UnregisterTexture(WId in_id) {
    texture_collection_.UnregisterAsset(in_id);
}

void WVkRenderResources::LoadTexture(WId in_id) {
    texture_collection_.LoadAsset(in_id);
}

void WVkRenderResources::UnloadTexture(WId in_id) {
    texture_collection_.UnloadAsset(in_id);
}

void WVkRenderResources::RegisterStaticMesh(TRef<WStaticMeshAsset> in_static_mesh_asset) {
    return static_mesh_collection_.RegisterAsset(in_static_mesh_asset.Get());
}

void WVkRenderResources::UnregisterStaticMesh(WId in_id) {
    static_mesh_collection_.UnregisterAsset(in_id);
}

void WVkRenderResources::LoadStaticMesh(WId in_id) {
    static_mesh_collection_.LoadAsset(in_id);
}

void WVkRenderResources::UnloadStaticMesh(WId in_id) {
    static_mesh_collection_.UnloadAsset(in_id);
}

const WVkTextureInfo & WVkRenderResources::TextureInfo(WId in_id) const {
    return texture_collection_.GetData(in_id);
}

const WVkMeshInfo & WVkRenderResources::StaticMeshInfo(WId in_id) const {
    return static_mesh_collection_.GetData(in_id);
}
