#pragma once

#include <vector>

#include "Object.h"
#include "Camera.h"
#include "Light.h"

class Renderer {
private:
    std::vector<std::shared_ptr<Object>> objects;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Light> light;

public:
    Renderer(std::shared_ptr<Camera> c, std::shared_ptr<Light> l);
    ~Renderer() = default;

    void addObject(std::shared_ptr<Object> o);
    void removeObject(std::shared_ptr<Object> o);
    void setCamera(std::shared_ptr<Camera> c);
    void setLight(std::shared_ptr<Light> l);

    void render() const;
};