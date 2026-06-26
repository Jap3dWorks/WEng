#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "WRenderPipelineAsset.WEngine.hpp"

class WENGINEOBJECTS_API WRenderPipelineAsset : public WAsset {
    WOBJECT_BODY

public:

    WPROPERTY(wct::render::EPipelineType, pipeline_type, wct::render::EPipelineType::GBuffer);
    WPROPERTY(wct::render::WShaderList, shader_list,);
    WPROPERTY(wct::render::WPipeParamDescriptorList, descriptor_list,);

private:

};
