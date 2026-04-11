#include "Object.h"
#include <iostream>

Object::Object(std::shared_ptr<Shader> shader, std::vector<std::shared_ptr<Renderable>> renderables) :
    shader(std::move(shader)),
    renderables(std::move(renderables))
{}

void Object::addRenderable(std::shared_ptr<Renderable> r) {
    renderables.push_back(std::move(r));
}

void Object::setShader(std::shared_ptr<Shader> s) {
    shader = std::move(s);
}

void Object::render() const {
    if (!shader) {
        std::cerr << "Object missing shader\n";
        return;
    }
    if (renderables.size() == 0) {
        std::cerr << "Object missing renderables\n";
        return;
    }

    shader->use();
    shader->setUniform("uColor", glm::vec3(1.0f, 0.0f, 1.0f));

    for (const std::shared_ptr<Renderable> r : renderables) {
        r->draw();
    }
}