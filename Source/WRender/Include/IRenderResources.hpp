#pragma once

#include "WCore/WCore.hpp"

class WTextureAsset;
class WStaticMeshAsset;

class IRenderResources {

public:

    virtual ~IRenderResources()=default;

    virtual void RegisterTexture(WTextureAsset & in_texture_asset)=0;

    virtual void UnregisterTexture(WId in_id)=0;

    virtual void LoadTexture(WId in_id)=0;

    virtual void UnloadTexture(WId in_id)=0;

    virtual void RegisterStaticMesh(WStaticMeshAsset & in_static_mesh_asset)=0;

    virtual void UnregisterStaticMesh(WId in_id)=0;

    virtual void LoadStaticMesh(WId in_id)=0;

    virtual void UnloadStaticMesh(WId in_id)=0;

};
