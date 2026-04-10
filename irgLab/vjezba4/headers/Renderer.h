#pragma once

#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include <vector>

class Renderer {
private:
    Camera *camera;
    Light *light;
    std::vector<Object> objects;

public:
    Renderer(Camera* camera, Light* light);
    ~Renderer() = default;

    void render();
    void update(float deltaTime);
    void addObject(Object *obj);
    Camera* getCamera();
    Light* getLight();
};