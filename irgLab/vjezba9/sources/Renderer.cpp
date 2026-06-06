#include "Renderer.h"
#include <algorithm>
#include <iostream>

Renderer::Renderer(std::shared_ptr<Camera> c, std::shared_ptr<Light> l) {
    if (!c) {
        std::cerr << "Provided camera may not be null\n";
        exit(EXIT_FAILURE);
    }
    if (!l) {
        std::cerr << "Provided light may not be null\n";
        exit(EXIT_FAILURE);
    }
    camera = std::move(c);
    light = std::move(l);
}

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

void Renderer::setCamera(std::shared_ptr<Camera> c) {
    if (!c) {
        std::cerr << "Provided camera may not be null\n";
        exit(EXIT_FAILURE);
    }
    camera = std::move(c);
}

void Renderer::setLight(std::shared_ptr<Light> l) {
    if (!l) {
        std::cerr << "Provided light may not be null\n";
        exit(EXIT_FAILURE);
    }
    light = std::move(l);
}

void Renderer::render() const {
    for (const std::shared_ptr<Object> &o : objects) {
        o->render(camera->getViewMatrix(), camera->getPerspectiveMatrix(), *light);
    }
}