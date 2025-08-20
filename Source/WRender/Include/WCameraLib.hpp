#pragma once

#include "WStructs/WComponentStructs.hpp"
#include "WStructs/WRenderStructs.hpp"
#include <glm/ext/quaternion_transform.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace CameraLib {
    inline WUBOCameraStruct UBOCameraStruct(
        const WCameraStruct & in_camera,
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

        glm::mat4 m{1.0};

        m = glm::rotate(m, in_transform.rotation.z, glm::vec3(0.0, 0.0, 1.0));
        m = glm::rotate(m, in_transform.rotation.x, glm::vec3(1.0, 0.0, 0.0));
        m = glm::rotate(m, in_transform.rotation.y, glm::vec3(0.0, 1.0, 0.0));

        m = glm::translate(m, in_transform.position);
        
        ubo_camera.view = m;

        // ubo_camera.view = glm::lookAt(
        //     in_transform.position,
        //     in_camera.point,
        //     glm::vec3(0.f, 1.f, 0.f)
        //     );

        ubo_camera.proj[1][1] *= -1;
        
        return ubo_camera;
    }

    // inline void UpdateView(WRenderCameraStruct & out_camera_struct) {
    // }

}
