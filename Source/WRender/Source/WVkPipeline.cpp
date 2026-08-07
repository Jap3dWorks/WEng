#include "WVulkan/Vk/WVkPipeline.hpp"

void wvk::pipeline::Destroy(
    VkPipeline pipeline,
    VkDevice device
    ) {
    vkDestroyPipeline(
        device,
        pipeline,
        nullptr
	    );
    
}

void wvk::pipeline::Destroy(
    VkPipelineLayout pipeline_layout,
    VkDevice device
    ) {
    // destroy pipeline layout
    vkDestroyPipelineLayout(
        device,
        pipeline_layout,
        nullptr);
    
}
