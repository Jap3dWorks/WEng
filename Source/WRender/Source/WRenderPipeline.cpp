#include "WVulkan/WVkRenderPipeline.hpp"
#include "WCore/WCore.h"
#include "WVulkan/WVkRenderCore.h"
#include "WVulkan/WVulkan.h"
#include "WLog.h"

#include <cassert>
#include <cassert>
// #include <stdexcept>
#include <utility>
#include <vulkan/vulkan_core.h>

// WRenderPipelinesManager
// -------------------

WVkRenderPipelinesManager::WVkRenderPipelinesManager()
{}

WVkRenderPipelinesManager::WVkRenderPipelinesManager(
    WVkDeviceInfo device, WVkRenderPassInfo render_pass_info
) : device_info_(device), render_pass_info_(render_pass_info)
{
    Initialize();
}

WId WVkRenderPipelinesManager::CreateRenderPipeline(
    WVkRenderPipelineInfo in_render_pipeline_info,
    std::vector<WVkShaderStageInfo> in_shader_stages,
    WId in_descriptor_set_layout_id
    )
{
    assert(in_render_pipeline_info.pipeline == VK_NULL_HANDLE);
    assert(in_render_pipeline_info.pipeline_layout == VK_NULL_HANDLE);

    auto& d_set_layout = descriptor_set_layouts_.Get(in_descriptor_set_layout_id);

    WId pid = render_pipelines_.Create(
        [this,
         &in_render_pipeline_info,
         &d_set_layout,
         &in_shader_stages](WId in_id) -> auto {
            WVulkan::Create(
                in_render_pipeline_info,
                device_info_,
                d_set_layout,
                render_pass_info_,
                in_shader_stages);
            
            in_render_pipeline_info.wid = in_id;
            return in_render_pipeline_info;
        }
        );

    return pid;
}

WId WVkRenderPipelinesManager::CreateDescriptorSetLayout() {

    WVkDescriptorSetLayoutInfo descriptor_set_layout_info;

    WVulkan::AddDSLDefaultBindings(descriptor_set_layout_info);

    return descriptor_set_layouts_.Create(
        [this,
         &descriptor_set_layout_info](WId _in_id) -> auto {
        WVulkan::Create(
            descriptor_set_layout_info,
            device_info_
            );

        descriptor_set_layout_info.wid = _in_id;
        return descriptor_set_layout_info;
    });
}

WId WVkRenderPipelinesManager::CreateDescriptorSet(
    WId in_descriptor_set_layout_id
    )
{
    auto & d_set_layout = descriptor_set_layouts_.Get(in_descriptor_set_layout_id);
    WVkDescriptorSetInfo descriptor_set_info;

    return descriptor_sets_.Create(
        [this,
         &descriptor_set_info,
         &d_set_layout]
        (WId in_id) {
            WVulkan::Create(
                descriptor_set_info,
                device_info_,
                d_set_layout,
                descriptor_pool_info_
                );
            
            descriptor_set_info.wid = in_id;
            return descriptor_set_info;
        }
        );
}

WVkRenderPipelinesManager::~WVkRenderPipelinesManager()
{
    Destroy();
}

WVkRenderPipelinesManager::WVkRenderPipelinesManager(
        WVkRenderPipelinesManager && other
    ) noexcept
{
    Move(std::move(other));
}


WVkRenderPipelinesManager & WVkRenderPipelinesManager::operator=(WVkRenderPipelinesManager && other) noexcept
{
    Move(std::move(other));

    return *this;
}

WId WVkRenderPipelinesManager::AddBinding(
    WId in_pipeline_id,
    WId in_descriptor_id,
    const WVkMeshInfo & in_mesh_info
    )
{
    assert(pipeline_bindings_.Contains(in_pipeline_id));

    // TODO: Rethink this

    return pipeline_bindings_.Create(
        [&in_pipeline_id,
         &in_descriptor_id,
         &in_mesh_info
            ](WId in_id) {
            return { {
                    in_id,
                    in_pipeline_id,
                    in_descriptor_id,
                    in_mesh_info
                }
            };
        }
        );
}

void WVkRenderPipelinesManager::Move(WVkRenderPipelinesManager && other)
{
    device_info_ = std::move(other.device_info_);
    render_pass_info_ = std::move(other.render_pass_info_);
    descriptor_pool_info_ = std::move(other.descriptor_pool_info_);

    render_pipelines_ = std::move(other.render_pipelines_);
    descriptor_set_layouts_ = std::move(other.descriptor_set_layouts_);
    descriptor_sets_ = std::move(other.descriptor_sets_);
    pipeline_bindings_ = std::move(other.pipeline_bindings_);
    stage_pipelines_ = std::move(other.stage_pipelines_);

    other.device_info_ = {};
    other.render_pass_info_ = {};
    other.descriptor_pool_info_ = {};
}

std::vector<WId> & WVkRenderPipelinesManager::StagePipelines(EPipelineType in_type) WNOEXCEPT {
    return stage_pipelines_[in_type];
}

const std::vector<WVkPipelineBindingInfo> & WVkRenderPipelinesManager::PipelineBindings(WId pipeline_id) const
{
    assert(pipeline_bindings_.Contains(pipeline_id));

    return pipeline_bindings_.Get(pipeline_id);
}

void WVkRenderPipelinesManager::Destroy()
{
    pipeline_bindings_.Clear();
    render_pipelines_.Clear();

    if (descriptor_pool_info_.descriptor_pool != VK_NULL_HANDLE)
    {
        // Also destroys descriptor sets
        WVulkan::Destroy(descriptor_pool_info_, device_info_);
        descriptor_pool_info_.descriptor_pool = VK_NULL_HANDLE;
    }

    descriptor_sets_.Clear();
    descriptor_set_layouts_.Clear();

    device_info_ = {};
    render_pass_info_ = {};
    descriptor_pool_info_ = {};
}

void WVkRenderPipelinesManager::Initialize() {

    WVulkan::Create(descriptor_pool_info_, device_info_);

    render_pipelines_.SetDestroyFn([this](auto & p) {
        WVulkan::Destroy(
            p,
            device_info_
            );
    });

    descriptor_set_layouts_.SetDestroyFn([this](auto & d) {
        WVulkan::Destroy(
            d,
            device_info_
            );
    });
}
