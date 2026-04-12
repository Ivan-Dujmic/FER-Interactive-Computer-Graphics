#pragma once

#include <glm/glm.hpp>

// #include "Lines.h"

class Transform {
protected:
    glm::vec4 position;
    glm::vec4 front; // local z-axis
    glm::vec4 up; // local y-axis
    glm::vec4 right; // local x-axis
    glm::vec4 scaleFactor;

public:
    Transform() = default;
    ~Transform() = default;

    glm::mat4 getModelMatrix() const;
    glm::mat4 getViewMatrix() const;
    void rotateFPS(float offsetX, float offsetY, bool constaintPitch = true);
    // void rotate(glm::mat4 rot);
    void globalMove(glm::vec3 delta);
    void localMove(glm::vec3 delta);
    void setOrientation(glm::vec3 center, glm::vec3 viewUp);
    void setPosition(glm::vec3 p);
    // void scale(glm::vec3 s);
    // void update(float deltaTime);
    // void registerAnimation(Lines curve);
};