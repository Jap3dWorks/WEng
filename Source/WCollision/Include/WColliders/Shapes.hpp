#pragma once

#include <variant>

namespace wcl::shapes {

    struct Cube{
        float x{10.f};  // side length
        float y{10.f};
        float z{10.f};
    };

    struct Sphere{
        float radius{5.f};
    };

    struct Capsule{
        float length{10.f};
        float radius{3.f};
    };
    
    using ShapeVariant = std::variant<Cube, Sphere, Capsule>;

}
