#include "WVulkan/RAII/WVkSurfaceRAII.hpp"

#include "WWindow/WWindow.hpp"

VkSurfaceKHR WVkSurfaceCreator::Create(wdw::WWindow* in_window) {
    VkSurfaceKHR result;
    in_window->CreateWindowSurface(result, instance_);

    return result;
}


// WVkSurfaceRAII::WVkSurfaceRAII(VkInstance in_instance,
//                                wdw::WWindow * in_window) :
//     vk_instance_(in_instance)
// {
//     in_window->CreateWindowSurface(vk_surface_, vk_instance_);

// }
