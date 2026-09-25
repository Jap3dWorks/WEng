#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TVisitor.hpp"
#include "WCollision/Components/Collider.hpp"
#include "WCollision/IntersectionVisitor.hpp"

#include <glm/glm.hpp>

namespace wcl {

    class WCOLLISION_API Resolver {

    public:

    static inline bool CheckIntersection(
        wcl::component::Collider const & a,
        glm::mat4 const & a_transform,
        glm::mat4 const & a_inv_transform,
        wcl::component::Collider const & b,
        glm::mat4 const & b_transform,
        glm::mat4 const & b_inv_transform
        ) {

        return std::visit<bool>(
            wcr::TVisitor {
                [&] <typename Shp1>
                    (Shp1 && ashp) -> bool {
                    return std::visit<bool>(
                        [&]<typename Shp2>
                        (Shp2 && bshp) {
                            return IntersectVisitor::Visit(
                                std::forward<Shp1>(ashp),
                                a_transform,
                                a_inv_transform,
                                std::forward<Shp2>(bshp),
                                b_transform,
                                b_inv_transform
                                );
                        },
                        b.Get_collision_shape()
                        );
                },
            },
            a.Get_collision_shape()
            );
    }
    };

}
