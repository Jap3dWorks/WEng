#include "WRenderPipeline.h"
#include <cassert>
#include <stdexcept>
#include <utility>
#include <vulkan/vulkan_core.h>
#include "WCore/WCore.h"
#include "WRenderCore.h"
#include "WVulkan.h"

// WRenderPipeline
// ---------------

WRenderPipeline::WRenderPipeline() {}

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

    WVulkan::Create(
        render_pipeline_info_,
        device_info_,
        in_descriptor_set_layout_info,
        in_render_pass_info,
        shader_stage_infos_
    );
}

WRenderPipeline::~WRenderPipeline()
{
    if(render_pipeline_info_.pipeline != VK_NULL_HANDLE)
    {
        WVulkan::Destroy(
            render_pipeline_info_,
            device_info_
        );
    }
}

WRenderPipeline::WRenderPipeline(WRenderPipeline && out_other)
{
    Move(std::move(out_other));
}

WRenderPipeline & WRenderPipeline::operator=(WRenderPipeline && out_other)
{
    Move(std::move(out_other));
    return *this;
}

void WRenderPipeline::Move(WRenderPipeline && out_other)
{
    render_pipeline_info_ = std::move(out_other.render_pipeline_info_);
    device_info_ = std::move(out_other.device_info_);
    shader_stage_infos_ = std::move(shader_stage_infos_);

    out_other.render_pipeline_info_ = {};
    out_other.device_info_ = {};
    out_other.shader_stage_infos_ = {};
}

WRenderPipelineInfo WRenderPipeline::RenderPipelineInfo() WCNOEXCEPT
{
    return render_pipeline_info_;
}


WId WRenderPipeline::WID() const
{
    return wid_;
}

void WRenderPipeline::WID(WId in_wid)
{
    wid_ = in_wid;
}

// WRenderPipelinesManager
// -------------------

WRenderPipelinesManager::WRenderPipelinesManager()
{}

WRenderPipelinesManager::WRenderPipelinesManager(
    WDeviceInfo& device, WRenderPassInfo& render_pass_info
) : device_info_(device), render_pass_info_(render_pass_info)
{
    WVulkan::Create(descriptor_pool_info_, device_info_);
}

WRenderPipeline & WRenderPipelinesManager::CreateRenderPipeline(
    WRenderPipelineInfo in_render_pipeline_info,
    std::vector<WShaderStageInfo> in_shader_stage_info,
    const WDescriptorSetLayoutInfo& descriptor_set_layout_info
)
{
    assert(
        in_render_pipeline_info.pipeline == VK_NULL_HANDLE &&
        "render_pipeline_info.pipeline must be nullptr"
    );
    assert(
        in_render_pipeline_info.pipeline_layout == VK_NULL_HANDLE &&
        "render_pipeline_info.pipeline_layout must be nullptr"
    );

    render_pipelines_[in_render_pipeline_info.type].emplace_back(
        device_info_,
        descriptor_set_layout_info,
        render_pass_info_,
        in_render_pipeline_info,
        in_shader_stage_info
    );

    WId wid{1}; // TODO: make this ok
    render_pipelines_[in_render_pipeline_info.type].back().WID(wid);

    return render_pipelines_[in_render_pipeline_info.type].back();
}

WDescriptorSetLayoutInfo & WRenderPipelinesManager::CreateDescriptorSet()
{

    WDescriptorSetLayoutInfo descriptor_set_layout_info;

    WVulkan::AddDSLDefaultBindings(descriptor_set_layout_info);

    WVulkan::Create(
        descriptor_set_layout_info,
        device_info_
        );

    descriptor_set_layouts_.push_back(descriptor_set_layout_info);

    WDescriptorSetInfo descriptor_set_info;

    WVulkan::Create(
        descriptor_set_info,
        device_info_,
        descriptor_set_layout_info,
        descriptor_pool_info_
        );

    descriptor_sets_.push_back(descriptor_set_info);

    return descriptor_set_layouts_.back();
}


WRenderPipelinesManager::~WRenderPipelinesManager()
{
    for(auto & descriptor_set_layout : descriptor_set_layouts_)
    {
        WVulkan::Destroy(
	    descriptor_set_layout,
	    device_info_
	    );
    }
}

WRenderPipelinesManager::WRenderPipelinesManager(
        WRenderPipelinesManager && other
    )
{
    Move(std::move(other));
}


WRenderPipelinesManager & WRenderPipelinesManager::operator=(WRenderPipelinesManager && other)
{
    Move(std::move(other));

    return *this;
}

void WRenderPipelinesManager::AddBinding(WId in_pipeline_id, WPipelineBinding in_pipeline_binding)
{
    assert(pipeline_bindings_.contains(in_pipeline_id) && "Not contains pipeline id.");

    pipeline_bindings_[in_pipeline_id].push_back(
        in_pipeline_binding
        );
}

void WRenderPipelinesManager::Move(WRenderPipelinesManager && out_other)
{
    device_info_ = std::move(out_other.device_info_);
    render_pass_info_ = std::move(out_other.render_pass_info_);
    render_pipelines_ = std::move(out_other.render_pipelines_);

    out_other.device_info_ = {};
    out_other.render_pass_info_ = {};
}

WRenderPipelinesManager::WPipelineDataMaps & WRenderPipelinesManager::RenderPipelines()noexcept
{
    return render_pipelines_;
}

const std::vector<WPipelineBinding> & WRenderPipelinesManager::PipelineBindings(WId pipeline_id) const
{
    assert(pipeline_bindings_.contains(pipeline_id));

    return pipeline_bindings_.find(pipeline_id)->second;
}
