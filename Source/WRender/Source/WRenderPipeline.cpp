#include "WRenderPipeline.h"
#include <cassert>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
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
    std::vector<WShaderModule> shader_modules(in_shader_stages.size());

    shader_stage_infos_ = in_shader_stages;

    for(int i=0; i< shader_stage_infos_.size(); i++)
    {
	shader_modules[i] = WVulkan::CreateShaderModule(
	    shader_stage_infos_[i],
	    device_info_
	    );
    }
    
    WVulkan::Create(
        render_pipeline_info_,
        device_info_,
        in_descriptor_set_layout_info,
        in_render_pass_info,
	shader_stage_infos_,
	shader_modules
    );

    for (auto& shader_module : shader_modules)
    {
	WVulkan::DestroyVkShaderModule(
	    shader_module,
	    device_info_
	    );
    }
}

WRenderPipeline::~WRenderPipeline()
{
    WVulkan::DestroyVkRenderPipeline(device_info_, render_pipeline_info_);
}

// WRenderPipelinesManager
// -------------------

WRenderPipeline& WRenderPipelinesManager::CreateRenderPipeline(
    WRenderPipelineInfo in_render_pipeline_info,
    std::vector<WShaderStageInfo> in_shader_stage_info,
    const WDescriptorSetLayoutInfo& descriptor_set_layout_info
)
{
    assert(
	in_render_pipeline_info.pipeline != VK_NULL_HANDLE &&
	"render_pipeline_info.pipeline must be nullptr"
	);
    assert(in_render_pipeline_info.pipeline_layout != VK_NULL_HANDLE &&
        "render_pipeline_info.pipeline_layout must be nullptr"
	);

    render_pipelines_[in_render_pipeline_info.type].emplace_back(
	device_info_,
	descriptor_set_layout_info,
	render_pass_info_,
	in_render_pipeline_info,
	in_shader_stage_info
	);

    return render_pipelines_[in_render_pipeline_info.type].back();
}

WDescriptorSetLayoutInfo& WRenderPipelinesManager::CreateDescriptorSetLayout(
    WDescriptorSetLayoutInfo descriptor_set_layout_info
)
{
    if (descriptor_set_layout_info.descriptor_set_layout != nullptr) {
        throw std::logic_error("info.descriptor_set_layout must be nullptr");
    }

    // Create Vulkan Descriptor Set Layout into info object
    WVulkan::Create(
	descriptor_set_layout_info,
	device_info_
	);

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

