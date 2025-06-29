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

WId WVkRenderResources::RegisterTexture(const WTextureStruct & in_texture_struct) {
    return texture_collection.RegisterAsset(in_texture_struct);
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

WId WVkRenderResources::RegisterStaticMesh(const WMeshStruct & in_mesh_struct) {
    return static_mesh_collection.RegisterAsset(in_mesh_struct);
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

WVkTextureInfo WVkRenderResources::TextureInfo(WId in_id) {
    return texture_collection.GetData(in_id);
}

WVkMeshInfo WVkRenderResources::StaticMeshInfo(WId in_id) {
    return static_mesh_collection.GetData(in_id);
}
