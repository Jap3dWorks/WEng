#pragma once

#include "WStructs/WComponentStructs.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WUtils/WMathUtils.hpp"
#include <glm/ext/quaternion_transform.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace WRenderUtils {
    inline WUBOCameraStruct ToUBOCameraStruct(
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

        // glm::mat4 m = in_transform.transform;
        // m[3] *= -1;

        glm::mat4 m = WMathUtils::ToMat4(
            -in_transform.position,
            in_transform.rotation,
            in_transform.rotation_order,
            in_transform.scale
            );

        // ubo_camera.view = in_transform.transform;
        // ubo_camera.view[3] *= -1;
        // ubo_camera.view[3][3] = 1;

        ubo_camera.view = m;

        // ubo_camera.view = glm::lookAt(
        //     in_transform.position,
        //     in_camera.point,
        //     glm::vec3(0.f, 1.f, 0.f)
        //     );

        ubo_camera.proj[1][1] *= -1;
        
        return ubo_camera;
    }

    inline WUBOModelStruct ToUBOModelStruct(
        const WTransformStruct & in_transform
        ) {
        return {in_transform.transform};
    }

}
