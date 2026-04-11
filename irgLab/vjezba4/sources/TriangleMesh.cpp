#include "TriangleMesh.h"
#include <glm/glm.hpp>

void TriangleMesh::initialize() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBO.data());
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO); {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]); {
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]); {
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]); {
            glBufferData(GL_ARRAY_BUFFER, uvCoords.size() * sizeof(glm::vec2), uvCoords.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
        }
    } glBindVertexArray(0);
}

std::pair<glm::vec3, glm::vec3> TriangleMesh::getBoundingBox() const {
    glm::vec3 minV = vertices[0];
    glm::vec3 maxV = vertices[0];

    for (const glm::vec3& v : vertices) {
        minV = glm::min(v, minV);
        maxV = glm::max(v, maxV);
    }

    return {minV, maxV};
}

void TriangleMesh::applyTransform(const glm::vec4 &mat) {
    for (glm::vec3& v : vertices) {
        v = glm::vec3(mat * glm::vec4(v, 1.0f));
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
}

TriangleMesh::TriangleMesh(
    const std::vector<glm::vec3> &vertices,
    const std::vector<glm::vec3> &normals,
    const std::vector<glm::vec2> &uvCoords,
    const std::vector<GLuint> &indices
) :
    vertices(vertices),
    normals(normals),
    uvCoords(uvCoords),
    indices(indices),
    VBO({0, 0, 0}),
    EBO(0)
{
    initialize();
}

TriangleMesh::TriangleMesh(
    std::vector<glm::vec3> &&vertices,
    std::vector<glm::vec3> &&normals,
    std::vector<glm::vec2> &&uvCoords,
    std::vector<GLuint> &&indices
) :
    vertices(std::move(vertices)),
    normals(std::move(normals)),
    uvCoords(std::move(uvCoords)),
    indices(std::move(indices)),
    VBO({0, 0, 0}),
    EBO(0)
{
    initialize();
}

TriangleMesh::TriangleMesh(TriangleMesh &&other) noexcept :
    vertices(std::move(other.vertices)),
    normals(std::move(other.normals)),
    uvCoords(std::move(other.uvCoords)),
    indices(std::move(other.indices)),
    VBO(std::move(other.VBO)),
    EBO(other.EBO)
{
    other.EBO = 0;
}

TriangleMesh::~TriangleMesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(3, VBO.data());
    glDeleteBuffers(1, &EBO);
}

TriangleMesh& TriangleMesh::operator=(TriangleMesh &&other) noexcept {
    vertices = std::move(other.vertices);
    normals = std::move(other.normals);
    uvCoords = std::move(other.uvCoords);
    indices = std::move(other.indices);
    VBO = std::move(other.VBO);
    EBO = other.EBO;
    other.EBO = 0;
}

const std::vector<glm::vec3>& TriangleMesh::getVertices() const {
    return vertices;
}

const std::vector<glm::vec3>& TriangleMesh::getNormals() const {
    return normals;
}

const std::vector<glm::vec2>& TriangleMesh::getUVCoords() const {
    return uvCoords;
}

const std::vector<GLuint>& TriangleMesh::getIndices() const {
    return indices;
}

void TriangleMesh::normalize() {
    // TODO
}

void TriangleMesh::draw() const {
    glBindVertexArray(VAO); {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Drawing wireframes
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset (because it's a global state)
    } glBindVertexArray(0);
}