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

struct WStaticModelComponent{
    std::vector<WVertex> vertices;
    std::vector<uint32_t> indices;
};

struct WModel{
    std::vector<WStaticModelComponent> meshes;
    // materials by index
    // std::vector<WTexture> textures;
};


