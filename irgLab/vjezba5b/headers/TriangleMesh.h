#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <utility>
#include "Renderable.h"
#include "Shader.h"
// #include "Material.h"
// #include "Texture.h"

class TriangleMesh : public Renderable {
private:
    std::vector<glm::vec3> vertices; // VBO[0]
    std::vector<glm::vec3> normals; // VBO[1]
    std::vector<glm::vec2> uvCoords; // VBO[2]
    std::vector<GLuint> indices;
    std::array<GLuint, 3> VBO;
    GLuint EBO;

    void initialize();

    std::pair<glm::vec3, glm::vec3> getBoundingBox() const;

    void applyTransform(const glm::vec4 &mat);

public:
    TriangleMesh(
        const std::vector<glm::vec3> &vertices,
        const std::vector<glm::vec3> &normals,
        const std::vector<glm::vec2> &uvCoords,
        const std::vector<GLuint> &indices
    );
    TriangleMesh(
        std::vector<glm::vec3> &&vertices,
        std::vector<glm::vec3> &&normals,
        std::vector<glm::vec2> &&uvCoords,
        std::vector<GLuint> &&indices
    );
    TriangleMesh(const TriangleMesh &other) = delete;
    TriangleMesh(TriangleMesh &&other) noexcept;

    ~TriangleMesh() override;

    TriangleMesh& operator=(const TriangleMesh &other) = delete;
    TriangleMesh& operator=(TriangleMesh &&other) noexcept;

    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<glm::vec3>& getNormals() const;
    const std::vector<glm::vec2>& getUVCoords() const;
    const std::vector<GLuint>& getIndices() const;

    // TODO: do we really need the setters?
    // void setVertices(const std::vector<glm::vec3>& v);
    // void setNormals(const std::vector<glm::vec3>& n);
    // void setUVCoords(const std::vector<glm::vec3>& i);

    void normalize() override;
    void draw() const override;
};