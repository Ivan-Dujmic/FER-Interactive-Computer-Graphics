#include "Renderer.h"
#include <algorithm>
#include <iostream>
#include <glm/glm.hpp>
#include "MyGLM.h"
#include "ResourceManager.h"

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
    depthShader = ResourceManager::getInstance().getShader("depth", false);
    initShadowMap();
}

Renderer::~Renderer() {
    if (depthMap != 0) glDeleteTextures(1, &depthMap);
    if (depthMapFBO != 0) glDeleteFramebuffers(1, &depthMapFBO);
}

void Renderer::initShadowMap() {
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT,
        SHADOW_WIDTH,
        SHADOW_HEIGHT,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Shadow framebuffer is not complete\n";
        exit(EXIT_FAILURE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::addObject(std::shared_ptr<Object> o) {
    objects.push_back(std::move(o));
}

void Renderer::removeObject(std::shared_ptr<Object> o) {
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

glm::mat4 Renderer::getLightProjectionMatrix() const {
    return MyGLM::frustum(-2.5f, 2.5f, -2.5f, 2.5f, 1.0f, 30.0f);
}

void Renderer::renderDepthPass(const glm::mat4 &lightView, const glm::mat4 &lightProjection) const {
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);
    for (const std::shared_ptr<Object> &o : objects) {
        o->renderDepth(lightView, lightProjection, *depthShader);
    }
    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderScenePass(const glm::mat4 &lightView, const glm::mat4 &lightProjection) const {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    for (const std::shared_ptr<Object> &o : objects) {
        o->render(
            camera->getViewMatrix(),
            camera->getPerspectiveMatrix(),
            *light,
            lightView,
            lightProjection,
            depthMap
        );
    }
}

void Renderer::render() const {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glm::mat4 lightView = light->getViewMatrix();
    glm::mat4 lightProjection = getLightProjectionMatrix();

    renderDepthPass(lightView, lightProjection);

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderScenePass(lightView, lightProjection);
}