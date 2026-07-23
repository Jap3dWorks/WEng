#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "WRenderPipelineParametersAsset.WEng.hpp"

class WENGINEOBJECTS_API WRenderPipelineParametersAsset : public WAsset {

    WOBJECT_BODY;

public:

    WPROPERTY(wct::render::RPipeParamList_Ubo, ubo_list,);
    WPROPERTY(wct::render::RPipeParamList_WAssetId, texture_list,);

public:

private:

};

