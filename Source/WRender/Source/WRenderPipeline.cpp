#include "WRenderPipeline.h"
#include <cassert>
#include <stdexcept>
#include "WRenderCore.h"
#include "WVulkan.h"

// WRenderPipeline
// ---------------

WRenderPipeline::WRenderPipeline(
    const WDeviceInfo& in_device_info,
    const WDescriptorSetLayoutInfo& in_descriptor_set_layout_info,
    const WRenderPassInfo& in_render_pass_info,
    const WRenderPipelineInfo& in_pipeline_info,
    std::vector<WShaderStageInfo> in_shader_stages
    ) :
    device_info_(in_device_info),
    render_pipeline_info_(in_pipeline_info)
{
    
    shader_stage_infos_ = in_shader_stages;

    for(auto& shader_stage_info: shader_stage_infos_)
    {
	shader_stage_info.vk_shader_module = VK_NULL_HANDLE;
	WVulkan::CreateShaderModule(device_info_, shader_stage_info);
    }
    
    WVulkan::CreateVkRenderPipeline(
        device_info_,
        in_descriptor_set_layout_info,
        in_render_pass_info,
        render_pipeline_info_,
	shader_stage_infos_
    );
}

WRenderPipeline::~WRenderPipeline()
{

    for (auto& shader_stage_info : shader_stage_infos_)
    {
	WVulkan::DestroyVkShaderModule(device_info_, shader_stage_info);
    }

    WVulkan::DestroyVkRenderPipeline(device_info_, render_pipeline_info_);
}

// WRenderPipelinesManager
// -------------------

WRenderPipeline& WRenderPipelinesManager::CreateRenderPipeline(
    WRenderPipelineInfo render_pipeline_info,
    const WDescriptorSetLayoutInfo& descriptor_set_layout_info
)
{
    if (render_pipeline_info.pipeline != nullptr) {
        throw std::logic_error("render_pipeline_info.pipeline must be nullptr");
    }
    if (render_pipeline_info.pipeline_layout != nullptr) {
        throw std::logic_error("render_pipeline_info.pipeline_layout must be nullptr");
    }

    render_pipelines_[render_pipeline_info.type].emplace_back(
	device_info_,
	descriptor_set_layout_info,
	render_pass_info_,
	render_pipeline_info
	);

    return render_pipelines_[render_pipeline_info.type].back();
}

WDescriptorSetLayoutInfo& WRenderPipelinesManager::CreateDescriptorSetLayout(
    WDescriptorSetLayoutInfo descriptor_set_layout_info
)
{
    if (descriptor_set_layout_info.descriptor_set_layout != nullptr) {
        throw std::logic_error("info.descriptor_set_layout must be nullptr");
    }

    // Create Vulkan Descriptor Set Layout into info object
    WVulkan::CreateVkDescriptorSetLayout(device_info_, descriptor_set_layout_info);

    descriptor_set_layouts_.push_back(std::move(descriptor_set_layout_info));

    return descriptor_set_layouts_.back();
}

WRenderPipelinesManager::WRenderPipelinesManager(
    WDeviceInfo& device, WRenderPassInfo& render_pass_info
) : device_info_(device), render_pass_info_(render_pass_info) {}

WRenderPipelinesManager::~WRenderPipelinesManager()
{
    for(auto& descriptor_set_layout : descriptor_set_layouts_)
    {
        WVulkan::DestroyDescriptorSetLayout(device_info_, descriptor_set_layout);
    }
}

