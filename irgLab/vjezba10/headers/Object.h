#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Transform.h"
#include "Shader.h"
#include "Renderable.h"
#include "Light.h"

class Object : public Transform {
private:
    std::shared_ptr<Shader> shader;
    std::vector<std::shared_ptr<Renderable>> scene;
    glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f); // Default color (magenta)

public:
    Object() = default;
    Object(std::shared_ptr<Shader> shader, std::vector<std::shared_ptr<Renderable>> scene);
    Object(std::shared_ptr<Shader> shader, std::vector<std::shared_ptr<Renderable>> scene, glm::vec3 color);
    ~Object() = default;

    void setShader(std::shared_ptr<Shader> s);
    void setColor(glm::vec3 c);
    void addRenderable(std::shared_ptr<Renderable> r);

    void normalize();

    void render(glm::mat4 viewMatrix, glm::mat4 perspectiveMatrix, const Light &light) const;
};