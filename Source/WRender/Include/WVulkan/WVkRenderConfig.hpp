#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>
#include <string_view>

inline constexpr std::uint32_t WENG_MAX_FRAMES_IN_FLIGHT{2};

inline constexpr std::string_view WENG_VK_OFFSCREEN_SHADER_PATH{"Content/Shaders/WRender_Offscreen.offscrn.spv"};
inline constexpr std::string_view WENG_VK_SWAPCHAIN_SHADER_PATH{"Content/Shaders/WRender_DrawInSwapChain.swap.spv"};
inline constexpr std::string_view WENG_VK_TONEMAPPING_SHADER_PATH{"Content/Shaders/WRender_Tonemapping.tone.spv"};

inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_COLOR_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};

inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_NORMAL_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};
inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_WSPOSITION_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};
inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_DEPTH_FORMAT{VK_FORMAT_D32_SFLOAT};
inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_MTLLRGHAO_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};
inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_EMISSION_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};
inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_EXTRA01_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};
// inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_EXTRA02_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};

inline constexpr VkFormat WENG_VK_OFFSCREEN_RENDER_COLOR_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};

inline constexpr VkFormat WENG_VK_POSTPROCESS_RENDER_COLOR_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};

inline constexpr std::uint8_t WENG_VK_GBUFFERS_COUNT{7};

