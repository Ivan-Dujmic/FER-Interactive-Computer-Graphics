#pragma once

#include <glm/glm.hpp>

class Material {
private:
    glm::vec3 ambientColor; 
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    float shininess;

public:
    Material(glm::vec3 ambient = glm::vec3(0.1f),
             glm::vec3 diffuse = glm::vec3(0.8f),
             glm::vec3 specular = glm::vec3(0.2f),
             float shininess = 32.0f
            );

    glm::vec3 getAmbientColor() const;
    glm::vec3 getDiffuseColor() const;
    glm::vec3 getSpecularColor() const;
    float getShininess() const;

    void setAmbientColor(glm::vec3 c);
    void setDiffuseColor(glm::vec3 c);
    void setSpecularColor(glm::vec3 c);
    void setShininess(float s);
};