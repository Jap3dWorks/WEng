#pragma once

#include "WCore.h"
#include "WRenderCore.h"
#include <string>
#include <vector>
#include "vulkan/vulkan.h"
#include <stdexcept>
#include <cassert>

enum class WShaderType
{
    Vertex,
    Fragment,
    // Geometry,
    Compute,
    // TessellationControl,
    // TessellationEvaluation
};

struct WTextureInfo
{
    WId id;
    std::string path;
    VkImage image;
    VkDeviceMemory image_memory;
    VkImageView image_view;
    VkSampler sampler;
    uint32_t mip_levels;
};

struct WShaderStageInfo
{
    WId id;
    std::vector<char> code;
    WShaderType type;
    std::string entry_point;
    
    std::vector<VkVertexInputBindingDescription> binding_descriptors;
    std::vector<VkVertexInputAttributeDescription> attribute_descriptors;

};

class WShaderModuleManager
{
public:
    WShaderModuleManager() = delete;
    WShaderModuleManager(const WShaderModuleManager&) = delete;
    WShaderModuleManager& operator=(const WShaderModuleManager&) = delete;

    WShaderModuleManager(WShaderModuleManager&& other)
    {
        assert(this != &other);
        
        shader_module = other.shader_module;
        device = other.device;

        other.shader_module = VK_NULL_HANDLE;
    };
    
    WShaderModuleManager& operator=(WShaderModuleManager&& other)
    {
        assert(this != &other);
        
        shader_module = other.shader_module;
        device = other.device;

        other.shader_module = nullptr;
    };

    ~WShaderModuleManager()
    {
        vkDestroyShaderModule(
            device, 
            shader_module,
            nullptr
        );
    }

    WShaderModuleManager(const WShaderStageInfo&, const WDeviceInfo&);

    const VkShaderModule GetShaderModule() const
    {
        return shader_module;
    }

private:
    VkShaderModule shader_module{VK_NULL_HANDLE};
    VkDevice device{VK_NULL_HANDLE};

};
