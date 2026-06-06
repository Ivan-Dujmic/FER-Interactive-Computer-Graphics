#pragma once

#include <glad/glad.h>
#include "Shader.h"

class Renderable {
protected:
    GLuint VAO;

public:
    Renderable();
    Renderable(Renderable &&other) noexcept; 
    Renderable(const Renderable &other) = delete;
    virtual ~Renderable() = default;

    Renderable& operator=(Renderable &&other) noexcept;
    Renderable& operator=(const Renderable &other) = delete;

    GLuint getVAO() const;

    virtual void normalize() = 0;
    virtual void draw(const Shader &shader) const = 0;
};