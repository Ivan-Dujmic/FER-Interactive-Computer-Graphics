#pragma once

#include <glm/glm.hpp>
#include "Transform.h"

class Light : public Transform {
private:
    glm::vec3 ambientIntensity;
    glm::vec3 diffuseIntensity;
    glm::vec3 specularIntensity;
    float left, right, bottom, top, near, far;
    float cutoffDeg, outerCutoffDeg;

public:
    Light(
        glm::vec3 position = glm::vec3(0.0f, 2.0f, 2.0f),
        glm::vec3 ambientIntensity = glm::vec3(0.25f),
        glm::vec3 diffuseIntensity = glm::vec3(0.85f),
        glm::vec3 specularIntensity = glm::vec3(1.0f),
        float left = -2.5f,
        float right = 2.5f,
        float bottom = -2.5f,
        float top = 2.5f,
        float near = 1.0f,
        float far = 100.0f,
        float cutoffDeg = 22.5f,
        float outerCutoffDeg = 30.0f
    );

    glm::vec3 getAmbientIntensity() const;
    glm::vec3 getDiffuseIntensity() const;
    glm::vec3 getSpecularIntensity() const;
    float getCutoffDeg() const;
    float getOuterCutoffDeg() const;

    void setAmbientIntensity(glm::vec3 i);
    void setDiffuseIntensity(glm::vec3 i);
    void setSpecularIntensity(glm::vec3 i);

    glm::mat4 getPerspectiveMatrix() const;
};