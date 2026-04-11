#pragma once

#include <vector>

#include "Object.h"
// #include "Camera.h"
// #include "Light.h"

class Renderer {
private:
    std::vector<std::shared_ptr<Object>> objects;

public:
    Renderer() = default;
    ~Renderer() = default;

    void addObject(std::shared_ptr<Object> o);
    void removeObject(std::shared_ptr<Object> o);

    void render() const;
    // void update(float deltaTime);
};