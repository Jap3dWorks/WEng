#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>
#include <string_view>
#include <vulkan/vulkan_core.h>

// namespace wvk::constants

inline constexpr std::uint8_t WENG_MAX_FRAMES_IN_FLIGHT{2};

inline constexpr std::string_view WENG_VK_LIGHTING_SHADER_PATH{"Content/Shaders/WRender_PBR.light.spv"};
inline constexpr std::string_view WENG_VK_SWAPCHAIN_SHADER_PATH{"Content/Shaders/WRender_DrawInSwapChain.swap.spv"};
inline constexpr std::string_view WENG_VK_TONEMAPPING_SHADER_PATH{"Content/Shaders/WRender_Tonemapping.tone.spv"};

inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_COLOR_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};
inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_EMISSION_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};
inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_NORMAL_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};

inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_MRAO_FORMAT{VK_FORMAT_R8G8B8A8_UNORM};

inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_EXTRA01_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};
inline constexpr VkFormat WENG_VK_GBUFFER_RENDER_DEPTH_FORMAT{VK_FORMAT_D32_SFLOAT};

inline constexpr VkFormat WENG_VK_LIGHTING_RENDER_COLOR_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};

inline constexpr VkFormat WENG_VK_POSTPROCESS_RENDER_COLOR_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};

// TODO check for a better solution.
inline constexpr std::uint8_t WENG_VK_GBUFFERS_COUNT{6};

#ifdef NDEBUG
inline constexpr bool WENG_VK_ENABLE_VALIDATION_LAYERS{false};
#else
inline constexpr bool WENG_VK_ENABLE_VALIDATION_LAYERS{true};
#endif

