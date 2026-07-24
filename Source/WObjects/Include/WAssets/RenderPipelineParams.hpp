#pragma once

#include "WCore/WCore.hpp"

#include "WObjects/WAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "was::RenderPipelineParams.WEng.hpp"

namespace was {

class WOBJECTS_API RenderPipelineParams : public WAsset {

    WOBJECT_BODY;

public:

    WPROPERTY(wct::render::RPipeParamList_Ubo, ubo_list,);
    WPROPERTY(wct::render::RPipeParamList_WAssetId, texture_list,);

public:

private:

};

}
