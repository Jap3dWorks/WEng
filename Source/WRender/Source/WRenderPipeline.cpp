#include "WRenderPipeline.h"
#include <cassert>
#include <stdexcept>
#include "WVulkan.h"

// WRenderPipelineInfo
// -------------------

WRenderPipelineInfo::~WRenderPipelineInfo()
{
}

// WRenderPipelines
// -------------------

WRenderPipelineInfo& WRenderPipelines::CreateRenderPipeline(WRenderPipelineInfo info)
{
    if (info.pipeline != nullptr) {
        throw std::logic_error("info.pipeline must be nullptr");
    }
    if (info.pipeline_layout != nullptr) {
        throw std::logic_error("info.pipeline_layout must be nullptr");
    }
    // TODO: -Check this implementation-
    // if (info.descriptor_set_layout == nullptr) {
    //     throw std::logic_error("info.descriptor_set_layout must be not nullptr");
    // }

    // Create Vulkan Pipeline into info object
    // WVulkan::CreateVkRenderPipeline(device_, info);

    render_pipelines_[info.type].push_back(std::move(info));

    return render_pipelines_[info.type].back();
}

WDescriptorSetLayoutInfo& WRenderPipelines::CreateDescriptorSetLayout(WDescriptorSetLayoutInfo info)
{
    if (info.descriptor_set_layout != nullptr) {
        throw std::logic_error("info.descriptor_set_layout must be nullptr");
    }

    // Create Vulkan Descriptor Set Layout into info object
    WVulkan::CreateVkDescriptorSetLayout(device_, info);

    descriptor_set_layouts_.push_back(std::move(info));

    return descriptor_set_layouts_.back();
}

WRenderPipelines::~WRenderPipelines()
{
    for (auto& pipeline_type : render_pipelines_)
    {
        for (auto& pipeline : pipeline_type.second)
        {
            WVulkan::DestroyVkRenderPipeline(device_, pipeline);
        }
    }
    for(auto& descriptor_set_layout : descriptor_set_layouts_)
    {
        WVulkan::DestroyDescriptorSetLayout(device_, descriptor_set_layout);
    }
}

