#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WStructs/WRenderStructs.hpp"

#include "WRenderPipelineAsset.WEngine.hpp"

WCLASS()
class WENGINEOBJECTS_API WRenderPipelineAsset : public WAsset {
    WOBJECT_BODY(WRenderPipelineAsset)

public:

    WRenderPipelineStruct & RenderPipeline() {
        return pipeline_;
    }

private:

    WRenderPipelineStruct pipeline_{};

};
