#include "WShader.h"


WShaderModule::WShaderModule(const WShaderStageInfo& shader_stage_info, const WDeviceInfo& device_info) :
    device(device_info.vk_device)
{
    VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.codeSize = shader_stage_info.code.size();
    ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(
        shader_stage_info.code.data()
    );

    if (vkCreateShaderModule(
        device_info.vk_device, 
        &ShaderModuleCreateInfo, 
        nullptr, 
        &shader_module
    ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module!");
    }
}


WShaderModule::~WShaderModule()
{
    vkDestroyShaderModule(
	device, 
	shader_module,
	nullptr
    );
}
