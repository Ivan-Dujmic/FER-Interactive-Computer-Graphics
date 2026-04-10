#pragma once

#include <glm/glm.hpp>
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Light.h"
#include "Renderable.h"
#include <vector>

class Object : public Transform {
protected:
    Shader *shader;
    Material *material;
    Texture *texture;
    std::vector<Renderable*> renderables;

public:
    Object(Shader *shader, Material *material = nullptr, Texture *texture = nullptr);
    virtual ~Object() = default;

    virtual void render(glm::mat4 perspectiveMatrix, glm::mat4 viewMatrix, Light *light) = 0;

    void setMaterial(Material *m);
    void setShader(Shader *s);
    void setTexture(Texture *t);

    Material* getMaterial();
    Shader* getShader();
    Texture* getTexture();
};