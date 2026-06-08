#pragma once

#include <vector>
#include <memory>
#include <glad/glad.h>

#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include "Shader.h"

class Renderer {
private:
    static constexpr unsigned int SHADOW_WIDTH = 2048;
    static constexpr unsigned int SHADOW_HEIGHT = 2048;

    std::vector<std::shared_ptr<Object>> objects;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Light> light;
    std::shared_ptr<Shader> depthShader;

    GLuint depthMapFBO = 0;
    GLuint depthMap = 0;

    void initShadowMap();
    void renderDepthPass(const glm::mat4 &lightView, const glm::mat4 &lightProjection) const;
    void renderScenePass(const glm::mat4 &lightView, const glm::mat4 &lightProjection) const;

public:
    Renderer(std::shared_ptr<Camera> c, std::shared_ptr<Light> l);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void addObject(std::shared_ptr<Object> o);
    void removeObject(std::shared_ptr<Object> o);
    void setCamera(std::shared_ptr<Camera> c);
    void setLight(std::shared_ptr<Light> l);

    void render() const;
};