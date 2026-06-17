#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "WRenderPipelineAsset.WEngine.hpp"

WCLASS()
class WENGINEOBJECTS_API WRenderPipelineAsset : public WAsset {
    WOBJECT_BODY

public:

    wct::render::WRenderPipelineStruct & RenderPipeline() {
        return pipeline_;
    }

private:

    wct::render::WRenderPipelineStruct pipeline_{};

};
