#pragma once
#include "WCore/WCore.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>

namespace wct::geometry {

    using Index = std::uint32_t;

    struct Vertex{
        glm::vec3 position{};
        glm::vec2 tex_coords{};
        glm::vec4 color{};
        glm::vec3 normal{};
        glm::vec4 tangent{};

        bool operator==(const Vertex& other) const{
            return position == other.position && 
                tex_coords == other.tex_coords &&
                color == other.color &&
                normal == other.normal;
        }
    };

    struct Mesh{
        std::vector<Vertex> vertices{};
        std::vector<Index> indices{};
    };


}

template<>
struct std::hash<wct::geometry::Vertex>{
    size_t operator()(wct::geometry::Vertex const& vertex) const
        {
            return (
                (hash<glm::vec3>()(vertex.position) ^
                 (hash<glm::vec3>()(vertex.color) << 1 )) >> 1 ) ^
                (hash<glm::vec2>()(vertex.tex_coords) << 1);
        }
};
