#pragma once

#include "WComponents/WCameraComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WUtils/WMath.hpp"
#include <glm/ext/quaternion_transform.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/matrix.hpp>

namespace wrd::render {

    inline wct::render::WCameraUBO ToUBOCameraStruct(
        const WCameraComponent & in_camera,
        const WTransformComponent & in_transform,
        float in_aspect
        ) {
        wct::render::WCameraUBO ubo_camera;
        ubo_camera.proj = glm::perspective(
            glm::radians(in_camera.Get_angle_of_view()),
            in_aspect,
            in_camera.Get_near_clipping(),
            in_camera.Get_far_clipping()
            );

        glm::mat3 orient{in_transform.Get_transform_matrix()};
        glm::mat4 o = glm::transpose(orient);

        glm::vec3 translation{in_transform.Get_transform_matrix()[3]};
        glm::mat4 t = glm::translate(glm::mat4{1}, -translation);

        ubo_camera.view = o * t;

        ubo_camera.proj[1][1] *= -1;
        
        return ubo_camera;
    }

    inline wct::render::WModelUBO ToUBOGraphicsStruct(
        const WTransformComponent & in_transform
        // const WTransformStruct & in_transform
        ) {

        glm::mat3 tmp = glm::transpose(
            glm::inverse(
                glm::mat3(in_transform.Get_transform_matrix())
                )
            );

        // row/col 4 is not used. can be used for any other useful data.
        glm::mat4 normal_matrix = glm::mat4(tmp);

        return {
            in_transform.Get_transform_matrix(),
            normal_matrix
        };
    }

}
