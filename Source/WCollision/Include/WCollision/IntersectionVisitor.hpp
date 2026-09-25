#pragma once

#include "WMath/Geometry/Intersection.hpp"
#include "WMath/Geometry/Shape.hpp"

#include <glm/glm.hpp>

namespace wcl {
    struct IntersectVisitor {
        
        // -------
        // box - box
        // -------

        static inline bool Visit(
            wmath::geometry::shape::Box const & abox,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Box const & bbox,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            )
            {
                return wmath::geometry::Intersects(
                    abox, bbox, a_inv_transform * b_transform
                    );
            }

        // ------------
        // box - sphere
        // ------------

        static inline bool Visit(
            wmath::geometry::shape::Box const & abox,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Sphere const & bsphr,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            glm::vec3 sphere_pos =
                glm::vec3{b_transform[3]} - glm::vec3{a_transform[3]};

            return wmath::geometry::Intersects(
                abox, bsphr, sphere_pos
                );
        }

        // -------------
        // box - capsule
        // -------------

        static inline bool Visit(
            wmath::geometry::shape::Box const & a_box,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Capsule const & b_cap,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            return wmath::geometry::Intersects(
                a_box, b_cap, a_inv_transform * b_transform
                );
        }

        // -----------
        // box - plane
        // -----------

        static inline bool Visit(
            wmath::geometry::shape::Box const & a_box,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Plane const & b_plane,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            return wmath::geometry::Intersects(
                a_box, b_plane
                );
        }

        // ----------
        // box - mesh
        // ----------

        static inline bool Visit(
            wmath::geometry::shape::Box const & a_box,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Mesh const & b_plane,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            return wmath::geometry::Intersects(
                a_box, b_plane, a_inv_transform * b_transform
                );
        }

        // ---------------
        // sphere - sphere
        // ---------------

        static inline bool Visit(
            wmath::geometry::shape::Sphere const & a_sph,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Sphere const & b_sph,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            return wmath::geometry::Intersects(
                a_sph, b_sph, b_transform[3] - a_transform[3]
                );
        }

        // ----------------
        // sphere - capsule
        // ----------------

        static inline bool Visit(
            wmath::geometry::shape::Sphere const & a_sph,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Capsule const & b_cap,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            return wmath::geometry::Intersects(
                a_sph, b_cap, a_inv_transform * b_transform
                );
        }


        // --------------
        // sphere - plane
        // --------------

        static inline bool Visit(
            wmath::geometry::shape::Sphere const & a_sph,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Plane const & b_pln,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            return wmath::geometry::Intersects(
                a_sph, b_pln
                );
        }

        // -------------
        // mesh - sphere
        // -------------

        static inline bool Visit(
            wmath::geometry::shape::Mesh const & a_mesh,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Sphere const & b_sph,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            return wmath::geometry::Intersects(
                a_mesh, b_sph, b_transform[3] - a_transform[3]
                );
        }

        // -----------------
        // capsule - capsule
        // -----------------

        static inline bool Visit(
            wmath::geometry::shape::Capsule const & a_cap,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Capsule const & b_cap,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            return wmath::geometry::Intersects(
                a_cap, b_cap, a_inv_transform * b_transform
                );
        }

        // ---------------
        // capsule - plane
        // ---------------

        static inline bool Visit(
            wmath::geometry::shape::Capsule const & a_cap,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Plane const & b_pln,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            // TODO intersection capsule plane
            return false;
        }

        // --------------
        // capsule - mesh
        // --------------

        static inline bool Visit(
            wmath::geometry::shape::Mesh const & a_mesh,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Capsule const & b_cap,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            return wmath::geometry::Intersects(
                a_mesh, b_cap, a_inv_transform * b_transform
                );
        }

        // -------------
        // plane - plane
        // -------------

        static inline bool Visit(
            wmath::geometry::shape::Plane const & a_pln,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Plane const & b_pln,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            // TODO plane intersection
            return false;
        }

        // ------------
        // Mesh - plane
        // ------------

        static inline bool Visit(
            wmath::geometry::shape::Mesh const & a_msh,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Plane const & b_pln,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            // TODO
            return false;
                

        }

        // -----------
        // mesh - mesh
        // -----------

        static inline bool Visit(
            wmath::geometry::shape::Mesh const & a_msh,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            wmath::geometry::shape::Mesh const & b_msh,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            ) {
            return wmath::geometry::Intersects(
                a_msh, b_msh, a_inv_transform * b_transform
                );
        }

        // ------------
        // Conmutations
        // ------------

        static inline bool Visit(
            auto const & a_shp,
            glm::mat4 const & a_transform,
            glm::mat4 const & a_inv_transform,
            auto const & b_shp,
            glm::mat4 const & b_transform,
            glm::mat4 const & b_inv_transform
            )
            {
                return Visit(b_shp, b_transform, b_inv_transform,
                             a_shp, a_transform, a_inv_transform);
            }        
        


    };

}
