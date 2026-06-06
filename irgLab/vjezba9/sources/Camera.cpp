#include "Camera.h"
#include "MyGLM.h"

Camera::Camera() :
    left(-0.5f),
    right(0.5f),
    bottom(-0.5f),
    top(0.5f),
    near(1.0f),
    far(100.0f)
{}
    
glm::mat4 Camera::getPerspectiveMatrix() const {
    return MyGLM::frustum(left, right, bottom, top, near, far);
}