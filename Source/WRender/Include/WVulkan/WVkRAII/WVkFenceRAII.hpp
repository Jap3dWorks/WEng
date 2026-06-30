#pragma once

#include <vulkan/vulkan_core.h>

class WVkFenceRAII {
public:

    WVkFenceRAII() = default;

    WVkFenceRAII(const WVkFenceRAII&) = delete;
    WVkFenceRAII& operator=(const WVkFenceRAII&) = delete;
    
    WVkFenceRAII(WVkFenceRAII&&) = default;
    WVkFenceRAII& operator=(WVkFenceRAII&&) = default;
    
    virtual ~WVkFenceRAII() = default;


    VkFence Fence() const {
        return fence_;
    }

private:

    void Destroy(){
        if (device_ != VK_NULL_HANDLE) {
            vkDestroyFence(
                device_,
                fence_,
                nullptr
                );

            device_ = VK_NULL_HANDLE;
        }
    }

private:
    
    VkDevice device_{VK_NULL_HANDLE};
    VkFence fence_{VK_NULL_HANDLE};
  
};
