#include "Renderable.h"

Renderable::Renderable() : 
    VAO(0)
{}

Renderable::Renderable(Renderable &&other) noexcept :
    VAO(other.VAO)
{
    other.VAO = 0;
}

Renderable& Renderable::operator=(Renderable &&other) noexcept {
    if (this != &other) {
        VAO = other.VAO;
        other.VAO = 0;
    }
    return *this;
}

GLuint Renderable::getVAO() const {
    return VAO;
}