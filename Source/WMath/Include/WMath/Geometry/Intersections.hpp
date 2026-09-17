#pragma once

#include "WMath/Geometry/Spaces.hpp"
#include "WMath/LinAlgebra.hpp"
#include "WMath/Geometry/Distance.hpp"
#include "WMath/Geometry/ClosestPoint.hpp"
#include "WMath/Geometry/IntersectionPoint.hpp"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace wmath::geometry::intersections {

    inline bool Intersects(
        wmath::geometry::shapes::AABB a,
        wmath::geometry::shapes::AABB b
        ) {
        if (a.max[0] < b.min[0] || a.min[0] > b.max[0]) return false;
        if (a.max[1] < b.min[1] || a.min[1] > b.max[1]) return false;
        if (a.max[2] < b.min[2] || a.min[2] > b.max[2]) return false;

        return true;
    }

    /**
     * @brief : If "a" and "b" are two Box collider shapes,
     * too ensure a valid inersection check with CheckOBBIntersection it is required to call
     * (CheckOBBIntersection(a,b,b_transform_relative_to_a) && CheckOBBIntersection(b,a,a_transform_relative_to_b)) == true
     */
    inline bool Intersects(
        wmath::geometry::shapes::Box axis_box,
        wmath::geometry::shapes::Box obb,
        glm::mat4 obb_transform
        ) {
        auto other_radii = wmath::geometry::shapes::BoxRadii(obb, obb_transform);

        glm::vec3 axis_radii{0.f};

        for(std::uint32_t i=0; i<other_radii.size(); i++) {
            glm::vec3 abs_values = {
                std::abs(other_radii[i].x),
                std::abs(other_radii[i].y),
                std::abs(other_radii[i].z)
            };
                    
            if (abs_values.x > axis_radii.x) {
                axis_radii.x = abs_values.x;
            }
            if (abs_values.y > axis_radii.y) {
                axis_radii.y = abs_values.y;
            }
            if (abs_values.z > axis_radii.z) {
                axis_radii.z = abs_values.z;
            }
        }

        // X projection
        if (std::abs(obb_transform[3].x) > axis_box.x + axis_radii.x)
            return false;

        // Y projection
        if (std::abs(obb_transform[3].y) > axis_box.y + axis_radii.y)
            return false;

        // Z projection
        if (std::abs(obb_transform[3].z) > axis_box.z + axis_radii.z)
            return false;

        return true;
    }

    inline bool Intersects(
        wmath::geometry::shapes::Box axis_box,
        glm::vec3 point
        ) {
        return axis_box.x >= std::abs(point.x) &&
            axis_box.y >= std::abs(point.y) &&
            axis_box.z >= std::abs(point.z);
    }

    /**
     * @brief Returns true if axis_box and sphere are intersecting.
     * @param center_box : box collision shape, it is considered the center of the system.
     * @param sphere : sphere collision shape.
     * @param sphere_position : sphere position relative to center_box.
     */
    inline bool Intersects(
        wmath::geometry::shapes::Box axis_box,
        wmath::geometry::shapes::Sphere sphere,
        glm::vec3 sphere_position
        ) {
        sphere_position = {
            std::abs(sphere_position.x),
            std::abs(sphere_position.y),
            std::abs(sphere_position.z)
        };

        glm::vec3 nearest_box_point = glm::vec3{
            std::min(axis_box.x, sphere_position.x),
            std::min(axis_box.y, sphere_position.y),
            std::min(axis_box.z, sphere_position.z)
        };

        glm::vec3 check = nearest_box_point - sphere_position;

        return glm::dot(check, check) <= sphere.radius * sphere.radius;
    }

    inline bool Intersects(
        wmath::geometry::shapes::Box axis_box,
        wmath::geometry::shapes::Capsule capsule,
        glm::mat4 capsule_transform
        ) {

        auto [p_a, p_b] = wmath::geometry::shapes::AsSegment(capsule, capsule_transform);

        return wcl::distance::MinSquareDistance(axis_box, p_a, p_b) <=
            (capsule.radius * capsule.radius);
    }

    /**
     * @brief Returns true if plane is intersecting the axis_box.
     * plane normal and distance should be relative to axis_box.
     * plane normal is not required to be normalized.
     */
    inline bool Intersects(
        wmath::geometry::shapes::Box axis_box,
        wmath::geometry::shapes::Plane plane
        ) {
        float r =
            axis_box.x * std::abs(plane.n.x) +
            axis_box.y * std::abs(plane.n.y) +
            axis_box.z * std::abs(plane.n.z);

        return std::abs(plane.dist) * glm::dot(plane.n, plane.n) <= r ;
    }

    inline bool Intersects(
        wmath::geometry::shapes::Box axis_box,
        wmath::geometry::shapes::Tri tri
        ) {

        float p0, p1, p2, r;

        glm::vec3 f0 = tri[1] - tri[0];
        glm::vec3 f1 = tri[2] - tri[1];
        glm::vec3 f2 = tri[0] - tri[2];

        // test axis a00
        p0 = tri[0].z * tri[1].y - tri[0].y*tri[1].z;
        p2 = tri[2].z*(tri[1].y - tri[0].y) - tri[2].y * (tri[1].z - tri[0].z);
        r = axis_box.y * std::abs(f0.z) + axis_box.z * std::abs(f0.y);
        if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return false;

        // test axis a01
        p0 = tri[0].y * (-tri[2].z + tri[1].z) + tri[0].z * (tri[2].y - tri[1].y);
        p1 = -tri[1].y * tri[2].z + tri[1].z * tri[2].y;
        r = axis_box.y * std::abs(f1.z) + axis_box.z * (f1.y);
        if (std::max(-std::max(p0,p1), std::min(p0,p1)) > r) return false;

        // test axis a02
        p0 = tri[0].y * tri[2].z - tri[0].z * tri[2].y;
        p1 = tri[1].y * (-tri[0].z + tri[2].z) + tri[1].z * (tri[0].y - tri[2].y);
        r = axis_box.y * std::abs(f2.z) + axis_box.z * (std::abs(f2.y));
        if (std::max(-std::max(p0,p1), std::min(p0,p1)) > r) return false;

        // test axis a10
        p0 = tri[0].x * tri[1].z - tri[0].z * tri[1].x;
        p2 = tri[2].x * (tri[1].z - tri[0].z) + tri[2].z * (-tri[1].x + tri[0].x);
        r = axis_box.x*std::abs(f0.z) + axis_box.z * std::abs(f0.x);
        if (std::max(-std::max(p0, p2), std::min(p0, p1)) > r) return false;

        // test axis a11
        p0 = tri[1].x * (tri[2].z - tri[1].z) + tri[1].z * (-tri[2].x + tri[1].x);
        p1 = tri[1].x * tri[2].z - tri[1].z * tri[2].x;
        r = axis_box.x * std::abs(f1.z) + axis_box.z * std::abs(f1.x);
        if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) return false;

        // test axis a12
        p0 = tri[0].z * tri[2].x - tri[0].x * tri[2].z;
        p1 = tri[1].x * (tri[0].z - tri[2].z) + tri[1].z * (-tri[0].x + tri[2].x);
        r = axis_box.x * std::abs(f2.z) + axis_box.z * std::abs(f2.x);
        if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) return false;

        // test axis a20
        p0 = tri[0].y * tri[1].x - tri[0].x * tri[1].y;
        p1 = tri[2].x * (-tri[1].y + tri[0].y) + tri[2].y * (tri[1].x - tri[0].x);
        r = axis_box.x * std::abs(f0.y) + axis_box.y * std::abs(f0.x);
        if (std::max(-std::max(p0, p1), std::min(p0,p1)) > r) return false;

        // test axis a21
        p0 = tri[0].x * (-tri[2].y + tri[1].y) + tri[0].y * (tri[2].x - tri[1].x);
        p1 = tri[1].y * tri[2].x - tri[1].x * tri[2].y;
        r = axis_box.x * std::abs(f1.y) + axis_box.y * std::abs(f1.x);
        if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) return false;

        // test axis a22
        p0 = tri[0].x * tri[2].y - tri[0].y * tri[2].x;
        p1 = tri[1].x * (-tri[0].y + tri[2].y) + tri[1].y * (tri[0].x - tri[2].x);
        r = axis_box.x * std::abs(f2.y) + axis_box.y * std::abs(f2.x);
        if(std::max(-std::max(p0, p1), std::min(p0, p1)) > r) return false;

        // test axis corresponding to face normals
        if (std::max(tri[0].x, std::max(tri[1].x, tri[2].x)) < - axis_box.x ||
            std::min(tri[0].x, std::min(tri[1].x, tri[2].x)) > axis_box.x) return false;

        if (std::max(tri[0].y, std::max(tri[1].y, tri[2].y)) < - axis_box.y ||
            std::min(tri[0].y, std::min(tri[1].y, tri[2].y)) > axis_box.y) return false;

        if (std::max(tri[0].z, std::max(tri[1].z, tri[2].z)) < - axis_box.z ||
            std::min(tri[0].z, std::min(tri[1].z, tri[2].z)) > axis_box.z) return false;
        
        // axis triangle face normal
        wmath::geometry::shapes::Plane plane;
        plane.n = glm::cross(f0, f1);
        plane.dist = glm::dot(plane.n, tri[0]) / glm::dot(plane.n, plane.n);

        return Intersects(axis_box, plane);
    }

    // TODO make the mesh the axis shape
    inline bool Intersects(
        wmath::geometry::shapes::Box axis_box,
        wmath::geometry::shapes::Mesh const & mesh,
        glm::mat4 mesh_transform
        ) {

        std::vector cpy = mesh.vertices;
        
        std::transform(
            cpy.begin(), cpy.end(), cpy.begin(),
            [&mesh_transform](auto & vert) -> glm::vec3 {
                return mesh_transform * glm::vec4{vert, 1.f};
            });

        for(std::uint32_t i=0; i < mesh.indices.size(); i=i+3) {
            if (Intersects(
                    axis_box,
                    wmath::geometry::shapes::Tri {
                        cpy[mesh.indices[i]],
                        cpy[mesh.indices[i+1]],
                        cpy[mesh.indices[i+2]]
                    }
                    )) {
                return true;
            }
        }
        
        return false;
    }

    inline bool Intersects(
        wmath::geometry::shapes::Sphere sphere,
        glm::vec3 point
        ) {
        return glm::dot(point, point) <= std::pow(sphere.radius, 2);
    }

    inline bool Intersects(
        wmath::geometry::shapes::Sphere a_sphere,
        wmath::geometry::shapes::Sphere b_sphere,
        glm::vec3 b_translation
        ) {

        float sqr_dist = glm::dot(b_translation, b_translation);

        return std::pow(a_sphere.radius + b_sphere.radius, 2) >= sqr_dist;
    }

    /**
     * @brief Check if axis_sphere and capsule are intersecting.
     * @param capsule_transform : capsule transform relative to axis_sphere.
     */
    inline bool Intersects(
        wmath::geometry::shapes::Sphere axis_sphere,
        wmath::geometry::shapes::Capsule capsule,
        glm::mat4 capsule_transform
        ) {

        auto [p_a, p_b] = wmath::geometry::shapes::AsSegment(capsule, capsule_transform);

        glm::vec3 segment = p_b - p_a;

        float t = (- glm::dot(p_a, segment))/(glm::dot(segment, segment));

        float t_min = std::max(std::min(t,1.f), 0.f);

        glm::vec3 spoint = p_a + segment * t;

        float sqr_dist = glm::dot(spoint, spoint);
        float tier = std::pow(axis_sphere.radius + capsule.radius, 2);

        return sqr_dist <= tier;
    }

    inline bool Intersects(
        wmath::geometry::shapes::Sphere axis_sphere,
        wmath::geometry::shapes::Plane plane
        ) {
        glm::vec3 p = plane.n * plane.dist;
        return glm::dot(p,p) <= (axis_sphere.radius * axis_sphere.radius);
    }

    inline bool Intersects(
        wmath::geometry::shapes::Tri tri,
        wmath::geometry::shapes::Sphere sphere,
        glm::vec3 sphere_pos
        ) {
        glm::vec3 closest = wcl::closest_point::OnTriangle(
            tri,
            glm::vec3{0.f}
            );

        glm::vec3 vector = closest - sphere_pos;

        return glm::dot(vector, vector) <= sphere.radius * sphere.radius;
    }

    inline bool Intersects(
        wmath::geometry::shapes::Mesh const & axis_mesh,
        wmath::geometry::shapes::Sphere sphere,
        glm::vec3 sphere_pos
        ) {
        for(std::uint32_t i=0; i<axis_mesh.indices.size(); i=i+3) {
            if (Intersects({
                        axis_mesh.vertices[axis_mesh.indices[i]],
                        axis_mesh.vertices[axis_mesh.indices[i+1]],
                        axis_mesh.vertices[axis_mesh.indices[i+2]]
                    },
                    sphere,
                    sphere_pos)) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Checks if a point is intersecting with a capsule.
     * Capsule transform is expressed relative to the point.
     * To compute the algorithm, point coords are (0, 0, 0).
     */
    inline bool Intersects(
        wmath::geometry::shapes::Capsule capsule,
        glm::mat4 capsule_transform
        ) {
        auto[p1_a, p1_b] = wmath::geometry::shapes::AsSegment(capsule, capsule_transform);

        glm::vec3 s1 = p1_b - p1_a;
        glm::vec3 s2 = -p1_a;

        float t = glm::dot(s1, s2) / glm::dot(s1, s1);

        t = std::max(std::min(t,1.f), 0.f);

        glm::vec3 p = p1_a + s1 * t;

        return glm::dot(p,p) <= capsule.radius * capsule.radius;
    }

    inline bool Intersects(
        wmath::geometry::shapes::Capsule axis_capsule,
        wmath::geometry::shapes::Capsule capsule,
        glm::mat4 capsule_transform
        ) {
        auto s0 = wmath::geometry::shapes::AsSegment(axis_capsule);
        auto s1 = wmath::geometry::shapes::AsSegment(capsule, capsule_transform);

        auto [s_min, t_min] = wcl::closest_point::OnSegments(
            s0, s1
            );

        glm::vec3 len_vec =
            (s0.p0 + (s0.p1 - s0.p0) * s_min) - (s1.p0 + (s1.p1 - s1.p0) * t_min);

        return glm::dot(len_vec, len_vec) <= std::pow(axis_capsule.radius + capsule.radius, 2);
    }

    inline bool Intersects(
        wmath::geometry::shapes::Tri tri,
        wmath::geometry::shapes::Segment segment,
        float radius
        ) {

        wmath::geometry::shapes::Plane plane = wmath::geometry::shapes::AsPlane(tri);
        auto intrsct_pnt = wcl::intersection_point::PlaneSegment(plane, segment);

        auto inside_tri = [&tri](glm::vec3 point) {
            auto[u,v] = wcl::spaces::AsVectorSum(
                point - tri[0],
                tri[1] - tri[0],
                tri[2] - tri[0]
                );

            if (u + v >= 0 && u + v <=1) return true;
            else return false;
        };

        if (intrsct_pnt) {
            if (inside_tri(intrsct_pnt.value())) return true;
        }

        auto min_vector =
            [&segment]
            (float s_min, glm::vec3 p0, glm::vec3 p1, float t_min) -> glm::vec3 {
            return (segment.p0 + (segment.p1 - segment.p0) * s_min) -
                (p0 + (p1 - p0) * t_min);
        };

        float sqrdist = std::numeric_limits<float>::max();
        
        // tri0 tri1

        auto [s_min, t_min] = wcl::closest_point::OnSegments(
            segment,
            wmath::geometry::shapes::Segment{tri[0], tri[1]}
            );
        float ftmp = wmath::lin_algbr::SqrLength(
            min_vector(s_min, tri[0], tri[1], t_min)
            );
        if (ftmp < sqrdist) sqrdist = ftmp;

        // tri1 tri2

        std::tie(s_min, t_min) = wcl::closest_point::OnSegments(
            segment,
            {tri[1], tri[2]}
            );
        ftmp = wmath::lin_algbr::SqrLength(
            min_vector(s_min, tri[1], tri[2], t_min)
            );
        if(ftmp < sqrdist) sqrdist = ftmp;

        // tri2 tri0
        
        std::tie(s_min, t_min) = wcl::closest_point::OnSegments(
            segment,
            {tri[2], tri[0]}
            );
        ftmp = wmath::lin_algbr::SqrLength(
            min_vector(s_min, tri[2], tri[0], t_min)
            );
        if(ftmp < sqrdist) sqrdist = ftmp;

        // test segment points
        // p0
        glm::vec3 pnt = wcl::closest_point::OnPlane(plane, segment.p0);
        if (inside_tri(pnt)) {
            ftmp = wmath::lin_algbr::SqrLength(pnt - segment.p0);
            if (ftmp < sqrdist) sqrdist = ftmp;
        }
        // p1
        pnt = wcl::closest_point::OnPlane(plane, segment.p1);
        if(inside_tri(pnt)) {
            ftmp = wmath::lin_algbr::SqrLength(pnt - segment.p1);
            if(ftmp < sqrdist) sqrdist = ftmp;
        }

        return ftmp <= radius * radius;
    }

    inline bool Intersects(
        wmath::geometry::shapes::Mesh const & mesh,
        wmath::geometry::shapes::Capsule capsule,
        glm::mat4 capsule_transform
        ) {

        auto segment = wmath::geometry::shapes::AsSegment(capsule, capsule_transform);

        for(std::uint32_t i=0; i<mesh.indices.size(); i=i+3) {
            if(Intersects(
                   wmath::geometry::shapes::Tri{
                       mesh.vertices[mesh.indices[i]],
                       mesh.vertices[mesh.indices[i+1]],
                       mesh.vertices[mesh.indices[i+2]]
                   },
                   segment,
                   capsule.radius
                   )) {
                return true;
            }
        }

        return false;
    }

    inline bool Intersects(
        wmath::geometry::shapes::Mesh const & axis_mesh,
        wmath::geometry::shapes::Mesh const & mesh,
        glm::mat4 mesh_transform
        ) {
        
    }

}
