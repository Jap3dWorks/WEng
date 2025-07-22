#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WComponent.hpp"

#include "WStaticMeshComponent.WEngine.hpp"

class WENGINEOBJECTS_API WStaticMeshComponent : public WComponent {

    WOBJECT_BODY(WStaticMeshComponent)

public:

    WId StaticMeshAsset();

    void StaticMeshAsset(const WId& in_id);

private:

    WId static_mesh_asset_;
  
};


