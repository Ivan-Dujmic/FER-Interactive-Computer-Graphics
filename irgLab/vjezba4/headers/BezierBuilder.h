#pragma once

#include "Lines.h"
#include <vector>

class BezierBuilder {
public:
    Lines* makeInterpolationCurve(const std::vector<glm::vec3>& points, int segments = 32);
    Lines* makeApproximationCurve(const std::vector<glm::vec3>& points, int segments = 32);
    Lines* getControlPolygon(const std::vector<glm::vec3>& points);
};