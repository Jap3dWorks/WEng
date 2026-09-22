#pragma once

#include "WObjects/WComponent.hpp"
#include "WMath/Geometry/Shape.hpp"

#include <variant>

#include "wcl::component::Collision.WEng.hpp"

namespace wcl::component {

    using CollisionShape = std::variant<
        wmath::geometry::shape::Box,
        wmath::geometry::shape::Capsule,
        wmath::geometry::shape::Sphere,
        wmath::geometry::shape::Plane,
        wmath::geometry::shape::Mesh
        >;

    class WCOLLISION_API Collision : public WComponent {

        WOBJECT_BODY;

    public:

        WPROPERTY(CollisionShape, collision_shape, wmath::geometry::shape::Box{});

    };
}
