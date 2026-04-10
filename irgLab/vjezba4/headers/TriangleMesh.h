#pragma once

#include <glm/glm.hpp>
#include "Renderable.h"
#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include <vector>
#include <utility>

class TriangleMesh : public Renderable {
private:
    // Todo should perhaps be vec4 instead of vec3
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> uvCoords;
    std::vector<int> indices;
    int VBO[3];
    int EBO;

    void applyTransform(glm::vec4 mat);
    std::pair<glm::vec3, glm::vec3> getBoundingBox();

public:
    TriangleMesh();
    ~TriangleMesh() override;

    void normalize();
    void draw() override;

    void setVertices(const std::vector<glm::vec3>& v);
    void setNormals(const std::vector<glm::vec3>& n);
    void setUVCoords(const std::vector<glm::vec3>& i);
};