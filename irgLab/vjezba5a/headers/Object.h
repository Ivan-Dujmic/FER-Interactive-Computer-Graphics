#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Transform.h"
#include "Shader.h"
#include "Renderable.h"
// #include "Texture.h"
// #include "Material.h"

class Object : public Transform {
private:
    std::shared_ptr<Shader> shader;
    std::vector<std::shared_ptr<Renderable>> scene;
    // std::shared_ptr<Texture> texture;
    // std::shared_ptr<Material> material;

public:
    Object() = default;
    Object(std::shared_ptr<Shader> shader, std::vector<std::shared_ptr<Renderable>> scene);
    //
    ~Object() = default;

    void setShader(std::shared_ptr<Shader> s);
    void addRenderable(std::shared_ptr<Renderable> r);
    // void setTexture(std::shared_ptr<Texture> t);
    // void setMaterial(std::shared_ptr<Material> m);

    void normalize();

    void render(glm::mat4 viewMatrix, glm::mat4 perspectiveMatrix) const;
};