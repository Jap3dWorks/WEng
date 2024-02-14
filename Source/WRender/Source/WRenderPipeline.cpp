#include "WRenderPipeline.h"
#include <cassert>
#include <stdexcept>
#include "WVulkan.h"


// WRenderPipelinesManager
// -------------------

WRenderPipelineInfo& WRenderPipelinesManager::CreateRenderPipeline(
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

    // Create Vulkan Pipeline into info object
    WVulkan::CreateVkRenderPipeline(
        device_info_,
        descriptor_set_layout_info,
        render_pass_info_,
        render_pipeline_info
    );

    render_pipelines_[render_pipeline_info.type].push_back(std::move(render_pipeline_info));

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
    for (auto& pipeline_type : render_pipelines_)
    {
        for (auto& pipeline : pipeline_type.second)
        {
            WVulkan::DestroyVkRenderPipeline(device_info_, pipeline);
        }
    }
    for(auto& descriptor_set_layout : descriptor_set_layouts_)
    {
        WVulkan::DestroyDescriptorSetLayout(device_info_, descriptor_set_layout);
    }
}

