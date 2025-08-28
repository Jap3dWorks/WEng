#pragma once
#include "WCore/WCore.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>

struct WVertexStruct{
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Color;
    // glm::vec3 Normal;
    // glm::vec3 Tangent;
    // glm::vec3 Bitangent;

    bool operator==(const WVertexStruct& other) const{
        return Position == other.Position && 
            TexCoords == other.TexCoords &&
            Color == other.Color;
            // Normal == other.Normal && 
    }
};

template<>
struct std::hash<WVertexStruct>{
    size_t operator()(WVertexStruct const& vertex) const
        {
            return (
                (hash<glm::vec3>()(vertex.Position) ^
                 (hash<glm::vec3>()(vertex.Color) << 1 )) >> 1 ) ^
                (hash<glm::vec2>()(vertex.TexCoords) << 1);
        }
};

struct WMeshStruct{
    std::vector<WVertexStruct> vertices;
    std::vector<uint32_t> indices;
};

// Meshes by Id, max 16
struct WMeshsesStruct{
    std::array<WMeshStruct, WENG_MAX_ASSET_IDS> meshes;
};



