#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Renderable.h"

class Curve : public Renderable {
private:
    std::vector<glm::vec3> vertices;
    GLuint VBO;

    void init();

public:
    Curve();
    explicit Curve(const std::vector<glm::vec3>& vertices);
    explicit Curve(std::vector<glm::vec3>&& vertices);
    Curve(Curve&& other) noexcept;
    Curve(const Curve& other) = delete;
    ~Curve();

    Curve& operator=(Curve&& other) noexcept;
    Curve& operator=(const Curve& other) = delete;

    void setVertices(const std::vector<glm::vec3>& vertices);
    void setVertices(std::vector<glm::vec3>&& vertices);
    const std::vector<glm::vec3>& getVertices() const;

    void normalize() override;
    void draw() const override;
};