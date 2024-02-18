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

