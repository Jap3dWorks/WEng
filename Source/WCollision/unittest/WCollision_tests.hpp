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
        wcl::shapes::Box box {5.f,5.f,5.f};
        glm::mat4 inv_transform {1.f};

        void Refresh() {
            inv_transform = wcr::math::InvertTransformMatrix(transform);
        }
    };

    struct CapsuleCollider : public BaseCollider {
        wcl::shapes::Capsule capsule{};
    };

    inline bool test1_CheckAABB_1() {
        wcl::shapes::AABB a {
            .min = glm::vec3{0},
            .max = glm::vec3{2.f, 3.f, 2.f}
        };

        wcl::shapes::AABB b {
            .min = glm::vec3{1.5f, 2.5f, 1.5f},
            .max = glm::vec3{10.f, 8.f, 6.f}
        };

        if(!wcl::collision::CheckAABB(a, b)) {
            WFLOG("AABBs should intersect!");
            return false;
        }

        return true;
    }

    inline bool test2_CheckAABB_2() {
        wcl::shapes::AABB a {
            .min = glm::vec3{0},
            .max = glm::vec3{2.f, 3.f, 2.f}
        };

        wcl::shapes::AABB b {
            .min = glm::vec3{2.5f, 3.5f, 2.5f},
            .max = glm::vec3{10.f, 8.f, 6.f}
        };

        if(wcl::collision::CheckAABB(a, b)) {
            WFLOG("AABBs should not intersect!");
            return false;
        }

        return true;
    }    

    inline bool test3_OBB_1() {
        BoxCollider a_box{};
        a_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.transform[3] = glm::vec4{0.f, 0.f, 0.f, 1.f};
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.transform[3] = glm::vec4{7.f, 7.f, 7.f, 1.f};
        b_box.Refresh();

        bool check = wcl::collision::CheckBoxIntersection(
            a_box.box, b_box.box, a_box.inv_transform * b_box.transform
            );

        if (!check) {
            WFLOG("Box a and Box b should be detected as an intersection!");
            return false;
        }

        return true;
    }

    inline bool test4_OBB_2() {
        BoxCollider a_box{};
        a_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.SetTranslation(glm::vec3{b_box.transform[0]} * 11.f);
        b_box.Refresh();

        glm::mat4 relative_transform = a_box.inv_transform * b_box.transform;

        bool check = wcl::collision::CheckBoxIntersection(
            a_box.box, b_box.box, relative_transform
            );

        relative_transform = b_box.inv_transform * a_box.transform;

        check = check && wcl::collision::CheckBoxIntersection(
            b_box.box, a_box.box, relative_transform
            );

        if (check) {
            WFLOG("Box a and box b should not be detected as an intersection!");
            return false;
        }

        return true;
    }

    inline bool test5_OBB_3() {
        BoxCollider a_box{};
        a_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.SetTranslation(glm::vec3{b_box.transform[0]} * -9.f);
        b_box.Refresh();

        glm::mat4 relative_transform = a_box.inv_transform * b_box.transform;

        bool check = wcl::collision::CheckBoxIntersection(
            a_box.box, b_box.box, relative_transform
            );

        relative_transform = b_box.inv_transform * a_box.transform;

        check = check && wcl::collision::CheckBoxIntersection(
            b_box.box, a_box.box, relative_transform
            );

        if (!check) {
            WFLOG("Box a and box b should be detected as an intersection!");
            return false;
        }

        return true;
    }


    inline bool test6_OBB_4() {
        BoxCollider a_box{};
        a_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        a_box.Refresh();

        BoxCollider b_box{};
        b_box.Rotate(wcr::math::PI<float> * 0.5f, {1.f, 1.f, 1.f});
        b_box.SetTranslation(glm::vec3{0.f, 9.f, 0.f});
        b_box.Refresh();

        glm::mat4 relative_transform = a_box.inv_transform * b_box.transform;

        bool check = wcl::collision::CheckBoxIntersection(
            a_box.box, b_box.box, relative_transform
            );

        relative_transform = b_box.inv_transform * a_box.transform;

        check = check && wcl::collision::CheckBoxIntersection(
            b_box.box, a_box.box, relative_transform
            );

        if (!check) {
            WFLOG("Box a and box b should be detected as an intersection!");
            return false;
        }

        return true;
    }

    inline bool test7_OBB_capsule_1() {

        BoxCollider axis_box{};
        CapsuleCollider capsule{};

        capsule.capsule.half_length=8.f;
        capsule.SetTranslation(glm::vec3{0.f, 0.f, 6.f});

        bool check = wcl::collision::CheckIntersection(
            axis_box.box, capsule.capsule, capsule.transform
            );

        if(!check) {
            WFLOG("Box and capsule should be intersecting!");
            return false;
        }

        return true;
    }

}
