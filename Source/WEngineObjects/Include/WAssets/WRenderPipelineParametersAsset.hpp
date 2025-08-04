#pragma once

#include "WCore/WCore.hpp"

#include "WEngineObjects/WAsset.hpp"
#include "WStructs/WRenderStructs.hpp"

#include "WRenderPipelineParametersAsset.WEngine.hpp"

WCLASS()
class WENGINEOBJECTS_API WRenderPipelineParametersAsset : public WAsset {
    WOBJECT_BODY(WRenderPipelineParametersAsset)

public:

    WRenderPipelineParametersStruct & RenderPipelineParameters() {
        return pipeline_parameters_;
    }

private:

    WRenderPipelineParametersStruct pipeline_parameters_{};

};

