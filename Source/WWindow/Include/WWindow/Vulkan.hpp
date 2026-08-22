#pragma once

#include <cstdint>

namespace wdw::vulkan {
    
    char const ** GetRequiredInstanceExtensions(std::uint32_t & out_extension_count);
    
}
