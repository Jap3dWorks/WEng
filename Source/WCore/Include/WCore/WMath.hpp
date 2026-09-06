#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/Math.hpp"
#include "glm/matrix.hpp"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>

#include <cmath>
#include <array>
#include <limits>

namespace wcr::math {

    template<typename T=float>
    inline constexpr T PI = glm::pi<T>();

    inline constexpr glm::mat3 RotateZMatrix(float z_angle) {
        return {
            {glm::cos(z_angle), glm::sin(z_angle), 0.0},
            {-glm::sin(z_angle), glm::cos(z_angle), 0.0},
            {0.0, 0.0, 1.0}
        };
    }

    inline constexpr glm::mat3 RotateYMatrix(float y_angle) {
        return {
            {glm::cos(y_angle), 0.0, -glm::sin(y_angle)},
            {0.0, 1.0, 0.0},
            {glm::sin(y_angle), 0.0, glm::cos(y_angle)}
        };
        
    }

    inline constexpr glm::mat3 RotateXMatrix(float x_angle ) {
        return {        
            {1.0, 0.0, 0.0},
            {0.0, glm::cos(x_angle), glm::sin(x_angle)},
            {0.0, -glm::sin(x_angle), glm::cos(x_angle)}
        };
    }

    inline glm::mat3 RotateMatrix(glm::mat3 const & in_matrix,
                                  glm::vec3 in_rotation,
                                  wct::math::ERotationOrder in_rotation_order) {
        
        glm::mat3 rz = RotateZMatrix(in_rotation.z);
        glm::mat3 rx = RotateXMatrix(in_rotation.x);
        glm::mat3 ry = RotateYMatrix(in_rotation.y);

        switch(in_rotation_order) {
        case wct::math::ERotationOrder::xyz:
            return rz * ry * rx * in_matrix;
        case wct::math::ERotationOrder::xzy:
            return ry * rz * rx * in_matrix;
        case wct::math::ERotationOrder::yxz:
            return rz * rx * ry * in_matrix;
        case wct::math::ERotationOrder::yzx:
            return rx * rz * ry * in_matrix;
        case wct::math::ERotationOrder::zxy:
            return ry * rx * rz * in_matrix;
        case wct::math::ERotationOrder::zyx:
            return rx * ry * rz * in_matrix;
        }
    }
    
    inline glm::mat4 RotateMatrix(glm::mat4 in_matrix,
                                  glm::vec3 in_rotation,
                                  wct::math::ERotationOrder in_rotation_order) {

        glm::mat3 rotation{in_matrix};

        glm::vec4 position{in_matrix[3]};

        in_matrix = RotateMatrix(rotation, in_rotation, in_rotation_order);

        in_matrix[3] = position;

        return in_matrix;
    }

    inline glm::mat4 ToMat4(glm::vec3 in_position,
                            glm::vec3 in_eurler_rotation,
                            wct::math::ERotationOrder in_rotation_order,
                            glm::vec3 in_scale) {
        
        glm::mat3 scale{1};
        
        scale[0] *= in_scale.x;
        scale[1] *= in_scale.y;
        scale[2] *= in_scale.z;

        return glm::translate(glm::mat4{1}, in_position) *
            glm::mat4(RotateMatrix(glm::mat3{1}, in_eurler_rotation, in_rotation_order)) *
            glm::mat4{scale};

    }

    /**
     * x is 0, y is 1, z is 2
     * Returns an array of 3 elements with a numeric translation of each axis
     * following the rotation order, e.g zyx -> {2,1,0}
     */
    inline constexpr std::array<std::uint8_t, 3> ToNumeric(wct::math::ERotationOrder in_rotation_order) noexcept {
        switch(in_rotation_order) {
        case wct::math::ERotationOrder::xyz:
            return {0,1,2};
        case wct::math::ERotationOrder::xzy:
            return {0, 2, 1};
        case wct::math::ERotationOrder::yxz:
            return {1,0,2};
        case wct::math::ERotationOrder::yzx:
            return {1,2,0};
        case wct::math::ERotationOrder::zxy:
            return {2,0,1};
        case wct::math::ERotationOrder::zyx:
            return {2,1,0};
        }
    }

    inline constexpr glm::vec3 ToEulerRotation(glm::mat3 in_matrix, wct::math::ERotationOrder rot_order) {

        in_matrix[0] = glm::normalize(in_matrix[0]);
        in_matrix[1] = glm::normalize(in_matrix[1]);
        in_matrix[2] = glm::normalize(in_matrix[2]);

        auto angle_to_axis_plane = []
            (glm::vec3 vector, glm::vec3 axis_plane) constexpr -> float {
            float angle_cos = glm::dot(vector, axis_plane);
            return PI<float> * 0.5 - glm::acos(angle_cos);
        };

        auto angle_to_axis_in_plane = []
            (glm::vec3 vector, glm::vec3 axis) constexpr ->float {
            return glm::dot(vector, axis);
        };

        auto resolve_rotation = [&in_matrix]
            (auto && rot_mat_fn, auto && anglfn,
             glm::vec3 vector, glm::vec3 axis_plane) constexpr -> float {
            float result = anglfn(vector, axis_plane);
            in_matrix = rot_mat_fn(-result) * in_matrix;
            return result;
        };

        auto compute_rotation =
            [&in_matrix, &angle_to_axis_in_plane, &angle_to_axis_plane, &resolve_rotation]
            (std::array<std::uint8_t, 3> axis_order) constexpr -> glm::vec3 {
                glm::vec3 rotation{0};
                glm::mat3 identity{1};
                auto RotFn = std::array{RotateXMatrix, RotateYMatrix, RotateZMatrix};
                rotation[axis_order[2]] = resolve_rotation(
                    RotFn[axis_order[2]], angle_to_axis_plane,
                    in_matrix[axis_order[0]], identity[axis_order[1]]
                    );

                rotation[axis_order[1]] = resolve_rotation(
                    RotFn[axis_order[1]], angle_to_axis_in_plane,
                    in_matrix[axis_order[0]], identity[axis_order[0]]
                    );

                rotation[axis_order[0]] = resolve_rotation(
                    RotFn[axis_order[0]], angle_to_axis_in_plane,
                    in_matrix[axis_order[1]], identity[axis_order[1]]
                    );

                return rotation;
            };
            

        return compute_rotation(
            ToNumeric(rot_order)
            );
    }

    inline constexpr auto ToPositionRotationScale(
        glm::mat4 const & in_matrix,
        wct::math::ERotationOrder in_order
        ) {
        
        glm::vec3 position = in_matrix[3];
        glm::vec3 scale = {glm::length(in_matrix[0]), glm::length(in_matrix[1]), glm::length(in_matrix[2])};
        glm::vec3 rotation = ToEulerRotation(in_matrix, in_order);

        return std::tuple{position, rotation, scale};
    }

    inline constexpr glm::mat3 InvertOrthogonalBase(
        glm::mat3 orthonormal_base, float x_len, float y_len, float z_len
        ) {

        glm::mat3 transposed = glm::transpose(orthonormal_base);

        transposed[0][0] /= x_len;
        transposed[1][0] /= x_len;
        transposed[2][0] /= x_len;

        transposed[0][1] /= y_len;
        transposed[1][1] /= y_len;
        transposed[2][1] /= y_len;
        
        transposed[0][2] /= z_len;
        transposed[1][2] /= z_len;
        transposed[2][2] /= z_len;

        return transposed;

    }

    inline constexpr glm::mat3 InvertOrthogonalBase(glm::mat3 orthogonal_base) {

        float x_len = glm::length(orthogonal_base[0]);
        float y_len = glm::length(orthogonal_base[1]);
        float z_len = glm::length(orthogonal_base[2]);

        orthogonal_base[0] /= x_len;
        orthogonal_base[1] /= y_len;
        orthogonal_base[2] /= z_len;

        return InvertOrthogonalBase(orthogonal_base, x_len, y_len, z_len);

    }

    inline constexpr glm::mat4 InvertTransformMatrix(glm::mat4 transform_mat) {
        
        glm::mat3 rot = transform_mat;

        glm::mat3 inv_rot = InvertOrthogonalBase(rot);

        float inv_tw = 1/transform_mat[3][3];

        glm::vec3 inv_t{0.f};
        for (std::uint32_t i=0; i<3; i++) {
            inv_t[i] = - (inv_rot[0][i] * transform_mat[3][0] +
                          inv_rot[1][i] * transform_mat[3][1] +
                          inv_rot[2][i] * transform_mat[3][2]) * inv_tw;
        }

        glm::mat4 inv_transform{1.f};

        inv_transform = inv_rot;

        inv_transform[3] = glm::vec4{inv_t, inv_tw};

        return inv_transform;

    }


    template<typename T> requires requires {
        std::is_same_v<T, glm::mat3> || std::is_same_v<T,glm::mat4>;
    }
    inline constexpr bool AreEqual(T a, T b, float epsilon=2.5E-06) {

        for (std::uint32_t i=0; i<T::length(); i++) {
            for(std::uint32_t j=0; j<T::length(); j++) {
                if (std::abs(a[i][j] - b[i][j]) > epsilon) {
                    return false;
                }
            }
        }

        return true;
    }

}
