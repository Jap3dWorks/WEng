#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "WRenderPipelineAsset.WEngine.hpp"

class WENGINEOBJECTS_API WRenderPipelineAsset : public WAsset {
    WOBJECT_BODY

public:

    WPROPERTY(wct::render::ERPipeType, pipeline_type, wct::render::ERPipeType::GBuffer);
    WPROPERTY(wct::render::ShaderList, shader_list,);
    WPROPERTY(wct::render::RPipeParamDescLayList, descriptor_list,);

private:

};
