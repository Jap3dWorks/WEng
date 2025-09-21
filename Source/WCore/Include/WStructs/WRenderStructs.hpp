#pragma once

#include "WCore/WCore.hpp"

#include <cstdint>
#include <array>

#include <glm/glm.hpp>

#define WENG_MAX_PIPELINE_SHADERS 8

/**
 * @brief Uniform buffer data structure.
*/
struct WUBOGraphicsStruct
{
    glm::mat4 model;
    // TODO params here
};

/**
 * @brief parameters to use in a shader.
 */
struct WUBOPostprocessStruct
{
    glm::vec4 param1;
    glm::vec4 param2;
    glm::vec4 param3;
    glm::vec4 param4;
};

/**
 * @brief Camera Data to be used in the shader.
 */
struct WUBOCameraStruct {
    glm::mat4 proj{};
    glm::mat4 view{1};
    glm::vec3 pos{1};
    glm::vec3 rot{0};  // pitch, yaw, roll
    glm::vec3 scale{1};
    float angle_of_view{54.f};
    float near_clipping{0.01f};
    float far_clipping{100.f};
};

struct WRenderSize {
    std::uint32_t width{0};
    std::uint32_t  height{0};
};

enum class EShaderStageFlag : uint8_t
{
    None=0,
    Vertex=1,
    Fragment=2,
    // Geometry,
    Compute=4,
    // TessellationControl,
    // TessellationEvaluation
};

constexpr const std::array<EShaderStageFlag, 4> WShaderStageFlagsList = {EShaderStageFlag::None,
                                                                         EShaderStageFlag::Vertex,
                                                                         EShaderStageFlag::Fragment,
                                                                         EShaderStageFlag::Compute };

constexpr EShaderStageFlag operator|(const EShaderStageFlag & l, const EShaderStageFlag & r) noexcept {
    return static_cast<EShaderStageFlag>(static_cast<std::uint8_t>(l) | static_cast<std::uint8_t>(r));
}

constexpr EShaderStageFlag operator&(const EShaderStageFlag & l, const EShaderStageFlag & r) noexcept {
    return static_cast<EShaderStageFlag>(static_cast<std::uint8_t>(l) & static_cast<std::uint8_t>(r));
}

enum class EPipelineType : uint8_t
{
    Graphics,       // Default Opaque
    Transparency,   // Alpha Blending
    Postprocess,    // Camera shader pipelines
    Compute,        // GPU
    RayTracing,     // Ray Tracing
    Swap            // Used to write in the swap chain
};

enum class EPipeParamType {
    None,
    Texture,
    Ubo
};

struct WPipeParamDescriptorStruct {
    std::uint8_t binding{0};
    EPipeParamType type{EPipeParamType::None};
    EShaderStageFlag stage_flags{EShaderStageFlag::None};
};

using WPipeParamDescriptorList = std::array<WPipeParamDescriptorStruct, 16>;

struct WShaderStruct {
    EShaderStageFlag type{EShaderStageFlag::None};
    char file[128]{""};
    char entry[16]{"main"};
};

using WShaderList = std::array<WShaderStruct, WENG_MAX_PIPELINE_SHADERS>;

struct WRenderPipelineStruct {
    EPipelineType type {EPipelineType::Graphics};
    WShaderList shaders{};
    std::uint8_t shaders_count{0};  // TODO: check if this is required

    WPipeParamDescriptorList params_descriptor{};
};

template<typename T>
struct TRPParam {
    std::uint16_t binding;
    T value;
};

// template<>
// struct TRPParam<void *> {
//     std::uint16_t binding;
//     void * ptr;
//     std::size_t size;
//     std::size_t offset;
// };

// using WRPPTexture = TRPParam<WAssetId>;
// using WRPPUbo = TRPParam<void*>;

struct WRPParameterStruct;
struct WRPParameterStruct_Texture;
struct WRPParameterStruct_Ubo;

struct WRPParameterVisitor {
    virtual void Visit(WRPParameterStruct *)=0;
    virtual void Visit(WRPParameterStruct_Texture *)=0;
    virtual void Visit(WRPParameterStruct_Ubo*)=0;
};

struct WRPParameterStruct {
    std::uint8_t binding{0};
    // EPipeParamType type{EPipeParamType::None};
    // void * data;
    // std::size_t size;
    // std::size_t offset;
    virtual void Visit(WRPParameterVisitor * in_visitor) {
        in_visitor->Visit(this);
    }
};

struct WRPParameterStruct_Texture : public WRPParameterStruct {
    WAssetId texture_id;
};

struct WRPParameterStruct_Ubo : public WRPParameterStruct {
    virtual void* Data() const=0;
    virtual std::size_t Size() const=0;
    std::size_t offset{0};
};

template<std::size_t N>
struct TRPParameterStruct_Ubo : public WRPParameterStruct_Ubo {
    char data[N];
    void * Data() const override { return data; }
    std::size_t Size() const override { return N; }
};

// struct 

// TODO: parameter UBO
// using WRPParameterList_Float = std::array<TRPParam<float>, 8>;
using WRPParameterList_WAssetId = std::array<TRPParam<WAssetId>, 8>;
// using WRPParameterList_Ubo = std::array<WRPPUbo,8>;

using WRPParameterList = std::array<WRPParameterStruct, 8>;

struct WRenderPipelineParametersStruct {
    WRPParameterList parameter_list{};
    std::uint8_t parameters_counts;
    
    // WRPParameterList_Float float_parameters{};
    // std::uint8_t float_parameters_count{0};
    
    WRPParameterList_WAssetId texture_assets{};
    std::uint8_t texture_assets_count{0};
};

