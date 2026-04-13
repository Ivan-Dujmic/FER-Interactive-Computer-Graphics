#include "Object.h"
#include <iostream>

Object::Object(std::shared_ptr<Shader> shader, std::vector<std::shared_ptr<Renderable>> scene) :
    shader(std::move(shader)),
    scene(std::move(scene))
{}

void Object::addRenderable(std::shared_ptr<Renderable> r) {
    scene.push_back(std::move(r));
}

void Object::setShader(std::shared_ptr<Shader> s) {
    shader = std::move(s);
}

void Object::normalize() {
    for (auto &r : scene) {
        r->normalize();
    }
}

void Object::render(glm::mat4 viewMatrix, glm::mat4 perspectiveMatrix) const {
    if (!shader) {
        std::cerr << "Object missing shader\n";
        exit(EXIT_FAILURE);
    }
    if (scene.size() == 0) {
        std::cerr << "Object missing scene\n";
        exit(EXIT_FAILURE);
    }

    shader->use();
    shader->setUniform("uColor", glm::vec3(1.0f, 0.0f, 1.0f));
    shader->setUniform("uModel", getModelMatrix());
    shader->setUniform("uView", viewMatrix);
    shader->setUniform("uProjection", perspectiveMatrix);

    for (const std::shared_ptr<Renderable> &r : scene) {
        r->draw();
    }
}