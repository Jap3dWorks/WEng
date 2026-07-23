#pragma once

#include "WComponents/Camera.hpp"
#include "WComponents/Transform.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WUtils/WMath.hpp"
#include <glm/ext/quaternion_transform.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/matrix.hpp>

namespace wrd::render {

    inline wct::render::CameraUBO ToUBOCameraStruct(
        wcm::Camera const & camera_component,
        wcm::Transform const & transform_component,
        float in_aspect
        ) {
        wct::render::CameraUBO ubo_camera;

        ubo_camera.proj = glm::perspective(
            camera_component.Get_field_of_view(),
            in_aspect,
            camera_component.Get_near_clipping(),
            camera_component.Get_far_clipping()
            );

        glm::mat3 orient{transform_component.Get_transform_matrix()};
        glm::mat4 o = glm::transpose(orient);

        glm::vec3 translation{transform_component.Get_transform_matrix()[3]};
        glm::mat4 t = glm::translate(glm::mat4{1}, -translation);

        ubo_camera.view = o * t;
        ubo_camera.proj[1][1] *= -1;
        ubo_camera.inv_view = glm::inverse(ubo_camera.view);
        ubo_camera.inv_proj = glm::inverse(ubo_camera.proj);

        ubo_camera.pos = transform_component.Get_position();
        ubo_camera.rot = transform_component.Get_rotation();
        ubo_camera.scale = transform_component.Get_scale();
        ubo_camera.near_clipping = camera_component.Get_near_clipping();
        ubo_camera.far_clipping = camera_component.Get_far_clipping();
        
        return ubo_camera;
    }

    inline wct::render::ModelUBO ToUBOGraphicsStruct(
        wcm::Transform const & in_transform
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
