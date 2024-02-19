#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>

struct WVertex{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    glm::vec4 Color;

    bool operator==(const WVertex& other) const{
        return Position == other.Position && 
               Normal == other.Normal && 
               TexCoords == other.TexCoords;
    }
};

template<>
struct std::hash<WVertex>{
    size_t operator()(WVertex const& vertex) const
    {
        return (
            (hash<glm::vec3>()(vertex.Position) ^ (hash<glm::vec3>()(vertex.Normal) << 1)) >> 1
            ) ^ (hash<glm::vec2>()(vertex.TexCoords) << 1);
    }
};

struct WMesh{
    std::vector<WVertex> vertices;
    std::vector<uint32_t> indices;
};

struct WModel{
    std::vector<WMesh> meshes;
    // materials by index
    // std::vector<WTexture> textures;
};

enum class ETextureChannels : uint8_t
{
    // 8 bit
    // R=1,G=2,B=4,A=8
    R=1,
    RG=3,
    RGB=7,
    RGBA=15,
    // 16 bit
    // d_16=16,
    R16=17,
    RG16=19,
    RGB16=23,
    RGBA16=31,
    // 32 bit
    // d_32=32,
    R32=33,
    RG32=35,
    RGB32=39,
    RGBA32=47,
};

ETextureChannels operator|(ETextureChannels a, ETextureChannels b)
{
    return static_cast<ETextureChannels>(
        static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
    );
}

ETextureChannels operator&(ETextureChannels a, ETextureChannels b)
{
    return static_cast<ETextureChannels>(
        static_cast<uint8_t>(a) & static_cast<uint8_t>(b)
    );
}

struct WTexture{
    uint32_t id;
    std::vector<uint8_t> data;
    ETextureChannels channels;
    uint32_t width;
    uint32_t height;    
};
