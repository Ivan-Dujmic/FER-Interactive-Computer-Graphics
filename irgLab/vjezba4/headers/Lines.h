#pragma once

#include <glm/glm.hpp>
#include "Renderable.h"
#include <vector>

class Lines : public Renderable {
private:
    std::vector<glm::vec3> vertices; // TODO: vec3 or vec4 ?
    std::vector<glm::vec3> colors;  // TODO: vec3 or vec4 ?
    int VBO;

public:
    Lines();

    void addVertex(glm::vec3 v, glm::vec3 c);
    void draw();
    
    const std::vector<glm::vec3>& getVertices();
    const std::vector<glm::vec3>& getColors();
};