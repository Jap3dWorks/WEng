#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "WRenderPipelineParametersAsset.WEngine.hpp"

WCLASS()
class WENGINEOBJECTS_API WRenderPipelineParametersAsset : public WAsset {
    WOBJECT_BODY(WRenderPipelineParametersAsset)

public:

    wct::render::WRenderPipelineParametersStruct & RenderPipelineParameters() {
        return pipeline_parameters_;
    }

private:

    wct::render::WRenderPipelineParametersStruct pipeline_parameters_{};

};

