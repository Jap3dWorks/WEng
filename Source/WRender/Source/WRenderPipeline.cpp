#include "WRenderPipeline.h"
#include <cassert>
#include <stdexcept>

// WRenderPipelineInfo
// -------------------

WRenderPipelineInfo::~WRenderPipelineInfo()
{
}

// WRenderPipelines
// -------------------

WRenderPipelineInfo* WRenderPipelines::Create(WRenderPipelineInfo info)
{
    if (info.pipeline != nullptr) {
        throw std::logic_error("info.pipeline must be nullptr");
    }

    // Create Vulkan Pipeline into info object
    // info->device = &Device_;
    WVulkanPipeline::Create(device_, info);

    pipelines_[info.type].push_back(std::move(info));

    return &pipelines_[info.type].back();
}

WRenderPipelines::~WRenderPipelines()
{
    for (auto& pipeline_type : pipelines_)
    {
        for (auto& pipeline : pipeline_type.second)
        {
            if (pipeline.pipeline_layout)
            {
                // destroy pipeline layout
                vkDestroyPipelineLayout(
                    device_.vk_device,
                    pipeline.pipeline_layout,
                    nullptr
                );
            }

            if (pipeline.pipeline)
            {   
                // destroy pipeline
                vkDestroyPipeline(
                    device_.vk_device,
                    pipeline.pipeline_layout,
                    nullptr
                );
            }
        }
    }
}

// WVulkanPipeline 
// ---------------

void WVulkanPipeline::Create(WDevice Device, WRenderPipelineInfo& out_pipeline_info)
{
    // Create Shader Stages
    std::vector<VkPipelineShaderStageCreateInfo> ShaderStages(
        out_pipeline_info.shaders.size()
    );

    WShaderStage *vertex_shader_stage = nullptr;

    for (uint32_t i = 0; i < out_pipeline_info.shaders.size(); i++)
    {
        ShaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStages[i].stage = WVulkan::ToShaderStageFlagBits(
            out_pipeline_info.shaders[i].type
        );
        
        if(out_pipeline_info.shaders[i].type == WShaderType::Vertex)
        {
            vertex_shader_stage = &out_pipeline_info.shaders[i];
        }

        ShaderStages[i].module = out_pipeline_info.shaders[i].vk_shader_module;
        ShaderStages[i].pName = out_pipeline_info.shaders[i].entry_point.c_str();
    }

    if (vertex_shader_stage == nullptr)
    {
        throw std::runtime_error("Vertex shader stage not found!");
    }

    VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
    VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(
        vertex_shader_stage->binding_descriptors.size()
    );
    VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>( 
        vertex_shader_stage->attribute_descriptors.size()
    );
    VertexInputInfo.pVertexBindingDescriptions = 
        vertex_shader_stage->binding_descriptors.data();
    VertexInputInfo.pVertexAttributeDescriptions = 
        vertex_shader_stage->attribute_descriptors.data();

    VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
    InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    InputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo ViewportState{};
    ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportState.viewportCount = 1;
    ViewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo Rasterizer{};
    Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    Rasterizer.depthClampEnable = VK_FALSE;
    Rasterizer.rasterizerDiscardEnable = VK_FALSE;
    Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    Rasterizer.lineWidth = 1.0f;
    Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // because of y-flip in the projection matrix
    Rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo Multisampling{};
    Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    Multisampling.sampleShadingEnable = VK_FALSE;
    Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

}