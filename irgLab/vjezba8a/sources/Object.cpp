#include "Object.h"
#include <iostream>
#include "Light.h"

Object::Object(std::shared_ptr<Shader> shader, std::vector<std::shared_ptr<Renderable>> scene) :
    shader(std::move(shader)),
    scene(std::move(scene)),
    color(1.0f, 0.0f, 1.0f)
{}

Object::Object(std::shared_ptr<Shader> shader, std::vector<std::shared_ptr<Renderable>> scene, glm::vec3 color) :
    shader(std::move(shader)),
    scene(std::move(scene)),
    color(color)
{}

void Object::addRenderable(std::shared_ptr<Renderable> r) {
    scene.push_back(std::move(r));
}

void Object::setShader(std::shared_ptr<Shader> s) {
    shader = std::move(s);
}

void Object::setColor(glm::vec3 c) {
    color = c;
}

void Object::normalize() {
    for (auto &r : scene) {
        r->normalize();
    }
}

void Object::render(glm::mat4 viewMatrix, glm::mat4 perspectiveMatrix, const Light &light) const {
    if (!shader) {
        std::cerr << "Object missing shader\n";
        exit(EXIT_FAILURE);
    }
    if (scene.size() == 0) {
        std::cerr << "Object missing scene\n";
        exit(EXIT_FAILURE);
    }

    shader->use();
    shader->setUniform("uColor", color);
    shader->setUniform("uModel", getModelMatrix());
    shader->setUniform("uView", viewMatrix);
    shader->setUniform("uProjection", perspectiveMatrix);
    glm::mat4 invView = glm::inverse(viewMatrix);
    glm::vec3 eye = glm::vec3(invView[3]);
    shader->setUniform("eye", eye);
    shader->setUniform("light.position", light.getPosition());
    shader->setUniform("light.ambient", light.getAmbientIntensity());
    shader->setUniform("light.diffuse", light.getDiffuseIntensity());
    shader->setUniform("light.specular", light.getSpecularIntensity());

    for (const std::shared_ptr<Renderable> &r : scene) {
        r->draw(*shader);
    }
}