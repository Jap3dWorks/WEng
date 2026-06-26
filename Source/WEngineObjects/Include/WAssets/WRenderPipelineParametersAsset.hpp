#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "WRenderPipelineParametersAsset.WEngine.hpp"

class WENGINEOBJECTS_API WRenderPipelineParametersAsset : public WAsset {

    WOBJECT_BODY;

public:

    WPROPERTY(wct::render::WRPParameterList_Ubo, ubo_list,);
    WPROPERTY(wct::render::WRPParameterList_WAssetId, texture_list,);

public:

private:

};

