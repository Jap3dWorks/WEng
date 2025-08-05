#include "WVulkan/WVkRenderResources.hpp"
#include "WVulkan/WVkRenderStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include <vector>

WVkRenderResources::WVkRenderResources()=default;

WVkRenderResources::~WVkRenderResources()=default;

WVkRenderResources::WVkRenderResources(
    const WVkDeviceInfo & in_device_info,
    const WVkCommandPoolInfo & in_command_pool_info
    )  :
    device_info_(in_device_info),
    command_pool_info_(in_command_pool_info),
    texture_collection_(),
    static_mesh_collection_()
{
    InitializeStaticMeshFunctions();
    InitializeTextureFunctions();
}

WVkRenderResources::WVkRenderResources(WVkRenderResources && other) :
    device_info_(std::move(other.device_info_)),
    command_pool_info_(std::move(other.command_pool_info_)),
    texture_collection_(std::move(other.texture_collection_)),
    static_mesh_collection_(std::move(other.static_mesh_collection_))
{
    InitializeStaticMeshFunctions();
    InitializeTextureFunctions();

    other.device_info_ = {};
    other.command_pool_info_ = {};
}

WVkRenderResources & WVkRenderResources::operator=(WVkRenderResources && other) {
    if (this != &other) {
        device_info_ = std::move(other.device_info_);
        command_pool_info_ = std::move(other.command_pool_info_);
        texture_collection_ = std::move(other.texture_collection_);
        static_mesh_collection_ = std::move(other.static_mesh_collection_);
        
        InitializeStaticMeshFunctions();
        InitializeTextureFunctions();

        other.device_info_ = {};
        other.command_pool_info_ = {};
    }

    return *this;
}

void WVkRenderResources::RegisterTexture(WTextureAsset & in_texture_asset) {
    return texture_collection_.RegisterAsset(in_texture_asset);
}

void WVkRenderResources::UnregisterTexture(const  WAssetId & in_id) {
    texture_collection_.UnregisterAsset(in_id);
}

void WVkRenderResources::LoadTexture(const WAssetId & in_id) {
    texture_collection_.LoadAsset(in_id);
}

void WVkRenderResources::UnloadTexture(const WAssetId & in_id) {
    texture_collection_.UnloadAsset(in_id);
}

void WVkRenderResources::RegisterStaticMesh(WStaticMeshAsset & in_static_mesh_asset) {
    return static_mesh_collection_.RegisterAsset(in_static_mesh_asset);
}

void WVkRenderResources::UnregisterStaticMesh(const WAssetId & in_id) {
    static_mesh_collection_.UnregisterAsset(in_id);
}

void WVkRenderResources::LoadStaticMesh(const WAssetId & in_id) {
    static_mesh_collection_.LoadAsset(in_id);
}

void WVkRenderResources::UnloadStaticMesh(const WAssetId & in_id) {
    static_mesh_collection_.UnloadAsset(in_id);
}

const WVkTextureInfo & WVkRenderResources::TextureInfo(const WAssetId & in_id) const {
    return texture_collection_.GetData(in_id);
}

const WVkMeshInfo & WVkRenderResources::StaticMeshInfo(const WAssetId & in_id) const {
    return static_mesh_collection_.GetData(in_id);
}

void WVkRenderResources::Clear() {
    texture_collection_.Clear();
    static_mesh_collection_.Clear();    
}

void WVkRenderResources::InitializeTextureFunctions() {
    texture_collection_.SetCreateFn(
        [this](const WAssetId & in_id) -> WVkTextureInfo {
            WVkTextureInfo result;
            WVulkan::Create(
                result,
                texture_collection_.GetAsset(in_id).GetTexture(),
                device_info_,
                command_pool_info_
                );
            
            return result;
        }
        );

    texture_collection_.SetClearFn(
        [this](WVkTextureInfo & in_info) -> void {
            WVulkan::Destroy(
                in_info,
                device_info_
                );
        }
        );
}

void WVkRenderResources::InitializeStaticMeshFunctions() {
    static_mesh_collection_.SetCreateFn(
        [this] (const WAssetId & in_id) -> WVkMeshInfo {
            WVkMeshInfo result;
            WVulkan::Create(
                result,
                static_mesh_collection_.GetAsset(in_id).GetMesh(),
                device_info_,
                command_pool_info_
                );
            return result;
        });
    
    static_mesh_collection_.SetClearFn(
        [this] (WVkMeshInfo & in_mesh_info) -> void {
            WVulkan::Destroy(
                in_mesh_info,
                device_info_
                );
        });    
}
