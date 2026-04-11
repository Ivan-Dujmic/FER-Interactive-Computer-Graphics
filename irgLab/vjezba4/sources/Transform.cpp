#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 Transform::getPosition() const {
    return position;
}

glm::vec3 Transform::getFront() const {
    return front;
}

glm::vec3 Transform::getUp() const {
    return up;
}

glm::vec3 Transform::getRight() const {
    return right;
}

glm::mat4 Transform::getModelMatrix() const {
    glm::mat4 model(1.0f);

    model[0] = glm::vec4(right * scaleFactor.x, 0.0f);
    model[1] = glm::vec4(up * scaleFactor.y, 0.0f);
    model[2] = glm::vec4(front * scaleFactor.y, 0.0f);
    model[3] = glm::vec4(position, 1.0f);
    
    return model;
}

glm::mat4 Transform::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Transform::rotateFPS(float offsetX, float offsetY, bool constaintPitch) {

}

void Transform::rotate(glm::mat4 rot) {

}

void Transform::globalMove(glm::mat4 delta) {

}

void Transform::localMove(glm::mat4 delta) {

}

void Transform::setOrientation() {

}

void Transform::setPosition(glm::vec3 p) {

}

void Transform::scale(glm::vec3 s) {

}

// void Transform::update(float deltaTime);

// void Transform::registerAnimation(Lines curve);