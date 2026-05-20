#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec4 vPos;

void main() {
    vPos = uProjection * uView * uModel * vec4(aPos, 1.0); 
}