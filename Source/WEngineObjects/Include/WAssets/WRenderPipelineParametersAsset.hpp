#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "WRenderPipelineParametersAsset.WEngine.hpp"

WCLASS()
class WENGINEOBJECTS_API WRenderPipelineParametersAsset : public WAsset {
    WOBJECT_BODY

public:

    wct::render::WRenderPipelineParameters & RenderPipelineParameters() {
        return pipeline_parameters_;
    }

private:

    wct::render::WRenderPipelineParameters pipeline_parameters_{};

};

