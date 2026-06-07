#pragma once

#include <glm/glm.hpp>

#define MAX_PITCH 89.0f

class Transform {
protected:
    glm::vec4 position;
    glm::vec4 front; // local z-axis
    glm::vec4 up; // local y-axis
    glm::vec4 right; // local x-axis
    glm::vec4 scaleFactor;

public:
    Transform(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
    ~Transform() = default;

    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    void setPosition(glm::vec3 p);
    void setScale(glm::vec3 s);

    glm::mat4 getModelMatrix() const;
    glm::mat4 getViewMatrix() const;
    void rotateFPS(float offsetX, float offsetY);
    void globalMove(glm::vec3 delta);
    void localMove(glm::vec3 delta);
    void setOrientation(glm::vec3 center, glm::vec3 viewUp);
};