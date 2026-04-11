#pragma once

#include <glm/glm.hpp>

// #include "Lines.h"

class Transform {
protected:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 scaleFactor;

public:
    Transform() = default;
    ~Transform() = default;

    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    glm::vec3 getUp() const;
    glm::vec3 getRight() const;

    glm::mat4 getModelMatrix() const;
    glm::mat4 getViewMatrix() const;
    void rotateFPS(float offsetX, float offsetY, bool constaintPitch);
    void rotate(glm::mat4 rot);
    void globalMove(glm::mat4 delta);
    void localMove(glm::mat4 delta);
    void setOrientation();
    void setPosition(glm::vec3 p);
    void scale(glm::vec3 s);
    // void update(float deltaTime);
    // void registerAnimation(Lines curve);
};