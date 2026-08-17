#include "WVulkan/RAII/Surface.hpp"

#include "WWindow/WWindow.hpp"

VkSurfaceKHR wvk::raii::SurfaceCreator::Create(wdw::WWindow* in_window) {
    VkSurfaceKHR result;
    in_window->CreateWindowSurface(result, instance_);

    return result;
}

