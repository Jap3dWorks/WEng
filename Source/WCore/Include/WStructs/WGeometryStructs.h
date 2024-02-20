#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>

struct WVertexStruct{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    glm::vec4 Color;

    bool operator==(const WVertexStruct& other) const{
        return Position == other.Position && 
               Normal == other.Normal && 
               TexCoords == other.TexCoords;
    }
};

template<>
struct std::hash<WVertexStruct>{
    size_t operator()(WVertexStruct const& vertex) const
    {
        return (
            (hash<glm::vec3>()(vertex.Position) ^ (hash<glm::vec3>()(vertex.Normal) << 1)) >> 1
            ) ^ (hash<glm::vec2>()(vertex.TexCoords) << 1);
    }
};

struct WMeshStruct{
    std::vector<WVertexStruct> vertices;
    std::vector<uint32_t> indices;
};

struct WModelStruct{
    std::vector<WMeshStruct> meshes;
};

