#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include "MyGLM.h"

Transform::Transform(glm::vec3 position) :
    position(glm::vec4(position, 1.0f)),
    front(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)),
    up(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)),
    right(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)),
    scaleFactor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f))
{}

glm::vec3 Transform::getPosition() const {
    return glm::vec3(position);
}

void Transform::setPosition(glm::vec3 p) {
    position = glm::vec4(p, 1.0f);
}

void Transform::setScale(glm::vec3 s) {
    scaleFactor = glm::vec4(s, 0.0f);
}

glm::mat4 Transform::getModelMatrix() const {
    glm::mat4 model(1.0f);

    model[0] = right * scaleFactor.x;
    model[1] = up * scaleFactor.y;
    model[2] = front * scaleFactor.z;
    model[3] = position;
    
    return model;
}

glm::mat4 Transform::getViewMatrix() const {
    return MyGLM::lookAtMatrix(glm::vec3(position), glm::vec3(position + front), glm::vec3(up));
}

void Transform::rotateFPS(float offsetX, float offsetY) {
    // Jaw (left-right) (around global up axis)
    glm::mat4 rot = MyGLM::rotate3D(glm::vec3(0.0f, 1.0f, 0.0f), -offsetX);
    front = glm::normalize(rot * front);
    up = glm::normalize(rot * up);
    right = glm::normalize(rot * right);

    // Pitch (up-down) (around local right axis)
    rot = MyGLM::rotate3D(right, offsetY);
    front = glm::normalize(rot * front);
    up = glm::normalize(rot * up);
}

// void Transform::rotate(glm::mat4 rot);

void Transform::globalMove(glm::vec3 delta) {
    position += glm::vec4(delta, 0.0f);
}

void Transform::localMove(glm::vec3 delta) {
    glm::vec3 worldDelta = glm::vec3(front) * delta.z + glm::vec3(up) * delta.y + glm::vec3(right) * delta.x;
    position += glm::vec4(worldDelta, 0.0f);
}

void Transform::setOrientation(glm::vec3 center, glm::vec3 viewUp) {
    glm::vec3 newFront = glm::normalize(center - glm::vec3(position));
    glm::vec3 newRight = glm::normalize(glm::cross(viewUp, newFront));
    glm::vec3 newUp = glm::cross(newFront, newRight);

    front = glm::vec4(newFront, 0.0f);
    right = glm::vec4(newRight, 0.0f);
    up = glm::vec4(newUp, 0.0f);
}

// void Transform::update(float deltaTime);

// void Transform::registerAnimation(Lines curve);