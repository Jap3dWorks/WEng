#pragma once

#include "WCore/WCore.h"

struct WTextureStruct;
struct WMeshStruct;

class IRenderResources {

public:

    virtual ~IRenderResources()=default;

    WNODISCARD virtual WId RegisterTexture(const WTextureStruct & in_texture_struct)=0;

    virtual void UnregisterTexture(WId in_id)=0;

    virtual void LoadTexture(WId in_id)=0;

    virtual void UnloadTexture(WId in_id)=0;

    WNODISCARD virtual WId RegisterStaticMesh(const WMeshStruct & in_mesh_struct)=0;

    virtual void UnregisterStaticMesh(WId in_id)=0;

    virtual void LoadStaticMesh(WId in_id)=0;

    virtual void UnloadStaticMesh(WId in_id)=0;

};
