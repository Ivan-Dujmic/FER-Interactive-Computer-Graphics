#pragma once

#include <glm/glm.hpp>
#include "Lines.h"

class Transform {
// TODO: should the attributes be glm::vec3 or glm::vec4 ?
protected:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 scale;

public:
    Transform();
    virtual ~Transform() = default;

    glm::mat4 getModelMatrix();
    glm::mat4 getViewMatrix();
    void rotateFPS(float offsetX, float offsetY, bool constaintPitch);
    void rotate(glm::mat4 rot);
    void globalMove(glm::mat4 delta);
    void localMove(glm::mat4 delta);
    void setOrientation();
    void setPosition(glm::vec3 p);
    void scale(glm::vec3 s);
    void update(float deltaTime);
    void registerAnimation(Lines curve);

    glm::vec3 getPosition();
    glm::vec3 getFront();
    glm::vec3 getUp();
    glm::vec3 getRight();
};