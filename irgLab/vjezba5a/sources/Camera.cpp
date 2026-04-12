#include "Camera.h"
#include "MyGLM.h"

Camera::Camera() :
    left(-1.0f),
    right(1.0f),
    bottom(-1.0f),
    top(1.0f),
    near(0.1f),
    far(100.0f)
{}
    
glm::mat4 Camera::getPerspectiveMatrix(glm::vec2 screenSize) const {
    return MyGLM::frustum(left, right, bottom, top, near, far);
}