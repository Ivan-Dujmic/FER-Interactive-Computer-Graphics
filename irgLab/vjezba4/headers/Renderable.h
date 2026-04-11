#pragma once

#include <glad/glad.h>

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

    virtual void draw() const = 0;
};