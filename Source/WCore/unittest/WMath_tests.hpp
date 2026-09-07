#pragma once

#include "WCore/WMath.hpp"
#include "WCoreTypes/Math.hpp"
#include "WLog.hpp"
#include "glm/ext/matrix_transform.hpp"

#include <glm/glm.hpp>

namespace wcr::math::utests {

    template<typename T>
    inline bool EnsureInverse(T mat, T inv) {
        T check = mat * inv;
        if (! wcr::math::AreEqual(check, T{1.f}, 1.0E-05)) {
            WFLOG("Matrix * Inverse needs to be equal to the Identity!");
            return false;
        }

        return true;
    }

    inline bool InvertMatrix_test1() {

        glm::mat4 identity{1.f};
        glm::mat4 inverse = wcr::math::InvertTransformMatrix(identity);

        return EnsureInverse(identity, inverse);
    }

    inline bool InvertMatrix_test2() {
        glm::mat3 rot = wcr::math::RotateMatrix(
            glm::mat3{1.f},
            glm::vec3{
                wcr::math::PI<float> * 0.38,
                wcr::math::PI<float> * 1.77,
                wcr::math::PI<float> * 0.17
            },
            wct::math::ERotationOrder::xyz
            );

        glm::mat3 inverse = wcr::math::InvertOrthogonalBase(rot);

        return EnsureInverse(rot, inverse);
    }

    inline bool InvertMatrix_test3() {

        glm::mat4 transform{1.f};
        transform[3] = glm::vec4{2.f, 4.f, 10.f, 1.f};

        glm::mat4 inverse = wcr::math::InvertTransformMatrix(transform);

        return EnsureInverse(transform, inverse);
    }

    inline bool InvertMatrix_test4() {

        glm::mat4 transform {1.f};

        transform = glm::rotate(transform,
                                wcr::math::PI<float> * 0.38f,
                                glm::vec3{.4f, 0.19f, -.9f});

        transform = glm::translate(transform, {12.f, 10.f, 9.f});

        glm::mat4 inverse = wcr::math::InvertTransformMatrix(transform);

        return EnsureInverse(transform, inverse);
        
    }

    inline bool InvertMatrix_test5() {

        glm::mat4 transform {1.f};

        transform = glm::rotate(transform,
                                wcr::math::PI<float> * 0.38f,
                                glm::vec3{.4f, 0.19f, -.9f});

        transform = glm::scale(transform, glm::vec3{3.f, 2.f, 10});

        transform[3] = glm::vec4{50.f, 80.f, 33.401, 1.f};

        glm::mat4 inverse = wcr::math::InvertTransformMatrix(transform);

        return EnsureInverse(transform, inverse);
        
    }

}

