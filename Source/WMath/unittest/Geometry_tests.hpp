#pragma once

#include "WLog.hpp"

#include "WMath/Geometry/Shape.hpp"
#include "WMath/Geometry/IntersectionCheck.hpp"
#include "WMath/LinAlgebra.hpp"

#include "WMath/Numerical.hpp"
#include "glm/ext/matrix_transform.hpp"

#include <glm/glm.hpp>

namespace wmath::geometry::utests {

    struct BaseCollider {
        glm::mat4 transform{1.f};
        
        void Rotate(float angle, glm::vec3 direction) {
            
            glm::mat3 rot = glm::rotate(
                glm::mat4{1.f},
                angle,
                glm::normalize(direction)
                );

            glm::vec4 pos = transform[3];
            glm::mat4 mat = rot * glm::mat3(transform);
            mat[3] = pos;

            transform = mat;
        }

        void SetTranslation(glm::vec3 translation) {
            transform[3] = glm::vec4{translation, 1.f};
        }
    };

    struct BoxCollider : public BaseCollider {
        wmath::geometry::shape::Box shape {5.f,5.f,5.f};
        glm::mat4 inv_transform {1.f};

        void Refresh() {
            inv_transform = wmath::lin_algbr::InvertTransformMatrix(transform);
        }
    };

    struct SphereCollider : public BaseCollider {
        wmath::geometry::shape::Sphere shape{
            .radius=5.f
        };
    };

    struct CapsuleCollider : public BaseCollider {
        wmath::geometry::shape::Capsule shape{
            .half_length=5.f,
            .radius=5.f
        };
    };

    struct MeshCollider : public BaseCollider {
        wmath::geometry::shape::Mesh shape{
            .vertices = std::vector{
                glm::vec3{2, -2, 0},
                glm::vec3{-2, 0, 2},
                glm::vec3{0, 2, -2},
                glm::vec3{3, -2, 1},
                glm::vec3{-2.5, 1, 3},
            },
            .indices = std::vector<std::uint32_t>{
                0,1,2,0,3,1,4,1,2
            }
        
        };
    };

    inline bool test_CheckAABB_1() {
        wmath::geometry::shape::AABB a {
            .min = glm::vec3{0},
            .max = glm::vec3{2.f, 3.f, 2.f}
        };

        wmath::geometry::shape::AABB b {
            .min = glm::vec3{1.5f, 2.5f, 1.5f},
            .max = glm::vec3{10.f, 8.f, 6.f}
        };

        if(!wmath::geometry::Intersects(a, b)) {
            return false;
        }

        return true;
    }

    inline bool test_CheckAABB_2() {
        wmath::geometry::shape::AABB a {
            .min = glm::vec3{0},
            .max = glm::vec3{2.f, 3.f, 2.f}
        };

        wmath::geometry::shape::AABB b {
            .min = glm::vec3{2.5f, 3.5f, 2.5f},
            .max = glm::vec3{10.f, 8.f, 6.f}
        };

        if(wmath::geometry::Intersects(a, b)) {
            // WFLOG("AABBs should not intersect!");
            return false;
        }

        return true;
    }    

    inline bool test_box_1() {
        BoxCollider a_box{};
        a_box.Rotate(wmath::numerical::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.transform[3] = glm::vec4{0.f, 0.f, 0.f, 1.f};
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wmath::numerical::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.transform[3] = glm::vec4{7.f, 7.f, 7.f, 1.f};
        b_box.Refresh();

        bool check = wmath::geometry::Intersects(
            a_box.shape, b_box.shape, a_box.inv_transform * b_box.transform
            );

        if (!check) {
            // WFLOG("Box a and Box b should be detected as an intersection!");
            return false;
        }

        return true;
    }

    inline bool test_box_2() {
        BoxCollider a_box{};
        a_box.Rotate(wmath::numerical::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wmath::numerical::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.SetTranslation(glm::vec3{b_box.transform[0]} * 11.f);
        b_box.Refresh();

        glm::mat4 relative_transform = a_box.inv_transform * b_box.transform;

        bool check = wmath::geometry::Intersects(
            a_box.shape, b_box.shape, relative_transform
            );

        relative_transform = b_box.inv_transform * a_box.transform;

        check = check && wmath::geometry::Intersects(
            b_box.shape, a_box.shape, relative_transform
            );

        if (check) {
            WFLOG("Box a and box b should not be detected as an intersection!");
            return false;
        }

        return true;
    }

    inline bool test_box_3() {
        BoxCollider a_box{};
        a_box.Rotate(wmath::numerical::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wmath::numerical::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.SetTranslation(glm::vec3{b_box.transform[0]} * -9.f);
        b_box.Refresh();

        glm::mat4 relative_transform = a_box.inv_transform * b_box.transform;

        bool check = wmath::geometry::Intersects(
            a_box.shape, b_box.shape, relative_transform
            );

        relative_transform = b_box.inv_transform * a_box.transform;

        check = check && wmath::geometry::Intersects(
            b_box.shape, a_box.shape, relative_transform
            );

        if (!check) {
            WFLOG("Box a and box b should be detected as an intersection!");
            return false;
        }

        return true;
    }

    inline bool test_box_4() {
        BoxCollider a_box{};
        a_box.Rotate(wmath::numerical::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wmath::numerical::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.SetTranslation(glm::vec3{0.f, 9.f, 0.f});
        b_box.Refresh();

        glm::mat4 relative_transform = a_box.inv_transform * b_box.transform;

        bool check = wmath::geometry::Intersects(
            a_box.shape, b_box.shape, relative_transform
            );

        relative_transform = b_box.inv_transform * a_box.transform;

        check = check && wmath::geometry::Intersects(
            b_box.shape, a_box.shape, relative_transform
            );

        if (!check) {
            WFLOG("Box a and box b should be detected as an intersection!");
            return false;
        }

        return true;
    }

    inline bool test_box_capsule_1() {
        BoxCollider axis_box{};

        CapsuleCollider capsule{};
        capsule.shape.radius = 5.1f;
        capsule.SetTranslation(glm::vec3{0.f, 0.f, 10.f});

        bool check = wmath::geometry::Intersects(
            axis_box.shape, capsule.shape, capsule.transform
            );

        if(!check) {
            WFLOG("Box and capsule should be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_box_capsule_2() {
        BoxCollider axis_box{};

        CapsuleCollider capsule{};
        capsule.SetTranslation(glm::vec3{0.f, 0.f, 9.9f});

        bool check = wmath::geometry::Intersects(
            axis_box.shape, capsule.shape, capsule.transform
            );

        if (!check) {
            WFLOG("Box and capsule should be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_box_capsule_3() {
        BoxCollider axis_box{};

        CapsuleCollider capsule{};
        capsule.shape.radius = 5.1f;
        capsule.Rotate(wmath::numerical::PI<float> * (0.25), {0.f, 1.f, 0.f});
        capsule.Rotate(wmath::numerical::PI<float> * (-0.5), {1.f, 0.f, -1.f});
        capsule.SetTranslation(
            glm::vec3{axis_box.shape.x, axis_box.shape.y, axis_box.shape.z} +
            glm::normalize(glm::vec3{1.f, 1.f, 1.f}) * 5.f
            );

        bool check = wmath::geometry::Intersects(
            axis_box.shape, capsule.shape, capsule.transform
            );

        if (!check) {
            WFLOG("Box and capsule should be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_box_capsule_4() {
        BoxCollider axis_box{};
        axis_box.shape = {5.f, 5.f, 5.f};

        CapsuleCollider capsule{};
        capsule.shape.half_length=5.f;
        capsule.shape.radius = 4.9f;
        capsule.Rotate(wmath::numerical::PI<float> * (0.25), {0.f, 1.f, 0.f});
        capsule.Rotate(wmath::numerical::PI<float> * (-0.5), {1.f, 0.f, -1.f});
        capsule.SetTranslation(
            glm::vec3{axis_box.shape.x, axis_box.shape.y, axis_box.shape.z} +
            glm::normalize(glm::vec3{1.f, 1.f, 1.f}) * 5.f
            );

        bool check = wmath::geometry::Intersects(
            axis_box.shape, capsule.shape, capsule.transform
            );

        if (check) {
            WFLOG("Box and capsule should NOT be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_box_plane_1() {
        BoxCollider b{};
        b.shape = {.x=5.1, .y=5.1, .z=5.1};
        
        wmath::geometry::shape::Plane p{
            .n=glm::vec3{-1.f, 1.f, -1.f},
            .dist=5.f
        };

        bool check = wmath::geometry::Intersects(
            b.shape, p
            );

        if (!check) {
            WFLOG("Box and plane should be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_box_plane_2() {
        BoxCollider b{};
        b.shape = {.x=4.9, .y=4.9, .z=4.9};

        wmath::geometry::shape::Plane p {
            .n=glm::vec3{-1.f, 1.f, -1.f},
            .dist=5.f
        };

        bool check = wmath::geometry::Intersects(
            b.shape, p
            );

        if (check) {
            WFLOG("Box and plane should NOT be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_box_triangle_1() {
        BoxCollider b{};
        b.shape = {.x=5.1f, .y=5.1f, .z=5.1f};

        glm::vec3 point{5.f, 5.f, 5.f};

        std::array<glm::vec3, 3> tri {
            point + glm::vec3{2, -2, 0},
            point + glm::vec3{-2, 0, 2},
            point + glm::vec3{0, 2, -2}
        };

        bool check = wmath::geometry::Intersects(
            b.shape, tri
            );

        if (!check) {
            WFLOG("Box and triangle should be intersecting!");
            return false;
        }
        
        return true;
    }

    inline bool test_box_triangle_2() {
        BoxCollider b{};
        b.shape = {.x=4.9, .y=4.9, .z=4.9};

        glm::vec3 point{5.f, 5.f, 5.f};

        std::array<glm::vec3, 3> tri {
            point + glm::vec3{2, -2, 0},
            point + glm::vec3{-2, 0, 2},
            point + glm::vec3{0, 2, -2}
        };

        bool check = wmath::geometry::Intersects(
            b.shape, tri
            );

        if (check) {
            WFLOG("Box and triangle should NOT be intersecting!");
            return false;
        }
        
        return true;
    }

    inline bool test_box_mesh_1() {
        BoxCollider b{};
        b.shape = {.x=5.1, .y=5.1, .z=5.1};

        MeshCollider m{};
        m.SetTranslation({5.f, 5.f, 5.f});

        bool check = wmath::geometry::Intersects(
            b.shape, m.shape, m.transform
            );

        if(!check) {
            WFLOG("Box and mesh should be intersecting!");
            return false;
        }
        return true;
    }

    inline bool test_box_mesh_2() {
        BoxCollider b{};
        b.shape = {.x=4.9, .y=4.9, .z=4.9};

        MeshCollider m{};
        m.SetTranslation({5.f, 5.f, 5.f});

        bool check = wmath::geometry::Intersects(
            b.shape, m.shape, m.transform
            );

        if(check) {
            WFLOG("Box and mesh should NOT be intersecting!");
            return false;
        }
        return true;
    }    

    inline bool test_sphere_1() {
        SphereCollider s1;
        SphereCollider s2;
        s2.shape.radius=5.1f;

        s2.SetTranslation(glm::normalize(glm::vec3{1.f, 1.f, 1.f}) * 10.f);

        bool check = wmath::geometry::Intersects(
            s1.shape, s2.shape, s2.transform[3]
            );

        if (!check) {
            WFLOG("Spheres should be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_sphere_2() {
        SphereCollider s1;
        SphereCollider s2;
        s2.shape.radius=4.9f;

        s2.SetTranslation(glm::normalize(glm::vec3{1.f, 1.f, 1.f}) * 10.f);

        bool check = wmath::geometry::Intersects(
            s1.shape, s2.shape, s2.transform[3]
            );

        if (check) {
            WFLOG("Spheres should NOT be intersecting!");
            return false;
        }

        return true;
    }    

    inline bool test_sphere_capsule_1() {
        SphereCollider s1;
        CapsuleCollider c2;

        c2.shape.radius=5.1f;
        c2.Rotate(wmath::numerical::PI<float> * (0.5), {0.f, 1.f, 0.f});
        c2.Rotate(wmath::numerical::PI<float> * (-0.25), {1.f, 0.f, 0.f});
        c2.SetTranslation(
            glm::normalize(glm::vec3{0.f, 1.f, -1.f}) * 10.f
            );

        bool check = wmath::geometry::Intersects(
            s1.shape, c2.shape, c2.transform
            );

        if (!check) {
            WFLOG("Sphere and capsule should be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_sphere_capsule_2() {
        SphereCollider s1;
        CapsuleCollider c2;

        c2.shape.radius=4.9f;
        c2.Rotate(wmath::numerical::PI<float> * (0.5), {0.f, 1.f, 0.f});
        c2.Rotate(wmath::numerical::PI<float> * (-0.25), {1.f, 0.f, 0.f});
        c2.SetTranslation(
            glm::normalize(glm::vec3{0.f, 1.f, -1.f}) * 10.f
            );

        bool check = wmath::geometry::Intersects(
            s1.shape, c2.shape, c2.transform
            );

        if (check) {
            WFLOG("Sphere and capsule should NOT be intersecting!");
            return false;
        }

        return true;
    }
    

    inline bool test_point_capsule_1() {
        CapsuleCollider c1;

        c1.shape.radius = 5.1f;
        c1.SetTranslation(glm::vec3(0.f, 5.f, 0.f));

       bool check = wmath::geometry::Intersects(
            c1.shape, c1.transform
            );

        if (!check) {
            WFLOG("Capsule and point should be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_point_capsule_2() {
        CapsuleCollider c1;

        c1.shape.radius = 4.9f;
        c1.SetTranslation(glm::vec3(0.f, 5.f, 0.f));

        bool check = wmath::geometry::Intersects(
            c1.shape, c1.transform
            );

        if (check) {
            WFLOG("Capsule and point should NOT be intersecting!");
            return false;
        }

        return true;
    }    

    inline bool test_capsule_1() {
        CapsuleCollider c1;
        c1.shape.radius=5.1f;

        CapsuleCollider c2;
        c2.shape.radius=5.f;
        c2.Rotate(wmath::numerical::PI<float> * (0.5), {0.f, 1.f, 0.f});
        c2.Rotate(wmath::numerical::PI<float> * (-0.25), {1.f, 0.f, 0.f});
        c2.SetTranslation(
            glm::normalize(glm::vec3{0.f, 1.f, -1.f}) * 10.f
            );

        bool check = wmath::geometry::Intersects(
            c1.shape, c2.shape, c2.transform
            );

        if (!check) {
            WFLOG("Capsules should be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_capsule_2() {
        CapsuleCollider c1;
        c1.shape.radius = 4.9f;
        
        CapsuleCollider c2;
        c2.shape.radius=5.f;
        c2.Rotate(wmath::numerical::PI<float> * (0.5), {0.f, 1.f, 0.f});
        c2.Rotate(wmath::numerical::PI<float> * (-0.25), {1.f, 0.f, 0.f});
        c2.SetTranslation(
            glm::normalize(glm::vec3{0.f, 1.f, -1.f}) * 10.f
            );

        bool check = wmath::geometry::Intersects(
            c1.shape, c2.shape, c2.transform
            );

        if (check) {
            WFLOG("Capsules should NOT be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_capsule_mesh_1() {
        MeshCollider m1;
        CapsuleCollider c1;
        c1.shape.radius=5.1;
        c1.Rotate(wmath::numerical::PI<float> * 0.5, glm::vec3{0,0,1});
        c1.SetTranslation({8.f, 0.f, -2.f});

        bool check = wmath::geometry::Intersects(
            m1.shape, c1.shape, c1.transform
            );

        if (!check) {
            WFLOG("Capsule and mesh should be intersecting!");
            return false;
        }
        
        return true;
    }

    inline bool test_capsule_mesh_2() {
        MeshCollider m1;
        CapsuleCollider c1;
        
        c1.shape.radius=4.9;
        c1.Rotate(wmath::numerical::PI<float> * 0.5, glm::vec3{0,0,1});
        c1.SetTranslation({8.f, 0.f, -2.f});

        bool check = wmath::geometry::Intersects(
            m1.shape, c1.shape, c1.transform
            );

        if (check) {
            WFLOG("Capsule and mesh should NOT be intersecting!");
            return false;
        }
        
        return true;
    }

    inline bool test_mesh_1() {
        return false;
    }

    inline bool test_mesh_2() {
        return false;
    }

}
