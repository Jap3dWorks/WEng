#pragma once

#include "WCore/WCore.h"
#include <glm/glm.hpp>


 struct WTransformStruct
{
public:
    WTransformStruct() = default;
    virtual ~WTransformStruct() = default;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};
