#pragma once

#include "WVulkan/Vk/WVkTypes.hpp"
#include "WVulkan/WVkConfig.hpp"
#include "WVulkan/RAII/DescriptorPool.hpp"
#include "WVulkan/RAII/DescriptorSetLayout.hpp"
#include "WVulkan/RAII/Pipeline.hpp"
#include "WVulkan/RAII/PipelineLayout.hpp"
#include "WRender/WShader.hpp"
#include "WVulkan/Vk/WVkRender.hpp"

#include <string_view>
#include <vulkan/vulkan_core.h>
#include <span>
#include <variant>

namespace wvk::raii {
    
    template<std::uint8_t FramesInFlight=WVK_MAX_FRAMES_IN_FLIGHT>
    class ShadowMapPipeline {

    public:

        static inline constexpr std::string_view SHADER_PATH{"/Content/Shaders/..."};

    public:

        ShadowMapPipeline() = default;
        ShadowMapPipeline(const ShadowMapPipeline&) = delete;
        ShadowMapPipeline(ShadowMapPipeline&&) = default;
        ShadowMapPipeline& operator=(const ShadowMapPipeline&) = delete;
        ShadowMapPipeline& operator=(ShadowMapPipeline&&) = default;
        ~ShadowMapPipeline() = default;

        ShadowMapPipeline(
            VkDevice device,
            std::string_view shader_path
            ) :
            descriptor_pool_({device}),
            descset_lay_(
                {device},
                std::array{
                    VkDescriptorSetLayoutBinding{
                        .binding=0,
                        .descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .descriptorCount=1,
                        .stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT,
                        .pImmutableSamplers=VK_NULL_HANDLE
                    }
                }),
            pipeline_layout_(
                {device},
                std::array{
                    *descset_lay_
                }
                )
            {
                InitializePipeline();
            }

    private:

        void InitializePipeline() {

                auto shadercode = wrd::shader::ReadShader(
                    wstr::SystemPath(SHADER_PATH)
                    );

                std::array shader_stages_info {
                    WVkShaderStageInfo{
                        .code{std::span<std::uint8_t>{shadercode}},
                        .type=wct::render::EShaderStageFlag::Vertex,
                        .entry_point{"vsMain"}
                    },
                    WVkShaderStageInfo{
                        .code{std::span<std::uint8_t>{shadercode}},
                        .type=wct::render::EShaderStageFlag::Fragment,
                        .entry_point{"fsMain"}
                    }
                };

                auto[shader_stages, shader_modules] =
                    wvk::shader::CreateShaderModules<shader_stages_info.size()>(
                        pipeline_layout_.Creator().device,
                        shader_stages_info
                        );

                
            
        }


    private:

        wvk::raii::DescriptorPool<1 * FramesInFlight, 0, 1 * FramesInFlight> descriptor_pool_{};
        wvk::raii::DescriptorSetLayout<1> descset_lay_{};
        wvk::raii::PipelineLayout<1>  pipeline_layout_{};
        wvk::raii::PipelineWrapper pipeline_{};

    };
}
