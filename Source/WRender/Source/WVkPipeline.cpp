#include "WVulkan/WVk/WVkPipeline.hpp"

void wvk::pipeline::Destroy(
    WVkRenderPipeline & pipeline_info,
    const VkDevice & in_device
    )
{
    if (pipeline_info.pipeline_layout)
    {
        // destroy pipeline layout
        vkDestroyPipelineLayout(
            in_device,
            pipeline_info.pipeline_layout,
            nullptr);

	pipeline_info.pipeline_layout = VK_NULL_HANDLE;
    }

    if (pipeline_info.pipeline)
    {
        // destroy pipeline
        vkDestroyPipeline(
            in_device,
            pipeline_info.pipeline,
            nullptr
	    );

	pipeline_info.pipeline = VK_NULL_HANDLE;
    }
}
