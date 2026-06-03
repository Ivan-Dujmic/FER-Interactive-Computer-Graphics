#pragma once

#include <glm/glm.hpp>
#include <vector>

class BezierBuilder {
private:
    static float bernstein(int i, int n, float t);
    static float binomial(int n, int k);
    static glm::vec3 evaluate(const std::vector<glm::vec3>& controlPoints, float t);

public:
    static std::vector<glm::vec3> makeControlPolygon(const std::vector<glm::vec3>& controlPoints);
    static std::vector<glm::vec3> makeApproximationCurve(const std::vector<glm::vec3>& controlPoints, int numSegments = 64);
    static std::vector<glm::vec3> makeInterpolationCurveFromLast4(const std::vector<glm::vec3>& interpolationPoints, int numSegments = 64);
    static std::vector<glm::vec3> interpolationControlPointsFromLast4(const std::vector<glm::vec3>& interpolationPoints);
};