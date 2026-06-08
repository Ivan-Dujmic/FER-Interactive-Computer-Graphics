#include "Object.h"
#include <iostream>
#include <glm/gtc/constants.hpp>
#include "Light.h"

Object::Object(std::shared_ptr<Shader> shader, std::vector<std::shared_ptr<Renderable>> scene) :
    shader(std::move(shader)),
    scene(std::move(scene)),
    color(1.0f, 0.0f, 1.0f)
{}

Object::Object(std::shared_ptr<Shader> shader, std::vector<std::shared_ptr<Renderable>> scene, glm::vec3 color) :
    shader(std::move(shader)),
    scene(std::move(scene)),
    color(color)
{}

void Object::addRenderable(std::shared_ptr<Renderable> r) {
    scene.push_back(std::move(r));
}

void Object::setShader(std::shared_ptr<Shader> s) {
    shader = std::move(s);
}

void Object::setColor(glm::vec3 c) {
    color = c;
}

void Object::normalize() {
    for (auto &r : scene) {
        r->normalize();
    }
}

void Object::render(glm::mat4 viewMatrix, glm::mat4 perspectiveMatrix, const Light &light) const {
    render(viewMatrix, perspectiveMatrix, light, glm::mat4(1.0f), glm::mat4(1.0f), 0);
}

void Object::render(
    glm::mat4 viewMatrix,
    glm::mat4 perspectiveMatrix,
    const Light &light,
    glm::mat4 lightViewMatrix,
    glm::mat4 lightPerspectiveMatrix,
    unsigned int shadowMapTexture
) const {
    if (!shader) {
        std::cerr << "Object missing shader\n";
        exit(EXIT_FAILURE);
    }
    if (scene.empty()) {
        std::cerr << "Object missing scene\n";
        exit(EXIT_FAILURE);
    }

    shader->use();
    shader->setUniform("uColor", color);
    shader->setUniform("uModel", getModelMatrix());
    shader->setUniform("uView", viewMatrix);
    shader->setUniform("uProjection", perspectiveMatrix);
    shader->setUniform("uLightView", lightViewMatrix);
    shader->setUniform("uLightProjection", lightPerspectiveMatrix);

    glm::mat4 invView = glm::inverse(viewMatrix);
    glm::vec3 eye = glm::vec3(invView[3]);
    shader->setUniform("eye", eye);
    shader->setUniform("light.position", light.getPosition());
    shader->setUniform("light.direction", light.getFront());
    shader->setUniform("light.ambient", light.getAmbientIntensity());
    shader->setUniform("light.diffuse", light.getDiffuseIntensity());
    shader->setUniform("light.specular", light.getSpecularIntensity());
    shader->setUniform("light.cutOff", glm::cos(glm::radians(light.getCutoffDeg())));
    shader->setUniform("light.outerCutOff", glm::cos(glm::radians(light.getOuterCutoffDeg())));

    if (shadowMapTexture != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
        shader->setUniform("shadowMap", 1);
        shader->setUniform("useShadowMap", true);
    } else {
        shader->setUniform("useShadowMap", false);
    }

    for (const std::shared_ptr<Renderable> &r : scene) {
        r->draw(*shader);
    }
}

void Object::renderDepth(glm::mat4 lightViewMatrix, glm::mat4 lightPerspectiveMatrix, const Shader &depthShader) const {
    if (scene.empty()) return;

    depthShader.use();
    depthShader.setUniform("uModel", getModelMatrix());
    depthShader.setUniform("uLightView", lightViewMatrix);
    depthShader.setUniform("uLightProjection", lightPerspectiveMatrix);

    for (const std::shared_ptr<Renderable> &r : scene) {
        r->draw(depthShader);
    }
}