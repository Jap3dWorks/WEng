#pragma once

#include "WObjects/WComponent.hpp"
#include "WMath/Geometry/Shape.hpp"

#include <variant>

#include "wcl::component::Collider.WEng.hpp"

namespace wcl::component {

    using CollisionShape = std::variant<
        wmath::geometry::shape::Box,
        wmath::geometry::shape::Sphere,
        wmath::geometry::shape::Capsule,
        wmath::geometry::shape::Plane,
        wmath::geometry::shape::Mesh
        >;

    class WCOLLISION_API Collider : public WComponent {

        WOBJECT_BODY;

    public:

        WPROPERTY(CollisionShape, collision_shape, wmath::geometry::shape::Box{});

    };
}
