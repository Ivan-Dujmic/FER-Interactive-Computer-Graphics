#include "Curve.h"
#include <utility>

void Curve::init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO); {
        glBindBuffer(GL_ARRAY_BUFFER, VBO); {
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
        }
    } glBindVertexArray(0);
}

Curve::Curve() {
    init();
}

Curve::Curve(const std::vector<glm::vec3>& vertices) :
    vertices(vertices),
    VBO(0) 
{
    init();
}

Curve::Curve(std::vector<glm::vec3>&& vertices) :
    vertices(std::move(vertices)),
    VBO(0) 
{
    init();
}

Curve::Curve(Curve&& other) noexcept :
    Renderable(std::move(other)),
    vertices(std::move(other.vertices)),
    VBO(other.VBO)
{
    other.VBO = 0;
}

Curve::~Curve() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

Curve& Curve::operator=(Curve&& other) noexcept {
    if (this != &other) {
        Renderable::operator=(std::move(other));
        glDeleteBuffers(1, &VBO);
        vertices = std::move(other.vertices);
        VBO = other.VBO;
        other.VBO = 0;
    }

    return *this;
}

void Curve::setVertices(const std::vector<glm::vec3>& vertices) {
    this->vertices = vertices;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_DYNAMIC_DRAW);
}

void Curve::setVertices(std::vector<glm::vec3>&& vertices) {
    this->vertices = std::move(vertices);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_DYNAMIC_DRAW);
}

const std::vector<glm::vec3>& Curve::getVertices() const {
    return vertices;
}

void Curve::normalize() {
    // Already in world space
}

void Curve::draw() const {
    if (vertices.empty()) return;

    GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    glLineWidth(4.0f);
    glPointSize(8.0f);

    glBindVertexArray(VAO); {
        if (vertices.size() >= 2) {
            glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(vertices.size()));
        }

        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertices.size()));
    } glBindVertexArray(0);

    glLineWidth(1.0f);
    glPointSize(1.0f);
    
    if (depthWasEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
}