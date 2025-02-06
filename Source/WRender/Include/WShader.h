#pragma once

#include "WCore/WCore.h"
#include "WRenderCore.h"
#include <string>
#include <vector>
#include "vulkan/vulkan.h"
#include <stdexcept>
#include <cassert>


class WShaderModule
{
public:

    WShaderModule() = delete;
    WShaderModule(const WShaderModule&) = delete;
    WShaderModule& operator=(const WShaderModule&) = delete;

    WShaderModule(WShaderModule&& other)
    {
        assert(this != &other);
        
        shader_module = other.shader_module;
        device = other.device;

        other.shader_module = VK_NULL_HANDLE;
    };
    
    WShaderModule& operator=(WShaderModule&& other)
    {
        assert(this != &other);
        
        shader_module = other.shader_module;
        device = other.device;

        other.shader_module = nullptr;

	return *this;
    }

    ~WShaderModule();

    WShaderModule(const WShaderStageInfo&, const WDeviceInfo&);

    const VkShaderModule GetShaderModule() const
    {
        return shader_module;
    }

private:

    VkShaderModule shader_module{VK_NULL_HANDLE};
    VkDevice device{VK_NULL_HANDLE};

};

