#pragma once

#include "WStructs/WComponentStructs.hpp"
#include "WStructs/WRenderStructs.hpp"
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
        ubo_camera.view = glm::lookAt(
            glm::vec3(-2.f, 2.f, 2.f),
            glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(0.f, 0.f, 1.f)
            );
        
        return ubo_camera;
    }

    // inline void UpdateView(WRenderCameraStruct & out_camera_struct) {
    // }

}
