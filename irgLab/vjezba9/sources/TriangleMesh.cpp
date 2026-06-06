#include "TriangleMesh.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>

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
    const std::vector<GLuint> &indices,
    const Material &material,
    std::shared_ptr<Texture> diffuseTexture
) :
    vertices(vertices),
    normals(normals),
    uvCoords(uvCoords),
    indices(indices),
    material(material),
    diffuseTexture(diffuseTexture),
    VBO({0, 0, 0}),
    EBO(0)
{
    initialize();
}

TriangleMesh::TriangleMesh(
    std::vector<glm::vec3> &&vertices,
    std::vector<glm::vec3> &&normals,
    std::vector<glm::vec2> &&uvCoords,
    std::vector<GLuint> &&indices,
    const Material &&material,
    std::shared_ptr<Texture> diffuseTexture
) :
    vertices(std::move(vertices)),
    normals(std::move(normals)),
    uvCoords(std::move(uvCoords)),
    indices(std::move(indices)),
    material(std::move(material)),
    diffuseTexture(diffuseTexture),
    VBO({0, 0, 0}),
    EBO(0)
{
    initialize();
}

TriangleMesh::TriangleMesh(TriangleMesh &&other) noexcept :
    Renderable(std::move(other)),
    vertices(std::move(other.vertices)),
    normals(std::move(other.normals)),
    uvCoords(std::move(other.uvCoords)),
    indices(std::move(other.indices)),
    material(std::move(other.material)),
    diffuseTexture(other.diffuseTexture),
    VBO(other.VBO),
    EBO(other.EBO)
{
    other.VBO = {0, 0, 0};
    other.EBO = 0;
}

TriangleMesh::~TriangleMesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(3, VBO.data());
    glDeleteBuffers(1, &EBO);
}

TriangleMesh& TriangleMesh::operator=(TriangleMesh &&other) noexcept {
    if (this != &other) {
        Renderable::operator=(std::move(other));
        glDeleteBuffers(3, VBO.data());
        glDeleteBuffers(1, &EBO);
        vertices = std::move(other.vertices);
        normals = std::move(other.normals);
        uvCoords = std::move(other.uvCoords);
        indices = std::move(other.indices);
        material = std::move(other.material);
        VBO = other.VBO;
        other.VBO = {0, 0, 0};
        EBO = other.EBO;
        other.EBO = 0;
        diffuseTexture = other.diffuseTexture;
    }
    return *this;
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

const Material& TriangleMesh::getMaterial() const {
    return material;
}

std::shared_ptr<Texture> TriangleMesh::getDiffuseTexture() const {
    return diffuseTexture;
}

void TriangleMesh::normalize() {
    // TODO: Make this work with Transform or MyGLM maybe?
    auto [minV, maxV] = getBoundingBox();
    glm::vec3 center = (minV + maxV) * 0.5f;
    float M = std::max({maxV.x - minV.x, maxV.y - minV.y, maxV.z - minV.z});
    float scale = 2 / M;

    for (auto &v : vertices) {
        v = (v - center) * scale;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
}

void TriangleMesh::draw(const Shader &shader) const {
    shader.setUniform("material.ambient", material.getAmbientColor());
    shader.setUniform("material.diffuse", material.getDiffuseColor());
    shader.setUniform("material.specular", material.getSpecularColor());
    shader.setUniform("material.shininess", material.getShininess());

    bool hasTexture = diffuseTexture != nullptr && diffuseTexture->isValid();
    shader.setUniform("material.hasDiffuseTexture", hasTexture);

    if (hasTexture) {
        diffuseTexture->bind(0);
        shader.setUniform("material.diffuseTexture", 0);
    }

    glBindVertexArray(VAO); {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, NULL);
    } glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}