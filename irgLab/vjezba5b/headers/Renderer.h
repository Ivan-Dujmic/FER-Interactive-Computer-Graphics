#pragma once

#include <vector>

#include "Object.h"
#include "Camera.h"
// #include "Light.h"

class Renderer {
private:
    std::vector<std::shared_ptr<Object>> objects;
    std::shared_ptr<Camera> camera;

public:
    Renderer(std::shared_ptr<Camera> c);
    ~Renderer() = default;

    void addObject(std::shared_ptr<Object> o);
    void removeObject(std::shared_ptr<Object> o);
    void setCamera(std::shared_ptr<Camera> c);

    void render() const;
    // void update(float deltaTime);
};