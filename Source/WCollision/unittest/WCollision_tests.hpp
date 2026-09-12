#pragma once

#include "WCollision/Shapes.hpp"
#include "WCollision/Collision.hpp"
#include "WCore/Math.hpp"

#include "WLog.hpp"
#include "glm/ext/matrix_transform.hpp"

#include <glm/glm.hpp>

namespace wcl::utests {

    struct BaseCollider {
        glm::mat4 transform{1.f};
        void Rotate(float angle, glm::vec3 direction) {
            transform = glm::rotate(
                transform,
                angle,
                glm::normalize(direction)
                );
        }

        void SetTranslation(glm::vec3 translation) {
            transform[3] = glm::vec4{translation, 1.f};
        }
    };

    struct BoxCollider : public BaseCollider {
        wcl::shapes::Box shape {5.f,5.f,5.f};
        glm::mat4 inv_transform {1.f};

        void Refresh() {
            inv_transform = wcr::math::InvertTransformMatrix(transform);
        }
    };

    struct SphereCollider : public BaseCollider {
        wcl::shapes::Sphere shape{
            .radius=5.f
        };
    };

    struct CapsuleCollider : public BaseCollider {
        wcl::shapes::Capsule shape{
            .half_length=5.f,
            .radius=5.f
        };
    };

    inline bool test_CheckAABB_1() {
        wcl::shapes::AABB a {
            .min = glm::vec3{0},
            .max = glm::vec3{2.f, 3.f, 2.f}
        };

        wcl::shapes::AABB b {
            .min = glm::vec3{1.5f, 2.5f, 1.5f},
            .max = glm::vec3{10.f, 8.f, 6.f}
        };

        if(!wcl::collision::Intersects(a, b)) {
            WFLOG("AABBs should intersect!");
            return false;
        }

        return true;
    }

    inline bool test_CheckAABB_2() {
        wcl::shapes::AABB a {
            .min = glm::vec3{0},
            .max = glm::vec3{2.f, 3.f, 2.f}
        };

        wcl::shapes::AABB b {
            .min = glm::vec3{2.5f, 3.5f, 2.5f},
            .max = glm::vec3{10.f, 8.f, 6.f}
        };

        if(wcl::collision::Intersects(a, b)) {
            WFLOG("AABBs should not intersect!");
            return false;
        }

        return true;
    }    

    inline bool test_BOX_1() {
        BoxCollider a_box{};
        a_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.transform[3] = glm::vec4{0.f, 0.f, 0.f, 1.f};
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.transform[3] = glm::vec4{7.f, 7.f, 7.f, 1.f};
        b_box.Refresh();

        bool check = wcl::collision::Intersects(
            a_box.shape, b_box.shape, a_box.inv_transform * b_box.transform
            );

        if (!check) {
            WFLOG("Box a and Box b should be detected as an intersection!");
            return false;
        }

        return true;
    }

    inline bool test_BOX_2() {
        BoxCollider a_box{};
        a_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.SetTranslation(glm::vec3{b_box.transform[0]} * 11.f);
        b_box.Refresh();

        glm::mat4 relative_transform = a_box.inv_transform * b_box.transform;

        bool check = wcl::collision::Intersects(
            a_box.shape, b_box.shape, relative_transform
            );

        relative_transform = b_box.inv_transform * a_box.transform;

        check = check && wcl::collision::Intersects(
            b_box.shape, a_box.shape, relative_transform
            );

        if (check) {
            WFLOG("Box a and box b should not be detected as an intersection!");
            return false;
        }

        return true;
    }

    inline bool test_BOX_3() {
        BoxCollider a_box{};
        a_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.SetTranslation(glm::vec3{b_box.transform[0]} * -9.f);
        b_box.Refresh();

        glm::mat4 relative_transform = a_box.inv_transform * b_box.transform;

        bool check = wcl::collision::Intersects(
            a_box.shape, b_box.shape, relative_transform
            );

        relative_transform = b_box.inv_transform * a_box.transform;

        check = check && wcl::collision::Intersects(
            b_box.shape, a_box.shape, relative_transform
            );

        if (!check) {
            WFLOG("Box a and box b should be detected as an intersection!");
            return false;
        }

        return true;
    }


    inline bool test_BOX_4() {
        BoxCollider a_box{};
        a_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.SetTranslation(glm::vec3{0.f, 9.f, 0.f});
        b_box.Refresh();

        glm::mat4 relative_transform = a_box.inv_transform * b_box.transform;

        bool check = wcl::collision::Intersects(
            a_box.shape, b_box.shape, relative_transform
            );

        relative_transform = b_box.inv_transform * a_box.transform;

        check = check && wcl::collision::Intersects(
            b_box.shape, a_box.shape, relative_transform
            );

        if (!check) {
            WFLOG("Box a and box b should be detected as an intersection!");
            return false;
        }

        return true;
    }

    inline bool test_BOX_capsule_1() {
        BoxCollider axis_box{};

        CapsuleCollider capsule{};
        capsule.shape.radius = 5.1f;
        capsule.SetTranslation(glm::vec3{0.f, 0.f, 10.f});

        bool check = wcl::collision::Intersects(
            axis_box.shape, capsule.shape, capsule.transform
            );

        if(!check) {
            WFLOG("Box and capsule should be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_BOX_capsule_2() {
        BoxCollider axis_box{};

        CapsuleCollider capsule{};
        capsule.SetTranslation(glm::vec3{0.f, 0.f, 9.9f});

        bool check = wcl::collision::Intersects(
            axis_box.shape, capsule.shape, capsule.transform
            );

        if (!check) {
            WFLOG("Box and capsule should be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_BOX_capsule_3() {
        BoxCollider axis_box{};

        CapsuleCollider capsule{};
        capsule.shape.radius = 5.1f;
        capsule.Rotate(wcr::math::PI<float> * (0.25), {0.f, 1.f, 0.f});
        capsule.Rotate(wcr::math::PI<float> * (-0.5), {1.f, 0.f, -1.f});
        capsule.SetTranslation(
            glm::vec3{axis_box.shape.x, axis_box.shape.y, axis_box.shape.z} +
            glm::normalize(glm::vec3{1.f, 1.f, 1.f}) * 5.f
            );

        bool check = wcl::collision::Intersects(
            axis_box.shape, capsule.shape, capsule.transform
            );

        if (!check) {
            WFLOG("Box and capsule should be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_BOX_capsule_4() {
        BoxCollider axis_box{};
        axis_box.shape = {5.f, 5.f, 5.f};

        CapsuleCollider capsule{};
        capsule.shape.half_length=5.f;
        capsule.shape.radius = 4.9f;
        capsule.Rotate(wcr::math::PI<float> * (0.25), {0.f, 1.f, 0.f});
        capsule.Rotate(wcr::math::PI<float> * (-0.5), {1.f, 0.f, -1.f});
        capsule.SetTranslation(
            glm::vec3{axis_box.shape.x, axis_box.shape.y, axis_box.shape.z} +
            glm::normalize(glm::vec3{1.f, 1.f, 1.f}) * 5.f
            );

        bool check = wcl::collision::Intersects(
            axis_box.shape, capsule.shape, capsule.transform
            );

        if (check) {
            WFLOG("Box and capsule should NOT be intersecting!");
            return false;
        }

        return true;
    }

    inline bool test_sphere_1() {
        SphereCollider s1;
        SphereCollider s2;
        s2.shape.radius=5.1f;

        s2.SetTranslation(glm::normalize(glm::vec3{1.f, 1.f, 1.f}) * 10.f);

        bool check = wcl::collision::Intersects(
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

        bool check = wcl::collision::Intersects(
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
        c2.Rotate(wcr::math::PI<float> * (0.25), {0.f, 1.f, 0.f});
        c2.Rotate(wcr::math::PI<float> * (-0.5), {1.f, 0.f, -1.f});
        c2.SetTranslation(
            glm::normalize(glm::vec3{1.f, 1.f, 1.f}) * 10.f
            );

        bool check = wcl::collision::Intersects(
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
        c2.Rotate(wcr::math::PI<float> * (0.25), {0.f, 1.f, 0.f});
        c2.Rotate(wcr::math::PI<float> * (-0.5), {1.f, 0.f, -1.f});
        c2.SetTranslation(
            glm::normalize(glm::vec3{1.f, 1.f, 1.f}) * 10.f
            );

        bool check = wcl::collision::Intersects(
            s1.shape, c2.shape, c2.transform
            );

        if (check) {
            WFLOG("Sphere and capsule should NOT be intersecting!");
            return false;
        }

        return true;
    }
    

    inline bool test_capsule_1() {
        CapsuleCollider c1;
        c1.Rotate(wcr::math::PI<float> * (-0.25), {0.f, 1.f, 0.f});

        CapsuleCollider c2;
        c2.shape.radius=5.1f;
        c2.Rotate(wcr::math::PI<float> * (0.25), {0.f, 1.f, 0.f});
        c2.Rotate(wcr::math::PI<float> * (-0.5), {1.f, 0.f, -1.f});
        c2.SetTranslation(
            glm::normalize(glm::vec3{1.f, 1.f, 1.f}) * 10.f
            );

        bool check = wcl::collision::Intersects(
            c1.shape, c2.shape, c2.transform
            );

        if (!check) {
            WFLOG("Capsules should be intersecting!");
            return false;
        }

        return true;
    }

    // TODO check this test
    inline bool test_capsule_2() {
        CapsuleCollider c1;
        c1.Rotate(wcr::math::PI<float> * (-0.25), {0.f, 1.f, 0.f});

        CapsuleCollider c2;
        c2.shape.radius=4.9f;
        c2.Rotate(wcr::math::PI<float> * (0.25), {0.f, 1.f, 0.f});
        c2.Rotate(wcr::math::PI<float> * (-0.5), {1.f, 0.f, -1.f});
        c2.SetTranslation(
            glm::normalize(glm::vec3{1.f, 1.f, 1.f}) * 10.f
            );

        bool check = wcl::collision::Intersects(
            c1.shape, c2.shape, c2.transform
            );

        if (check) {
            WFLOG("Capsules should NOT be intersecting!");
            return false;
        }

        return true;
    }

}
