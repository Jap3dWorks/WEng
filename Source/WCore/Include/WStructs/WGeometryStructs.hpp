#pragma once
#include "WCore/WCore.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>

struct WVertexStruct{
    glm::vec3 position;
    glm::vec2 tex_coords;
    glm::vec3 color;
    glm::vec3 normal;
    // glm::vec3 Tangent;
    // glm::vec3 Bitangent;

    bool operator==(const WVertexStruct& other) const{
        return position == other.position && 
            tex_coords == other.tex_coords &&
            color == other.color &&
            normal == other.normal;
    }
};

template<>
struct std::hash<WVertexStruct>{
    size_t operator()(WVertexStruct const& vertex) const
        {
            return (
                (hash<glm::vec3>()(vertex.position) ^
                 (hash<glm::vec3>()(vertex.color) << 1 )) >> 1 ) ^
                (hash<glm::vec2>()(vertex.tex_coords) << 1);
        }
};

struct WMeshStruct{
    std::vector<WVertexStruct> vertices;
    std::vector<uint32_t> indices;
};

// Meshes by Id, max 16
// struct WMeshsesStruct{
//     std::array<WMeshStruct, WENG_MAX_ASSET_IDS> meshes;
// };



