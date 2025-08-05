#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WComponent.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WStructs/WComponentStructs.hpp"

#include "WStaticMeshComponent.WEngine.hpp"

class WENGINEOBJECTS_API WStaticMeshComponent : public WComponent {

    WOBJECT_BODY(WStaticMeshComponent)

public:

    WStaticMeshStruct & StaticMeshStruct() { return static_mesh_struct_; }

    const WStaticMeshStruct & StaticMeshStruct() const { return static_mesh_struct_; }

private:

    WStaticMeshStruct static_mesh_struct_{};
  
};


