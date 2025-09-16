#include "WVulkan/WVkPostprocessPipelines.hpp"
#include "WVkPostprocessPipeUtils.hpp"


void WVkPostprocessPipelines::CreatePipeline(const WAssetId & in_id,
                                             const WRenderPipelineStruct & in_pipeline_struct) {

    std::vector<WVkShaderStageInfo> shaders = pipelines_db_.BuildShaders(
        in_pipeline_struct.shaders_count,
        in_pipeline_struct.shaders,
        WVkPostprocessPipeUtils::BuildPostprocessShaderStageInfo
        );

    pipelines_db_.CreateDescSetLayout(
        in_id,
        device_info_,
        WVkPostprocessPipeUtils::UpdateDSL_PostprocessBinding
        );

    pipelines_db_.CreatePipeline(
        in_id,
        device_info_,
        in_id,
        shaders,
        WVkPostprocessPipeUtils::CreatePostprocessPipeline
        );
    
}
