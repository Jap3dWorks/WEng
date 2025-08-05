#pragma once

#include "WStructs/WRenderStructs.hpp"
#include <glm/glm.hpp>

namespace CameraLib {
    inline void UpdatePerpectiveProjection(WRenderCameraStruct & out_camera_struct, float in_aspect) {
        // in_camera_struct.proj = glm::perspective(
        //     glm::radians(in_camera_struct.angle_of_view),
        //     in_aspect,
        //     in_camera_struct.near_clipping,
        //     in_camera_struct.far_clipping
        //     );
        // glm::lookAt(
        //     glm::vec3(-2.f, 2.f, 2.f),
        //     glm::vec3(0.f, 0.f, 0.f),
        //     glm::vec3(0.f, 0.f, 1.f)
        //     ),
        //     glm::perspective(
        //         glm::radians(45.f),
        //         (float) width_ / (float) height_,
        //         1.f, 10.f
        //         )
    }

    inline void UpdateView(WRenderCameraStruct & out_camera_struct) {

    }

}
