#include "MyGLM.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 MyGLM::translate3D(glm::vec3 translateVector) {
    return glm::translate(glm::mat4(1.0f), translateVector);
}

glm::mat4 MyGLM::scale3D(glm::vec3 scaleVector) {
    return glm::scale(glm::mat4(1.0f), scaleVector);
}

glm::mat4 MyGLM::rotate3D(glm::vec3 axis, float angle) {
    return glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
}

glm::mat4 MyGLM::lookAtMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 viewUp) {
    return glm::lookAt(eye, center, viewUp);
}

glm::mat4 MyGLM::frustum(float l, float r, float b, float t, float n, float f) {
    return glm::frustum(l, r, b, t, n, f);
}