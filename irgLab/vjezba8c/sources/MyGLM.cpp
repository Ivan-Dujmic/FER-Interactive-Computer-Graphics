#include "MyGLM.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 MyGLM::translate3D(glm::vec3 translateVector) {
    glm::mat4 mat(0.0f);
    
    mat[0][0] = 1.0f;
    mat[1][1] = 1.0f;
    mat[2][2] = 1.0f;
    mat[3] = glm::vec4(translateVector, 1.0f);

    return mat;
}

glm::mat4 MyGLM::scale3D(glm::vec3 scaleVector) {
    glm::mat4 mat(0.0f);

    mat[0][0] = scaleVector.x;
    mat[1][1] = scaleVector.y;
    mat[2][2] = scaleVector.z;
    mat[3][3] = 1.0f;

    return mat;
}

glm::mat4 MyGLM::rotate3D(glm::vec3 axis, float angle) {
    float rad = glm::radians(angle);
    float cos = glm::cos(rad);
    float iCos = 1.0f - cos;
    float sin = glm::sin(rad);
    axis = glm::normalize(axis);

    glm::mat4 mat(0.0f);

    mat[0][0] = cos + axis.x * axis.x * iCos;
    mat[0][1] = axis.x * axis.y * iCos + axis.z * sin;
    mat[0][2] = axis.x * axis.z * iCos - axis.y * sin;
    mat[1][0] = axis.y * axis.x * iCos - axis.z * sin;
    mat[1][1] = cos + axis.y * axis.y * iCos;
    mat[1][2] = axis.y * axis.z * iCos + axis.x * sin;
    mat[2][0] = axis.z * axis.x * iCos + axis.y * sin;
    mat[2][1] = axis.z * axis.y * iCos - axis.x * sin;
    mat[2][2] = cos + axis.z * axis.z * iCos;
    mat[3][3] = 1.0f;

    return mat;
}

glm::mat4 MyGLM::lookAtMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 viewUp) {
    glm::vec3 newFront = glm::normalize(center - eye);
    glm::vec3 newRight = glm::normalize(glm::cross(newFront, viewUp));
    glm::vec3 newUp =  glm::cross(newRight, newFront);

    glm::mat4 translationMat = translate3D(-eye);
    glm::mat4 rotationMatrix(1.0f);

    rotationMatrix[0][0] = newRight.x;
    rotationMatrix[0][1] = newUp.x;
    rotationMatrix[0][2] = -newFront.x;
    rotationMatrix[1][0] = newRight.y;
    rotationMatrix[1][1] = newUp.y;
    rotationMatrix[1][2] = -newFront.y;
    rotationMatrix[2][0] = newRight.z;
    rotationMatrix[2][1] = newUp.z;
    rotationMatrix[2][2] = -newFront.z;

    return rotationMatrix * translationMat;
}

glm::mat4 MyGLM::frustum(float l, float r, float b, float t, float n, float f) {
    glm::mat4 mat(0.0f);

    mat[0][0] = (2.0f * n) / (r - l);
    mat[1][1] = (2.0f * n) / (t - b);
    mat[2][0] = (r + l) / (r - l);
    mat[2][1] = (t + b) / (t - b);
    mat[2][2] = (-f - n) / (f - n);
    mat[2][3] = -1.0f;
    mat[3][2] = (-2.0f * f * n) / (f - n);

    return mat;
}

glm::mat4 MyGLM::ortho(float l, float r, float b, float t, float n, float f) {
    glm::mat4 mat(0.0f);

    mat[0][0] = 2.0f / (r - l);
    mat[1][1] = 2.0f / (t - b);
    mat[1][3] = -(t + b) / (t - b);
    mat[2][2] = -2.0f / (f - n);
    mat[3][0] = -(r + l) / (r - l);
    mat[3][2] = -(f + n) / (f - n);
    mat[3][3] = 1.0f;

    return mat;
}