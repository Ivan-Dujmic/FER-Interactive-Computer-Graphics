#include "Light.h"

Light::Light(
    glm::vec3 position,
    glm::vec3 ambientIntensity,
    glm::vec3 diffuseIntensity,
    glm::vec3 specularIntensity
) :
    Transform(position),
    ambientIntensity(ambientIntensity),
    diffuseIntensity(diffuseIntensity),
    specularIntensity(specularIntensity)
{}

glm::vec3 Light::getAmbientIntensity() const {
    return ambientIntensity;
}

glm::vec3 Light::getDiffuseIntensity() const {
    return diffuseIntensity;
}

glm::vec3 Light::getSpecularIntensity() const {
    return specularIntensity;
}

void Light::setAmbientIntensity(glm::vec3 i) {
    ambientIntensity = i;
}

void Light::setDiffuseIntensity(glm::vec3 i) {
    diffuseIntensity = i;
}

void Light::setSpecularIntensity(glm::vec3 i) {
    specularIntensity = i;
}