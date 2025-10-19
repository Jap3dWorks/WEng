#pragma once

#include "WStructs/WComponentStructs.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WUtils/WMathUtils.hpp"
#include <glm/ext/quaternion_transform.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/matrix.hpp>

namespace WRenderUtils {
    inline WUBOCameraStruct ToUBOCameraStruct(
        const WCameraPropertiesStruct & in_camera,
        const WTransformStruct & in_transform,
        float in_aspect
        ) {
        WUBOCameraStruct ubo_camera;
        ubo_camera.proj = glm::perspective(
            glm::radians(in_camera.angle_of_view),
            in_aspect,
            in_camera.near_clipping,
            in_camera.far_clipping
            );

        glm::mat3 orient{in_transform.transform_matrix};
        glm::mat4 o = glm::transpose(orient);

        glm::vec3 translation{in_transform.transform_matrix[3]};
        glm::mat4 t = glm::translate(glm::mat4{1}, -translation);

        ubo_camera.view = o * t;

        ubo_camera.proj[1][1] *= -1;
        
        return ubo_camera;
    }

    inline WUBOGraphicsStruct ToUBOGraphicsStruct(
        const WTransformStruct & in_transform
        ) {

        glm::mat3 normal_matrix = glm::transpose(
            glm::inverse(
                glm::mat3(in_transform.transform_matrix)
                )
            );

        return {
            in_transform.transform_matrix,
            normal_matrix
        };
    }

}
