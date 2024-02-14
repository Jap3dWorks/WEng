#pragma once

#include <glm/glm.hpp>
#include <vector>


struct WVertex{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    glm::vec4 Color;
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