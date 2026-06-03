#include "Material.h"

Material::Material(
    glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular,
    float shininess
) :
    ambientColor(ambient),
    diffuseColor(diffuse),
    specularColor(specular),
    shininess(shininess)
{}

glm::vec3 Material::getAmbientColor() const {
    return ambientColor;
}

glm::vec3 Material::getDiffuseColor() const {
    return diffuseColor;
}

glm::vec3 Material::getSpecularColor() const {
    return specularColor;
}

float Material::getShininess() const {
    return shininess;
}

void Material::setAmbientColor(glm::vec3 c) {
    ambientColor = c;
}

void Material::setDiffuseColor(glm::vec3 c) {
    diffuseColor = c;
}

void Material::setSpecularColor(glm::vec3 c) {
    specularColor = c;
}

void Material::setShininess(float s) {
    shininess = s;
}