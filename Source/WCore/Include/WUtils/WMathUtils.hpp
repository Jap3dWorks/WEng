#pragma once

#include "WCore/WCore.hpp"
#include "WStructs/WMathStructs.hpp"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>


// #include

namespace WMathUtils {
    
    inline void RotateMatrix(glm::mat4 & out_matrix,
                             const glm::vec3 & in_euler_rotation,
                             const ERotationOrder & in_rotation_order) {

        switch(in_rotation_order) {
        case ERotationOrder::xyz:
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.x, {1.0, 0.0, 0.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.y, {0.0, 1.0, 0.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.z, {0.0, 0.0, 1.0});
            break;
        case ERotationOrder::xzy:
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.x, {1.0, 0.0, 0.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.z, {0.0, 0.0, 1.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.y, {0.0, 1.0, 0.0});
            break;
        case ERotationOrder::yxz:
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.y, {0.0, 1.0, 0.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.x, {1.0, 0.0, 0.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.z, {0.0, 0.0, 1.0});
            break;
        case ERotationOrder::yzx:
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.y, {0.0, 1.0, 0.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.z, {0.0, 0.0, 1.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.x, {1.0, 0.0, 0.0});
            break;
        case ERotationOrder::zxy:
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.z, {0.0, 0.0, 1.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.x, {1.0, 0.0, 0.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.y, {0.0, 1.0, 0.0});
            break;
        case ERotationOrder::zyx:
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.z, {0.0, 0.0, 1.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.y, {0.0, 1.0, 0.0});
            out_matrix = glm::rotate(out_matrix, in_euler_rotation.x, {1.0, 0.0, 0.0});
            break;
        }
    }

    inline glm::mat4 ToMat4(const glm::vec3 & in_position,
                            const glm::vec3 & in_eurler_rotation,
                            const ERotationOrder & in_rotation_order,
                            const glm::vec3 & in_scale) {
        
        glm::mat4 result{1};
        
        RotateMatrix(result, in_eurler_rotation, in_rotation_order);

        result[0] *=in_scale.x;
        result[1] *=in_scale.y;
        result[2] *=in_scale.z;

        result[3] = {in_position, 1.0};
    
        return result;
    }
}
