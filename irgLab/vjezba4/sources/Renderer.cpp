#include "Renderer.h"
#include <algorithm>

void Renderer::addObject(std::shared_ptr<Object> o) {
    objects.push_back(std::move(o));
}

void Renderer::removeObject(std::shared_ptr<Object> o) {
    // Not expecting duplicate objects (otherwise use std::erase)
    auto it = std::find(objects.begin(), objects.end(), o);
    if (it != objects.end()) {
        objects.erase(it);
    }
}

void Renderer::render() const {
    for (const std::shared_ptr<Object> &o : objects) {
        o->render();
    }
}

// void Renderer::update(float deltaTime);