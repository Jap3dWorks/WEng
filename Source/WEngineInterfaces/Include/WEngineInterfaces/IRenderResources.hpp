#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"

class WTextureAsset;
class WStaticMeshAsset;

/**
 * @brief Interface for classes that manage the lifetime of render resources like geometries or textures.
 */
class IRenderResources {

public:
    
    virtual ~IRenderResources()=default;

    virtual void RegisterTexture(TRef<WTextureAsset> in_texture_asset)=0;

    virtual void UnregisterTexture(WId in_id)=0;

    virtual void LoadTexture(WId in_id)=0;

    virtual void UnloadTexture(WId in_id)=0;

    virtual void RegisterStaticMesh(TRef<WStaticMeshAsset> in_static_mesh_asset)=0;

    virtual void UnregisterStaticMesh(WId in_id)=0;

    virtual void LoadStaticMesh(WId in_id)=0;

    virtual void UnloadStaticMesh(WId in_id)=0;

};
