#pragma once

#include "WCore/WCore.hpp"
#include "WCore/WId.hpp"
#include "WObjects/WAsset.hpp"
#include "WCoreTypes/WRenderTypes.hpp"

#include "was::RenderPipeline.WEng.hpp"
#include <optional>

namespace was {

    class WOBJECTS_API RenderPipeline : public WAsset {

        WOBJECT_BODY;

    public:

        WPROPERTY(wct::render::ERPipeType, pipeline_type, wct::render::ERPipeType::GBuffer);
        WPROPERTY(wct::render::ShaderList, shader_list,);
        WPROPERTY(wct::render::RPipeParamDescriptorsLayout, descriptors_layout,);

    public:

        WNODISCARD
        wcr::wid::WSubIdxId
        DescriptorIndexAt(std::uint8_t set, std::uint8_t binding) {
            for(std::uint8_t i =0; i<descriptors_layout.size(); ++i)
                if (descriptors_layout[i].set == set &&
                    descriptors_layout[i].binding == binding) {
                    return i;
                }

            return wcr::wid::nullid;
        }

    };
}
