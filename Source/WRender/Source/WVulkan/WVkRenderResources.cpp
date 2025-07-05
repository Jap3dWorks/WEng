#include "WVulkan/WVkRenderResources.hpp"

WVkRenderResources::WVkRenderResources() :
    texture_collection(),
    static_mesh_collection() {}

WVkRenderResources::~WVkRenderResources() {
    texture_collection={};
    static_mesh_collection={};
}

WVkRenderResources::WVkRenderResources(
    const WVkDeviceInfo & in_device_info,
    const WVkCommandPoolInfo & in_command_pool_info) :
    texture_collection(in_device_info, in_command_pool_info),
    static_mesh_collection(in_device_info, in_command_pool_info) {}


WVkRenderResources::WVkRenderResources(WVkRenderResources && other) {
    Move(std::move(other));
}

WVkRenderResources & WVkRenderResources::operator=(WVkRenderResources && other) {
    Move(std::move(other));

    return *this;
}

void WVkRenderResources::Move(WVkRenderResources && other) {
    texture_collection = std::move(other.texture_collection);
    static_mesh_collection = std::move(other.static_mesh_collection);
}

void WVkRenderResources::RegisterTexture(TRef<WTextureAsset> in_texture_asset) {
    return texture_collection.RegisterAsset(in_texture_asset.Get());
}

void WVkRenderResources::UnregisterTexture(WId in_id) {
    texture_collection.UnregisterAsset(in_id);
}

void WVkRenderResources::LoadTexture(WId in_id) {
    texture_collection.LoadAsset(in_id);
}

void WVkRenderResources::UnloadTexture(WId in_id) {
    texture_collection.UnloadAsset(in_id);
}

void WVkRenderResources::RegisterStaticMesh(TRef<WStaticMeshAsset> in_static_mesh_asset) {
    return static_mesh_collection.RegisterAsset(in_static_mesh_asset.Get());
}

void WVkRenderResources::UnregisterStaticMesh(WId in_id) {
    static_mesh_collection.UnregisterAsset(in_id);
}

void WVkRenderResources::LoadStaticMesh(WId in_id) {
    static_mesh_collection.LoadAsset(in_id);
}

void WVkRenderResources::UnloadStaticMesh(WId in_id) {
    static_mesh_collection.UnloadAsset(in_id);
}

const WVkTextureInfo & WVkRenderResources::TextureInfo(WId in_id) const {
    return texture_collection.GetData(in_id);
}

const WVkMeshInfo & WVkRenderResources::StaticMeshInfo(WId in_id) const {
    return static_mesh_collection.GetData(in_id);
}
