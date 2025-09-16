#pragma once

#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkanStructs.hpp"
#include "WVulkan/WVkPipelinesBase.hpp"

class WVkPostprocessPipelines :
    public WVkPipelinesBase<WAssetId, WEntityComponentId, WENG_MAX_FRAMES_IN_FLIGHT>
{
public:

    void CreatePipeline(const WAssetId & in_id,
                        const WRenderPipelineStruct & in_pipeline_struct);

    WEntityComponentId CreateBinding(const WEntityComponentId & in_binding_id,
                                     const WAssetId & in_pipeline_id,
                                     std::vector<WVkTextureInfo> in_textures,
                                     std::vector<std::uint16_t> in_texture_bindings);

private:

};


