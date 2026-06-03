#include "BezierBuilder.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <algorithm>
#include <cmath>

float BezierBuilder::bernstein(int i, int n, float t) {
    return binomial(n, i) * std::pow(t, static_cast<float>(i)) * std::pow(1.0f - t, static_cast<float>(n - i));
}

float BezierBuilder::binomial(int n, int k) {
    if (k < 0 || k > n) {
        return 0.0f;
    }
    if (k == 0 || k == n) {
        return 1.0f;
    }

    k = std::min(k, n - k);
    float result = 1.0f;
    for (int i = 1; i <= k; ++i) {
        result *= static_cast<float>(n - k + i);
        result /= static_cast<float>(i);
    }

    return result;
}

glm::vec3 BezierBuilder::evaluate(const std::vector<glm::vec3>& controlPoints, float t) {
    int n = static_cast<int>(controlPoints.size()) - 1;
    glm::vec3 p(0.0f);

    for (int i = 0 ; i <= n ; i++) {
        p += bernstein(i, n, t) * controlPoints[i];
    }

    return p;
}

std::vector<glm::vec3> BezierBuilder::makeControlPolygon(const std::vector<glm::vec3>& controlPoints) {
    return controlPoints;
}

std::vector<glm::vec3> BezierBuilder::makeApproximationCurve(const std::vector<glm::vec3>& controlPoints, int numSegments) {
    std::vector<glm::vec3> curve;
    if (controlPoints.empty()) return curve;

    numSegments = std::max(numSegments, 2);
    curve.reserve(numSegments);

    for (int i = 0 ; i < numSegments ; i++) {
        float t = static_cast<float>(i) / static_cast<float>(numSegments - 1);
        curve.push_back(evaluate(controlPoints, t));
    }

    return curve;
}

std::vector<glm::vec3> BezierBuilder::makeInterpolationCurveFromLast4(const std::vector<glm::vec3>& interpolationPoints, int numSegments) {
    std::vector<glm::vec3> controlPoints = interpolationControlPointsFromLast4(interpolationPoints);
    return makeApproximationCurve(controlPoints, numSegments);
}

std::vector<glm::vec3> BezierBuilder::interpolationControlPointsFromLast4(const std::vector<glm::vec3>& interpolationPoints) {
    if (interpolationPoints.size() < 4) {
        return {};
    }

    std::vector<glm::vec3> p(interpolationPoints.end() - 4, interpolationPoints.end());

    glm::mat4 F(1.0f);
    for (int row = 0 ; row < 4 ; row++) {
        float t = static_cast<float>(row) / 3.0f;
        for (int col = 0 ; col < 4 ; col++) {
            F[col][row] = bernstein(col, 3, t);
        }
    }

    glm::mat4 invF = glm::inverse(F);
    glm::vec4 px(p[0].x, p[1].x, p[2].x, p[3].x);
    glm::vec4 py(p[0].y, p[1].y, p[2].y, p[3].y);
    glm::vec4 pz(p[0].z, p[1].z, p[2].z, p[3].z);

    glm::vec4 ax = invF * px;
    glm::vec4 ay = invF * py;
    glm::vec4 az = invF * pz;

    return {
        glm::vec3(ax.x, ay.x, az.x),
        glm::vec3(ax.y, ay.y, az.y),
        glm::vec3(ax.z, ay.z, az.z),
        glm::vec3(ax.w, ay.w, az.w)
    };
}