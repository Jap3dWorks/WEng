#pragma once

#include "WCore.h"
#include <glm/glm.hpp>


class WCORE_API WTransform
{
public:
    WTransform() = default;
    virtual ~WTransform() = default;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

};
