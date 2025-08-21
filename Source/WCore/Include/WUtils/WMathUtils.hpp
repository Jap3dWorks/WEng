#pragma once

#include "WCore/WCore.hpp"
#include "WStructs/WMathStructs.hpp"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>


namespace WMathUtils {

    inline glm::mat3 RotateMatrix(const glm::mat3 & in_matrix,
                                  const glm::vec3 & in_euler_rotation,
                                  const ERotationOrder & in_rotation_order) {
        
        glm::mat3 rz{
            {glm::cos(in_euler_rotation.z), glm::sin(in_euler_rotation.z), 0.0},
            {-glm::sin(in_euler_rotation.z), glm::cos(in_euler_rotation.z), 0.0},
            {0.0, 0.0, 1.0}
        };

        glm::mat3 rx{
            {1.0, 0.0, 0.0},
            {0.0, glm::cos(in_euler_rotation.x), glm::sin(in_euler_rotation.x)},
            {0.0, -glm::sin(in_euler_rotation.x), glm::cos(in_euler_rotation.x)}
        };

        glm::mat3 ry{
            {glm::cos(in_euler_rotation.y), 0.0, -glm::sin(in_euler_rotation.y)},
            {0.0, 1.0, 0.0},
            {glm::sin(in_euler_rotation.y), 0.0, glm::cos(in_euler_rotation.y)}
        };

        switch(in_rotation_order) {
        case ERotationOrder::xyz:
            return rz * ry * rx * in_matrix;
        case ERotationOrder::xzy:
            return ry * rz * rx * in_matrix;
        case ERotationOrder::yxz:
            return rz * rx * ry * in_matrix;
        case ERotationOrder::yzx:
            return rx * rz * ry * in_matrix;
        case ERotationOrder::zxy:
            return ry * rx * rz * in_matrix;
        case ERotationOrder::zyx:
            return rx * ry * rz * in_matrix;
        }
    }
    
    inline glm::mat4 RotateMatrix(const glm::mat4 & in_matrix,
                                  const glm::vec3 & in_euler_rotation,
                                  const ERotationOrder & in_rotation_order) {

        glm::mat3 rotation{in_matrix};

        rotation = RotateMatrix(rotation, in_euler_rotation, in_rotation_order);

        glm::mat4 result{rotation};
        result[3] = in_matrix[3];

        return result;
    }

    inline glm::mat4 ToMat4(const glm::vec3 & in_position,
                            const glm::vec3 & in_eurler_rotation,
                            const ERotationOrder & in_rotation_order,
                            const glm::vec3 & in_scale) {
        
        glm::mat3 scale{1};
        
        scale[0] *= in_scale.x;
        scale[1] *= in_scale.y;
        scale[2] *= in_scale.z;

        return glm::translate(glm::mat4{1}, in_position) *
            glm::mat4(RotateMatrix(glm::mat3{1}, in_eurler_rotation, in_rotation_order)) *
            glm::mat4{scale};

    }
}
