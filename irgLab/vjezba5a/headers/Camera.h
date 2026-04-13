#pragma once

#include "Transform.h"

class Camera : public Transform {
private:
    float left, right, bottom, top, near, far;

public:
    Camera();
    
    glm::mat4 getPerspectiveMatrix() const;
};