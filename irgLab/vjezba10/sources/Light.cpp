#include "Light.h"
#include "MyGLM.h"

Light::Light(
    glm::vec3 position,
    glm::vec3 ambientIntensity,
    glm::vec3 diffuseIntensity,
    glm::vec3 specularIntensity,
    float left,
    float right,
    float bottom,
    float top,
    float near,
    float far,
    float cutoffDeg,
    float outerCutoffDeg
) :
    Transform(position),
    ambientIntensity(ambientIntensity),
    diffuseIntensity(diffuseIntensity),
    specularIntensity(specularIntensity),
    left(left),
    right(right),
    bottom(bottom),
    top(top),
    near(near),
    far(far),
    cutoffDeg(cutoffDeg),
    outerCutoffDeg(outerCutoffDeg)
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

float Light::getCutoffDeg() const {
    return cutoffDeg;
}

float Light::getOuterCutoffDeg() const {
    return outerCutoffDeg;
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

glm::mat4 Light::getPerspectiveMatrix() const {
    return MyGLM::frustum(left, right, bottom, top, near, far);
}