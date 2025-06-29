#include "WVulkan/WVkRenderPipeline.hpp"
#include <cassert>
#include <stdexcept>
#include <utility>
#include <vulkan/vulkan_core.h>
#include "WCore/WCore.h"
#include "WVulkan/WVkRenderCore.h"
#include "WVulkan/WVulkan.h"
#include <cassert>

// WRenderPipeline
// ---------------

WVkRenderPipeline::WVkRenderPipeline() {}

WVkRenderPipeline::WVkRenderPipeline(
    const WVkDeviceInfo& in_device_info,
    const WVkDescriptorSetLayoutInfo& in_descriptor_set_layout_info,
    const WVkRenderPassInfo& in_render_pass_info,
    const WVkRenderPipelineInfo& in_pipeline_info,
    std::vector<WVkShaderStageInfo> in_shader_stages
    ) :
    device_info_(in_device_info),
    render_pipeline_info_(in_pipeline_info)
{

    WLOGFNAME("Create a Render Pipeline, ID: " << in_pipeline_info.wid);

    shader_stage_infos_ = in_shader_stages;

    WVulkan::Create(
        render_pipeline_info_,
        device_info_,
        in_descriptor_set_layout_info,
        in_render_pass_info,
        shader_stage_infos_
    );
}

WVkRenderPipeline::~WVkRenderPipeline()
{
    if(render_pipeline_info_.pipeline != VK_NULL_HANDLE)
    {
        WVulkan::Destroy(
            render_pipeline_info_,
            device_info_
        );
    }
}

WVkRenderPipeline::WVkRenderPipeline(WVkRenderPipeline && out_other)
{
    Move(std::move(out_other));
}

WVkRenderPipeline & WVkRenderPipeline::operator=(WVkRenderPipeline && out_other)
{
    Move(std::move(out_other));
    return *this;
}

void WVkRenderPipeline::Move(WVkRenderPipeline && out_other)
{
    render_pipeline_info_ = std::move(out_other.render_pipeline_info_);
    device_info_ = std::move(out_other.device_info_);
    shader_stage_infos_ = std::move(shader_stage_infos_);

    out_other.render_pipeline_info_ = {};
    out_other.device_info_ = {};
    out_other.shader_stage_infos_ = {};
}

WVkRenderPipelineInfo WVkRenderPipeline::RenderPipelineInfo() WCNOEXCEPT
{
    return render_pipeline_info_;
}


WId WVkRenderPipeline::WID() const
{
    return wid_;
}

void WVkRenderPipeline::WID(WId in_wid)
{
    assert(!wid_.IsValid());

    wid_ = in_wid;
}

// WRenderPipelinesManager
// -------------------

WVkRenderPipelinesManager::WVkRenderPipelinesManager()
{}

WVkRenderPipelinesManager::WVkRenderPipelinesManager(
    WVkDeviceInfo device, WVkRenderPassInfo render_pass_info
) : device_info_(device), render_pass_info_(render_pass_info)
{
    WVulkan::Create(descriptor_pool_info_, device_info_);
}

WVkRenderPipeline & WVkRenderPipelinesManager::CreateRenderPipeline(
    WVkRenderPipelineInfo in_render_pipeline_info,
    std::vector<WVkShaderStageInfo> in_shader_stage_info,
    const WVkDescriptorSetLayoutInfo & descriptor_set_layout_info
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

    WId wid{++pipelines_count_};
    render_pipelines_[in_render_pipeline_info.type].back().WID(wid);

    pipeline_bindings_[wid] = {};  // future bindings here

    return render_pipelines_[in_render_pipeline_info.type].back();
}

WVkDescriptorSetLayoutInfo & WVkRenderPipelinesManager::CreateDescriptorSetLayout()
{

    WVkDescriptorSetLayoutInfo descriptor_set_layout_info;

    WVulkan::AddDSLDefaultBindings(descriptor_set_layout_info);

    WVulkan::Create(
        descriptor_set_layout_info,
        device_info_
        );

    descriptor_set_layouts_.push_back(descriptor_set_layout_info);


    return descriptor_set_layouts_.back();
}

const WVkDescriptorSetInfo & WVkRenderPipelinesManager::CreateDescriptorSet(
    const WVkDescriptorSetLayoutInfo & in_descriptor_set_layout_info
    )
{
    WVkDescriptorSetInfo descriptor_set_info;

    WVulkan::Create(
        descriptor_set_info,
        device_info_,
        in_descriptor_set_layout_info,
        descriptor_pool_info_
        );

    descriptor_sets_.push_back(descriptor_set_info);

    return descriptor_sets_.back();
}

WVkRenderPipelinesManager::~WVkRenderPipelinesManager()
{
    Destroy();
}

WVkRenderPipelinesManager::WVkRenderPipelinesManager(
        WVkRenderPipelinesManager && other
    )
{
    Move(std::move(other));
}


WVkRenderPipelinesManager & WVkRenderPipelinesManager::operator=(WVkRenderPipelinesManager && other)
{
    Move(std::move(other));

    return *this;
}

void WVkRenderPipelinesManager::AddBinding(
    WId in_pipeline_id,
    const WVkDescriptorSetInfo & in_descriptor_set_info,
    const WVkMeshInfo & in_mesh_info
    )
{
    assert(pipeline_bindings_.contains(in_pipeline_id) && "Not contains pipeline id.");

    pipeline_bindings_[in_pipeline_id].push_back(
        {in_descriptor_set_info, in_mesh_info}
        );

}

void WVkRenderPipelinesManager::Move(WVkRenderPipelinesManager && other)
{
    device_info_ = std::move(other.device_info_);
    render_pass_info_ = std::move(other.render_pass_info_);
    render_pipelines_ = std::move(other.render_pipelines_);
    pipeline_bindings_ = std::move(other.pipeline_bindings_);
    descriptor_pool_info_ = std::move(other.descriptor_pool_info_);
    descriptor_set_layouts_ = std::move(other.descriptor_set_layouts_);

    pipelines_count_ = std::move(other.pipelines_count_);

    other.device_info_ = {};
    other.render_pass_info_ = {};
    other.descriptor_pool_info_ = {};
}

WVkRenderPipelinesManager::WPipelineDataMaps & WVkRenderPipelinesManager::RenderPipelines()noexcept
{
    return render_pipelines_;
}

const std::vector<WVkPipelineBindingInfo> & WVkRenderPipelinesManager::PipelineBindings(WId pipeline_id) const
{
    assert(pipeline_bindings_.contains(pipeline_id));

    return pipeline_bindings_.find(pipeline_id)->second;
}

void WVkRenderPipelinesManager::Destroy()
{
    pipeline_bindings_.clear();
    render_pipelines_.clear();

    if (descriptor_pool_info_.descriptor_pool != VK_NULL_HANDLE)
    {
        // Also destroys descriptor sets
        WVulkan::Destroy(descriptor_pool_info_, device_info_);
        descriptor_pool_info_.descriptor_pool = VK_NULL_HANDLE;
    }

    descriptor_sets_.clear();

    for(auto & descriptor_set_layout : descriptor_set_layouts_)
    {
        WVulkan::Destroy(
            descriptor_set_layout,
            device_info_
            );
    }

    descriptor_set_layouts_.clear();

    device_info_ = {};
    render_pass_info_ = {};
    descriptor_pool_info_ = {};
}
